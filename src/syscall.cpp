/*
 *  syscall.cpp:
 *      - Implementation of function SysCall() for logged 
 *        execution of system commands.
 */
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "include/log.h"
#include "include/syscall.h"

using namespace std;

static const bool sandboxMode = false;

int SysCall(const char *str, bool logOutput) {
    if (!sandboxMode) {
        int ret;

        if (logOutput) {
            char buffer[256];
            string cmd = str;
            cmd += " 2>&1";

            log << CMMD << cmd << std::endl;
            FILE *pipe = popen(cmd.c_str(), "r");

            if (!pipe) {
                log << ERRR << "Error creating pipe for the last command." << std::endl;
                return -1;
            }

            while (!feof(pipe))
                if (fgets(buffer, sizeof(buffer), pipe))
                    log << buffer;

            ret = pclose(pipe);
        } else {
            log << CMMD << str << std::endl;
            ret = system(str);
        }

        if (ret < 0 || ret == 127)
            log << ERRR << "Error executing the last command." << std::endl;
        else if (ret != 0)
            log << ERRR << "Error while executing the last command." << std::endl;
        else
            log << INFO << "The operation completed successfully." << std::endl;

        return ret;
    } else {
        log << CMMD << str << std::endl;
        return 0;
    }
}


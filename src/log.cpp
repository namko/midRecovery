/*
 *  log.cpp:
 *      - Implementation of MID recovery logger.
 */
#include <fstream>
#include <string.h>

#include "include/config.h"
#include "include/log.h"

using namespace std;

ofstream log;

string Log::logPath = "";
bool Log::isInternal = false;

bool Log::Init(const int argc, const char *argv[]) {
    if (argc != 1)
        logPath = argv[1];
    else
        logPath = "log.txt";
    
    log.open(logPath.c_str());

    if (!log.fail())
        if (argc != 1) {
            isInternal = (argv[1] == strstr(argv[1], MOUNT_INTSD));
            log << INFO << "Begin log on " << (isInternal ? "internal" : "external")
                    << " SD card." << endl;
        } else {
            isInternal = false;
            log << INFO << "Begin log on ram-disk." << endl;
        }

    return !log.fail();
}

void Log::Flush() {
    log.flush();
}

void Log::Close() {
    log << INFO << "End log." << endl;
    log.flush();
    log.close();
}

bool Log::IsInternalSD() {
    return isInternal;
}

const char *Log::GetPath() {
    return logPath.c_str();
}


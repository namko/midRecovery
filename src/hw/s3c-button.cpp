/*
 *  s3c-button.cpp:
 *      - Implementation of class for input from s3c-button.
 */
#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <linux/input.h>

#include "../include/log.h"
#include "../include/Window.h"
#include "s3c-button.h"

using namespace std;

bool S3CButton::init = false;

int S3CButton::fd = 0;

bool S3CButton::Initialize() {
    if (init)
        return init;

    for (int i = 0; i < 20; i++) {
        char filename[40];
        sprintf(filename, "/dev/input/event%d", i);

        if ((fd = open(filename, O_RDONLY)) < 0)
            continue;

    	char name[256] = {NULL};
    	ioctl(fd, EVIOCGNAME(sizeof(name)), name);

        if (strcmp(name, "s3c-button")) {
            close(fd);
            continue;
        }

        init = true;
        break;
    }

    return init;
}

// Function for waiting for, and returning a button press.
WindowInput S3CButton::GetKeyPress() {
    if (!init) {
        log << ERRR << "Trying to read button without initialization!" << endl;
        return WindowInput(-1);
    }

	while (1) {
	    struct input_event ev;
		int rd = read(fd, &ev, sizeof(struct input_event));

		if (rd != (int)sizeof(struct input_event)) {
            log << ERRR << "Received partial event!" << endl;
			return WindowInput(-1);
        }

		if (ev.type == EV_KEY && ev.value != 0) {
            switch (ev.code) {
            case KEY_HOME:
                return WI_SELECT;
            case KEY_MENU:
                return WI_UP;
            case KEY_BACK:
                return WI_DOWN;
            }
        }
    }
}


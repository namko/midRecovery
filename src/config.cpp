/*
 *  config.cpp:
 *      - Implementation of MID Recovery configuration.
 */
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <linux/input.h>

#include "include/config.h"
#include "include/log.h"
#include "hw/mtd.h"

#if TARGET == 703
#include "hw/s3c-button.h"
#endif

using namespace std;

static char defaultPath[PATH_MAX];

void ConfigInit(const int argc, const char *argv[]) {
    log << "Starting MID recovery " << RECOVERY_VERSION 
        << "..." << endl;

    if (argc != 1 &&  argv[1] == strstr(argv[1], MOUNT_INTSD))
        // Internal SD card
        strcpy(defaultPath, MOUNT_INTSD);
    else if (argc != 1 &&  argv[1] == strstr(argv[1], MOUNT_SDCARD))
        // External SD card
        strcpy(defaultPath, MOUNT_SDCARD);
    else
        // Ram-disk
        strcpy(defaultPath, "/");

    MTD::Init();

#if TARGET == 703
    if (!S3CButton::Initialize()) {
        log << ERRR << "Problem initializing s3c-button." << endl;
        exit(2);
    }
#endif
}

void ConfigDeInit() {
}

int GetButtonPress() {
    switch (WINDOW_INPUT_FN()) {
    case WI_SELECT:
        return KEY_HOME;
    case WI_UP:
        return KEY_MENU;
    case WI_DOWN:
        return KEY_BACK;
    }
}

const char *GetDefaultPath() {
    return defaultPath;
}


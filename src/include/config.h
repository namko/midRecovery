/*
 *  config.h:
 *      - MID Recovery configuration.
 */
#ifndef __CONFIG_H_
#define __CONFIG_H_

#include <iostream>
#include "Window.h"

void ConfigInit(const int argc, const char *argv[]);
void ConfigDeInit();

int GetButtonPress();
const char *GetDefaultPath();

static const char *MOUNT_NAND = "/mnt/nand";
static const char *MOUNT_INTSD = "/mnt/intsd";
static const char *MOUNT_CACHE = "/mnt/cache";
static const char *MOUNT_DATA = "/mnt/data";
static const char *MOUNT_SYSTEM = "/mnt/system";
static const char *MOUNT_SDCARD = "/mnt/sdcard";
static const char *MOUNT_ROOT = "/mnt/root";
static const char *MOUNT_TMP = "/mnt/tmp";

static const char *MOUNT_DATA_DALVIK = "/mnt/data/dalvik-cache";
static const char *MOUNT_ROOT_SYSTEM = "/mnt/root/system";

static const char *FS_CACHE = "ext4";
static const char *FS_DATA = "ext4";
static const char *FS_SYSTEM = "ext4";

static const int UBID_NUMBER = 0;
static const int UBIV_NUMBER = 0;
static const char *UBIV_NAME = "rootfs";
static const char *DEV_NAND = "ubi0:rootfs"; // this must be "ubi$UBID_NUMBER:$UBIV_NAME"
static const char *DEV_UBI = "/dev/ubi";

static const char *DEV_INTSD = "/dev/mmcblk0";
static const char *DEV_INTSDP = "/dev/mmcblk0p1";
static const char *DEV_CACHE = "/dev/mmcblk0p2";
static const char *DEV_DATA = "/dev/mmcblk0p3";
static const char *DEV_SYSTEM = "/dev/mmcblk0p4";

static const char *SYS_INTSD = "/sys/block/mmcblk0";
static const char *SYS_INTSDP = "/sys/block/mmcblk0/mmcblk0p1";
static const char *SYS_CACHE = "/sys/block/mmcblk0/mmcblk0p2";
static const char *SYS_DATA = "/sys/block/mmcblk0/mmcblk0p3";
static const char *SYS_SYSTEM = "/sys/block/mmcblk0/mmcblk0p4";

#if TARGET == PC
    // Read character from "stdin" and convert to response.
    static WindowInput WindowDefaultInputFn() {
        char ch;
        std::cin >> ch;

        if (ch == '+')
            return WI_DOWN;
        else if (ch == '-')
            return WI_UP;
        else
            return WI_SELECT;
    }

    // For builds on PC, use the default input function.
    static const WindowInputFn WINDOW_INPUT_FN = WindowDefaultInputFn;

    // Maximum "inner" width and height of the window.
    static const int MAX_INNER_WIDTH = 80;
    static const int MAX_INNER_HEIGHT = 20;

#elif TARGET == 703
    #include "../hw/s3c-button.h"

    // For builds on MID703, use s3c-button input function.
    static const WindowInputFn WINDOW_INPUT_FN = S3CButton::GetKeyPress;

    // Maximum "inner" width and height of the window.
    static const int MAX_INNER_WIDTH = 100;
    static const int MAX_INNER_HEIGHT = 25;

#else
    // ERROR
    #error Define TARGET before compiling.

#endif

#endif  //  __CONFIG_H_


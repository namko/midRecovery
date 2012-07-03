/*
 *  mtd.cpp:
 *      - MTD device definition.
 */
#include <stdlib.h>
#include "mtd.h"

struct MTD gMTDs[MTD_MAX + 1];

MTD::MTD() { }

MTD::MTD(const char *name, const char *device, 
        const char *sysfs, int number,
        int start, int size, const char *filename, 
        const char *filepattern) {
    this->name = name;
    this->device = device;
    this->sysfs = sysfs;
    this->number = number;
    this->start = start;
    this->size = size;
    this->filename = filename;
    this->filepattern = filepattern;
}

void MTD::Init() {
#if TARGET == 703
    gMTDs[MTD_BOOTLOADER] = MTD("bootloader",   "/dev/mtd6",    "/sys/block/mtdblock6", 6,  0x00000000, 0x00080000, "u-boot.bin",   "u-boot.bin");
    gMTDs[MTD_BOOTARGS] =   MTD("bootargs",     "/dev/mtd0",    "/sys/block/mtdblock0", 0,  0x00080000, 0x00080000, "u-boot.arg",   "u-boot.arg");
    gMTDs[MTD_RECOVERY] =   MTD("recovery",     "/dev/mtd1",    "/sys/block/mtdblock1", 1,  0x00100000, 0x00800000, "recovery",     "recovery"  );
    gMTDs[MTD_KERNEL] =     MTD("kernel",       "/dev/mtd2",    "/sys/block/mtdblock2", 2,  0x00900000, 0x00600000, "zImage",       "zImage"    );
    gMTDs[MTD_LOGO] =       MTD("logo",         "/dev/mtd3",    "/sys/block/mtdblock3", 3,  0x00F00000, 0x00400000, "logo.bmp",     "*.bmp"     );
    gMTDs[MTD_PARAM] =      MTD("param",        "/dev/mtd5",    "/sys/block/mtdblock5", 5,  0x01300000, 0x00200000, "param",        "param"     );
    gMTDs[MTD_ROOTFS] =     MTD("rootfs",       "/dev/mtd4",    "/sys/block/mtdblock4", 4,  0x01500000, 0xFFFFFFFF, NULL,           NULL        );
#endif
}


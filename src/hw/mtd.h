/*
 *  mtd.h:
 *      - MTD device definition.
 */
#ifndef __MTD_H_
#define __MTD_H_

enum MTD_Index {
    MTD_BOOTLOADER,
    MTD_BOOTARGS,
    MTD_RECOVERY,
    MTD_KERNEL,
    MTD_LOGO,
    MTD_PARAM,
    MTD_ROOTFS,

    MTD_MAX = MTD_ROOTFS,
};

class MTD {
public:
    const char *name;
    const char *device;
    const char *sysfs;
    int number;
    int start;
    int size;
    const char *filename;
    const char *filepattern;

    MTD();
    MTD(const char *name, const char *device, 
        const char *sysfs, int number,
        int start, int size, const char *filename, 
        const char *filepattern);

    static void Init();
};

extern MTD gMTDs[];

#endif  //  __MTD_H_

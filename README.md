Custom recovery for MID 703 and equivalent tablets.
===================================================

Steps for compiling recovery kernel+ramdisk combo
-------------------------------------------------
Edit "Makefile" to update the following:
* ARM-CXX -- Path for ARM C++ cross compiler.
* KERNELDIR -- Path to the kernel used for building recovery.

For MID 703 and equivalent tablets:
    make clean
    make recovery_clean
    make -j4 recovery_703

Steps for compiling binary only
-------------------------------
For emulator (PC build):
    make clean
    make -j4

For MID 703:
    make clean
    make -j4 703


Custom recovery for MID 703 and equivalent tablets.
===================================================

Steps for compiling recovery kernel+ramdisk combo
-------------------------------------------------
Edit "Makefile" to update the following:
* ARM-CXX -- Path for ARM C++ cross compiler.
* KERNELDIR -- Path to the kernel used for building recovery.

In this mode the entire recovery (consisting of the kernel and ramdisk combo) is built. First, the binary is built (out/midRecovery); then the CPIO for ramdisk is built (out/recovery.cpio). Finally, the appropriate configuration is copied to the kernel directory with the CPIO and after the build process is complete, the zImage is copied to the recovery output folder (out/recovery).

###To build for Herotab C8/Dropad A8/Haipad M7/iBall Slide i7011 and compatibles (MID 703):

    make recovery_703

###To clean the kernel directory:
In addition to cleaning the kernel directory, this also removes the files added by the recovery build process (e.g. "recovery.cpio").

    make recovery_clean

###To clean the recovery output directory:

    make clean

Steps for compiling binary only
-------------------------------
Edit "Makefile" to update the following if not building for PC:
* ARM-CXX -- Path for ARM C++ cross compiler.

In this mode only the (static) binary is built. The ramdisk and recovery combo is not built.

###To build for Herotab C8/Dropad A8/Haipad M7/iBall Slide i7011 and compatibles (MID 703):

    make 703

###To build for PC (emulation):

    make

###To clean the recovery output directory:

    make clean


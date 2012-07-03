/*
 *  common.cpp:
 *      - Utility functions.
 */

// Executes a shell script (by executing the command "sh $str $1 $2 $2 ...").
static bool ExecuteShellScript(const char *str, int numArgs = 0, ...) {
    string 
    cmd = "sh ";
    cmd += str;

    if (numArgs > 0) {
        va_list args;
        va_start(args, numArgs);

        for (int i = 0; i < numArgs; i++) {
            cmd += " ";
            cmd += va_arg(args, const char *);
        }
 
        va_end(args);
    }

    return ExecuteAndNotifyIfFail(cmd.c_str());
}

// Returns the size of the specified block device, or 0 on error.
// This will open "$blockDev/size" to determine.
static size_t GetBlockDeviceSize(const char *blockDev) {
    string 
    path = blockDev;
    path += "/size";

    size_t out = 0;
    ifstream in(path.c_str());

    if (in.fail())
        goto fail;

    in >> out;

    if (in.fail())
        goto fail;
    else
        goto success;

fail:
    out = 0;

success:
    if (in.is_open())
        in.close();

    return out;
}

// Returns the free space on the given mountpoint or 0 on error.
// This will call statvfs() to determine.
static size_t GetMountpointFreeSpace(const char *path) {
    struct statvfs buf;

    if (statvfs(path, &buf))
        return 0;

    return size_t(buf.f_bsize) * buf.f_bavail;
}

// First unmounts normally, and then lazy unmounts if there is a problem.
// There is a "cout" message with lazy-unmount.
inline bool UnmountA(const char *mountpoint) {
    if (!Unmount(mountpoint)) {
        cout << "Unmounting... (lazy-mode)" << endl;
        return Unmount(mountpoint, true);
    }

    return true;
}

// Formats the specified device, using the input filesystem string
// to decide which function to call. If unable to determine, false
// is returned with "*formatted" = false. Otherwise, the value returned
// by the formatting function is returned with "*formatted" = true.
inline bool Format(const char *dev, const char *fs, bool *formatted = NULL) {
    if (formatted)
        *formatted = true;

    if (!strcmp(fs, "vfat"))
        return FormatFat(dev);
    else if (!strcmp(fs, "ext4"))
        return FormatExt(dev, 4);
    else if (!strcmp(fs, "ext3"))
        return FormatExt(dev, 3);
    else if (!strcmp(fs, "ext2"))
        return FormatExt(dev, 2);

    if (formatted)
        *formatted = false;

    return false;
}

// Mounts a device, archives the contents and then unmounts it.
// The mount *MUST* be read-only (as specified in "opts").
// Also, the backup must not reside on the mountpoint.
static bool BackupMountpoint(const char *tgz, const char *dev, 
        const char *mountpoint, const char *fs = NULL, 
        const char *opts = NULL) {

    bool failed;

    cout << "Mounting..." << endl;
    if (!Mount(dev, mountpoint, fs, opts))
        return false;

    cout << "Compressing..." << endl;
    if (failed = !Tar(true, tgz, mountpoint)) {
        cout << "Deleting failed archive..." << endl;
        Remove(tgz, false, true);
    }

    cout << "Unmounting..." << endl;
    UnmountA(mountpoint);

    return !failed;
}

// Backups a UBI device after attaching it and when done
// detaches it. Calls BackupMountpoint().
static bool BackupUBI(const char *tgz, int mtd, int ubi,
        const char *dev, const char *mountpoint, 
        const char *opts = NULL) {

    bool failed;

    if (opts == NULL)
        opts = "ro";

    cout << "Attaching NAND..." << endl;
    if (!UBIAttach(mtd, ubi))
        return false;

    // Backup mountpoint.
    failed = !BackupMountpoint(tgz, dev, mountpoint, "ubifs", opts);

    cout << "Detaching NAND..." << endl;
    UBIDetach(ubi);

    return !failed;
}

// Formats a UBI device ("ubifs" only) after attaching it
// and when done leaves it attached.
static bool FormatAndAttachUBI(const MTD &mtd, int ubi) {
    cout << "Erasing NAND..." << endl;
    if (!FlashEraseAll(mtd.device))
        goto fail_erase;

    cout << "Attaching NAND..." << endl;
    if (!UBIAttach(mtd.number, ubi))
        goto fail_attach;

    cout << "Creating 'rootfs' volume..." << endl;
    if (!UBIMakeVolume(ubi, UBIV_NUMBER, UBIV_NAME))
        goto fail_mkvol;

    return true;

fail_mkvol:
    cout << "Detaching NAND..." << endl;
    UBIDetach(ubi);

fail_attach:
fail_erase:
    return false;
}

// Mounts a device, extracts the archive and then unmounts it.
// NOTE the backup must not reside on the mountpoint.
// If "needFormat" = true, then Format() is called.
static bool RestoreMountpoint(const char *tgz, const char *dev, 
        const char *mountpoint, const char *fs, 
        const char *opts = NULL, bool needFormat = true) {

    bool failed;
    string temp;

    if (needFormat) {
        cout << "Formatting..." << endl;
        if (!Format(dev, fs))
            return false;
    }

    cout << "Mounting..." << endl;
    if (!Mount(dev, mountpoint, fs, opts))
        return false;

    cout << "Extracting..." << endl;
    failed = !Tar(false, tgz, mountpoint);

    cout << "Unmounting..." << endl;
    UnmountA(mountpoint);

    return !failed;
}

// Restores a UBI device after attaching it and when done
// detaches it. Calls RestoreMountpoint().
static bool RestoreUBI(const char *tgz, const MTD &mtd, int ubi,
        const char *dev, const char *mountpoint, 
        const char *opts = NULL) {

    bool failed;

    if (opts == NULL)
        opts = "rw";

    if (!FormatAndAttachUBI(mtd, ubi))
        return false;

    failed = !RestoreMountpoint(tgz, dev, mountpoint, "ubifs", opts, false);

    cout << "Detaching NAND..." << endl;
    UBIDetach(ubi);

    return !failed;
}

// Erase and flash the specified MTD device. The blocksize and count
// is only used while writing, the entire device is erased. Uses
// "flash_eraseall" and "dd".
inline bool FlashMTD(const char *dev, const char *file) {
    cout << "Erasing..." << endl;
    if (!FlashEraseAll(dev))
        return false;

    cout << "Flashing..." << endl;
    return NandWrite(dev, file, true);
}

// Backups an MTD partition. If it is not present, backups the corresponding
// internal SD card area (after accounting for MBR).
inline bool BackupMTDPartition(const MTD &mtd, const char *file) {
    bool success;

    if (access(mtd.sysfs, F_OK) == 0)
        // For NAND devices, read directly.
        success = NandDump(mtd.device, file);
    else
        // For no-NAND devices, read SD card.
        success = DiskDump(DEV_INTSD, file, 
            512,                    // size of 1 sector
            mtd.size / 512,         // number of sectors
            1 + mtd.start / 512,    // offset in device
            0);                     // offset in file

    if (!success)
        Remove(file, false, true);

    return success;
}

// Restores an MTD partition. If it is not present, restores the corresponding
// internal SD card area (after accounting for MBR).
inline bool RestoreMTDPartition(const MTD &mtd, const char *file) {
    if (access(mtd.sysfs, F_OK) == 0)
        // For NAND devices, write directly.
        return FlashMTD(mtd.device, file);
    else
        // For no-NAND devices, write to SD card.
        return DiskDump(file, DEV_INTSD, 
            512,                    // size of 1 sector
            mtd.size / 512,         // number of sectors
            0,                      // offset in file
            1 + mtd.start / 512);   // offset in device
}

// Mounts '/' and '/system' as needed on "/mnt/root".
// NOTE mount(s) will be read-write.
inline bool MountRootfs() {
    const char *root = MOUNT_ROOT;      // initial mount for rootfs
    const char *system = MOUNT_ROOT;    // initial mount for system
    const bool haveNAND = (access(gMTDs[MTD_ROOTFS].sysfs, F_OK) == 0);

    if (haveNAND) {
        const MTD &mtd = gMTDs[MTD_ROOTFS];

        cout << "Attaching NAND..." << endl;
        if (!UBIAttach(mtd.number, UBID_NUMBER))
            goto fail_attach_nand;

        cout << "Mounting NAND..." << endl;
        if (!Mount(DEV_NAND, root, "ubifs", "rw"))
            goto fail_mount_nand;

        cout << "Ensuring '/system' directory exists..." << endl;
        if (!MakeDirectory(MOUNT_ROOT_SYSTEM, true))
            goto fail_mkdir_nand;

        // Change system mountpoint to subdirectory.
        system = MOUNT_ROOT_SYSTEM;
    }

    cout << "Mounting 'system' partition..." << endl;
    if (Mount(DEV_SYSTEM, system, NULL, "rw"))
        return true;

fail_mkdir_nand:
    if (haveNAND) {
        cout << "Unmounting NAND..." << endl;
        UnmountA(root);
    }

fail_mount_nand:
    if (haveNAND) {
        cout << "Detaching NAND..." << endl;
        UBIDetach(UBID_NUMBER);
    }

fail_attach_nand:
    return false;
}

// Unounts '/' and '/system' as needed on "/mnt/root".
inline bool UnmountRootfs() {
    bool failed = false;
    const char *root = MOUNT_ROOT;      // initial mount for rootfs
    const char *system = MOUNT_ROOT;    // initial mount for system
    const bool haveNAND = (access(gMTDs[MTD_ROOTFS].sysfs, F_OK) == 0);

    // Change system mountpoint to subdirectory if needed.
    if (haveNAND)
        system = MOUNT_ROOT_SYSTEM;

    cout << "Unmounting 'system' partition..." << endl;
    failed |= !UnmountA(system);

    if (haveNAND) {
        cout << "Unmounting NAND..." << endl;
        failed |= !UnmountA(root);

        cout << "Detaching NAND..." << endl;
        failed |= !UBIDetach(UBID_NUMBER);
    }

    return !failed;
}


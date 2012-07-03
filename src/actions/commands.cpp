/*
 *  commands.cpp:
 *      - Shell commands (e.g. tar, mount, etc.) implementation.
 */

// Execute the specified command and notify user if failed.
// Notification is simply using "cout".
inline bool ExecuteAndNotifyIfFail(const char *cmd) {
    if (SysCall(cmd) != 0) {
        cout << "An error occured while trying to perform the requested operation" << endl;
        return false;
    }

    return true;
}

// Post a message asking for button press to continue.
inline void NotifyWaitForButton() {
    cout << "Press any key to return to the menu." << endl;
    GetButtonPress();
}

// Executes "cp" command.
static bool Copy(const char *files, const char *dest,
        bool recursive = false, bool permissions = false) {

    string
    cmd = "cp";

    if (recursive || permissions) {
        cmd += " -";

        if (permissions)
            cmd += "p";

        if (recursive)
            cmd += "r";
    }

    cmd += " ";
    cmd += files;
    cmd += " ";
    cmd += dest;

    return ExecuteAndNotifyIfFail(cmd.c_str());
}

// Executes "mv" command.
static bool Move(const char *files, const char *dest) {
    string 
    cmd = "mv ";
    cmd += files;
    cmd += " ";
    cmd += dest;
    return ExecuteAndNotifyIfFail(cmd.c_str());
}

// Executes "rm" command.
static bool Remove(const char *files, bool recursive = false, 
        bool force = false) {

    string 
    cmd = "rm";

    if (recursive || force) {
        cmd += " -";

        if (recursive)
            cmd += "r";

        if (force)
            cmd += "f";
    }

    cmd += " ";
    cmd += files;

    return ExecuteAndNotifyIfFail(cmd.c_str());
}

// Executes "mkdir" command.
static bool MakeDirectory(const char *dir, bool parent = false) {
    string 
    cmd = "mkdir";

    if (parent)
        cmd += " -p";

    cmd += " ";
    cmd += dir;

    return ExecuteAndNotifyIfFail(cmd.c_str());
}

// Executes "pushd" command.
static bool PushDirectory(const char *dir) {
    string 
    cmd = "pushd ";
    cmd += dir;
    return ExecuteAndNotifyIfFail(cmd.c_str());
}

// Executes "popd" command.
static bool PopDirectory() {
    string cmd = "popd";
    return ExecuteAndNotifyIfFail(cmd.c_str());
}

// Executes "tar" command for creating\extracting an archive.
static bool Tar(bool create, const char *tar, const char *chdir,
        const char *files = NULL, bool addPerms = true, 
        bool compress = true, bool verbose = true) {

    string 
    cmd = "tar -";

    if (create && files == NULL)
        files = ".";

    if (create)
        cmd += "c";
    else
        cmd += "x";

    if (create && addPerms)
        cmd += "p";

    if (create && compress)
        cmd += "z";

    if (verbose)
        cmd += "v";

    cmd += "f ";
    cmd += tar;

    if (chdir) {
        cmd += " -C ";
        cmd += chdir;
    }

    if (files) {
        cmd += " ";
        cmd += files;
    }

    return ExecuteAndNotifyIfFail(cmd.c_str());
}


// Executes the "unzip" command.
static bool Unzip(const char *zip, const char *chdir,
        bool quiet = false, bool overwrite = true) {

    string
    cmd = "unzip";

    if (quiet)
        cmd += " -q";

    if (overwrite)
        cmd += " -o";

    cmd += " ";
    cmd += zip;
    cmd += " -d ";
    cmd += chdir;

    return ExecuteAndNotifyIfFail(cmd.c_str());
}

// Executes "mount" command.
static bool Mount(const char *dev, const char *mountpoint,
        const char *fs = NULL, const char *opts = NULL) {

    string 
    cmd = "mount";

    if (fs) {
        cmd += " -t ";
        cmd += fs;
    }

    if (opts) {
        cmd += " -o ";
        cmd += opts;
    }

    cmd += " ";
    cmd += dev;
    cmd += " ";
    cmd += mountpoint;

    return ExecuteAndNotifyIfFail(cmd.c_str());
}

// Executes "umount" command.
static bool Unmount(const char *mountpoint, bool lazy = false) {
    string 
    cmd = "umount";

    if (lazy)
        cmd += " -l";

    cmd += " ";
    cmd += mountpoint;

    return ExecuteAndNotifyIfFail(cmd.c_str());
}

// Executes "mkdosfs" command.
static bool FormatFat(const char *dev) {
    string 
    cmd = "mkdosfs ";
    cmd += dev;
    return ExecuteAndNotifyIfFail(cmd.c_str());
}

// Executes "mkfs.ext2", "mkfs.ext3" or "mkfs.ext4" command.
static bool FormatExt(const char *dev, int fs = 2) {
    string 
    cmd = "mkfs.ext";

    if (fs == 4)
        cmd += "4";
    else if (fs == 3)
        cmd += "3";
    else
        cmd += "2";

    cmd += " ";
    cmd += dev;

    return ExecuteAndNotifyIfFail(cmd.c_str());
}

// Executes "dd" command.
static bool DiskDump(const char *src, const char *dest, 
        int bs = -1, int count = -1, int skip = 0, int seek = 0) {

    string 
    cmd = "dd if=";
    cmd += src;
    cmd += " of=";
    cmd += dest;

    if (bs != -1) {
        cmd += " bs=";
        cmd += NumberToString(bs);
    }

    if (count != -1) {
        cmd += " count=";
        cmd += NumberToString(count);
    }

    if (skip != 0) {
        cmd += " skip=";
        cmd += NumberToString(skip);
    }

    if (seek != 0) {
        cmd += " seek=";
        cmd += NumberToString(seek);
    }

    return ExecuteAndNotifyIfFail(cmd.c_str());
}

// Executes "flash_eraseall" command.
static bool FlashEraseAll(const char *mtd, bool quiet = true) {
    string 
    cmd = "flash_eraseall";

    if (quiet)
        cmd += " -q";

    cmd += " ";
    cmd += mtd;
    return ExecuteAndNotifyIfFail(cmd.c_str());
}

// Executes "nanddump" command.
static bool NandDump(const char *mtd, const char *file, 
        bool noOOB = true, bool noBadBlocks = true) {

    string 
    cmd = "nanddump -f ";
    cmd += file;

    if (noOOB)
        cmd += " -o";

    if (noBadBlocks)
        cmd += " -b";

    cmd += " ";
    cmd += mtd;

    return ExecuteAndNotifyIfFail(cmd.c_str());
}

// Executes "nandwrite" command.
static bool NandWrite(const char *mtd, const char *file, 
        bool pad = true, bool quiet = true) {

    string 
    cmd = "nandwrite ";
    cmd += mtd;
    cmd += " ";
    cmd += file;

    if (pad)
        cmd += " -p";

    if (quiet)
        cmd += " -q";

    return ExecuteAndNotifyIfFail(cmd.c_str());
}

// Executes "ubiattach" command.
static bool UBIAttach(int MTD, int device) {
    string 
    cmd = "ubiattach /dev/ubi_ctrl -m ";
    cmd += NumberToString(MTD);
    cmd += " -d ";
    cmd += NumberToString(device);
    return ExecuteAndNotifyIfFail(cmd.c_str());
}

// Executes "ubimkvol" command.
static bool UBIMakeVolume(int device, int volID, const char *volName,
        int sizeInBytes = -1) {

    string 
    cmd = "ubimkvol ";
    cmd += DEV_UBI;
    cmd += NumberToString(device);
    cmd += " -n ";
    cmd += NumberToString(volID);
    cmd += " -N ";
    cmd += volName;

    if (sizeInBytes == -1) {
        cmd += " -m";
    } else {
        cmd += " -s ";
        cmd += sizeInBytes;
    }

    return ExecuteAndNotifyIfFail(cmd.c_str());
}

// Executes "ubirmvol" command.
static bool UBIRemoveVolume(int device, int volID) {
    string
    cmd = "ubirmvol ";
    cmd += DEV_UBI;
    cmd += NumberToString(device);
    cmd = " -n ";
    cmd += NumberToString(volID);
    return ExecuteAndNotifyIfFail(cmd.c_str());
}

// Executes "ubirmvol" command.
static bool UBIRemoveVolume(int device, const char *volName) {
    string
    cmd = "ubirmvol ";
    cmd += DEV_UBI;
    cmd += NumberToString(device);
    cmd = " -N ";
    cmd += volName;
    return ExecuteAndNotifyIfFail(cmd.c_str());
}

// Executes "ubidetach" command.
static bool UBIDetach(int device) {
    string 
    cmd = "ubidetach /dev/ubi_ctrl -d ";
    cmd += NumberToString(device);
    return ExecuteAndNotifyIfFail(cmd.c_str());
}


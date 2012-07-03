/*
 *  backups.cpp:
 *      - Actions for "Backup\Restore" menu.
 */

// Creates a temporary folder in the given directory.
// Returns the complete path.
inline bool MakeTempFolder(string &out, const char *path) {
    char temp[PATH_MAX], *result;

    strcpy(temp, path);
    strcat(temp, "/tempdir.XXXXXXXX");

    if (!(result = mkdtemp(temp)))
        return false;

    out = temp;
    return true;
}

// Creates a backup path string of the format
// "$path/Backup_2010-12-30_16:00:00.mfw".
inline void MakeBackupPath(string &out, const char *path) {
    time_t t = time(0);
    tm *now = localtime(&t);

    char temp[64];
    sprintf(temp, "Backup_%d-%d-%d_%d-%d-%d.mfw", 
        now->tm_year + 1900, now->tm_mon + 1, now->tm_mday,
        now->tm_hour, now->tm_min, now->tm_sec);

    JoinPath(out, path, temp);
}

// Check if there may be insufficient space for backup creation.
inline bool VerifyBackupCreationSpace(bool backupSystem, bool backupData,
        const char *tempPath) {

    size_t kernel = 0, nand = 0, system = 0, data = 0;

    if (backupSystem) {
        const MTD &mtdKernel = gMTDs[MTD_KERNEL];
        const MTD &mtdNAND = gMTDs[MTD_ROOTFS];

        if (mtdKernel.name)
            if ((kernel = mtdKernel.size) == 0)
                return false;

        if (access(mtdNAND.sysfs, F_OK) == 0)
            if ((nand = 512 * GetBlockDeviceSize(mtdNAND.sysfs)) == 0)
                return false;

        if ((system = 512 * GetBlockDeviceSize(SYS_SYSTEM)) == 0)
            return false;
    }

    if (backupData)
        if ((data = 512 * GetBlockDeviceSize(SYS_DATA)) == 0)
            return false;

    // Assuming that all partitions are not full and
    // compression is fairly effective, this estimate
    // should provide reasonable accuracy.
    return (kernel + nand + system + data) < 
        GetMountpointFreeSpace(tempPath);
}

// Creates a backup in the directory specified by user using the
// format provided by MakeBackupPath(). While making the backup
// the temporary directory provided by MakeTempFolder() is used.
// Calls BackupUBI(), BackupMountpoint() and BackupMTDPartition().
static bool CreateBackup(bool backupSystem, bool backupData) {
    FileWindow fw;
    string temp, files, tempPath, backupPath;
    bool failed = false;

    gTerminal.clear();

    // Make sure we can access "/system" partition if needed.
    if (backupSystem && access(SYS_SYSTEM, F_OK) != 0) {
        cout << "Unable to access 'system' partition on internal SD." << endl;
        NotifyWaitForButton();
        return false;
    }

    // Make sure we can access "/data" partition if needed.
    if (backupData && access(SYS_DATA, F_OK) != 0) {
        cout << "Unable to access 'data' partition on internal SD." << endl;
        NotifyWaitForButton();
        return false;
    }

    // Get a temporary folder.
    if (!MakeTempFolder(tempPath, GetDefaultPath())) {
        cout << "Unable to create a temporary folder for backup creation." << endl;
        NotifyWaitForButton();
        return false;
    }

    cout << "Press the HOME key to select a folder to place the " << endl
        << "backup in, or any other key to cancel." << endl;

    if (!VerifyBackupCreationSpace(backupSystem, backupData, tempPath.c_str()))
        cout << "WARNING: There may be insufficient space for creating " << endl
            << "a backup." << endl;

    if (GetButtonPress() != KEY_HOME)
        goto cleanup;

    // Get the output directory.
    fw.SetPath(GetDefaultPath());
    fw.SetDirectoryMode(true);
    fw.Show();

    // Get the backup archive path.
    MakeBackupPath(backupPath, fw.GetSelectedPath());

    // Try to backup kernel if needed.
    if (backupSystem && gMTDs[MTD_KERNEL].name) {
        const MTD &mtd = gMTDs[MTD_KERNEL];
        cout << "* Backing up kernel..." << endl;
        JoinPath(temp, tempPath.c_str(), mtd.filename);

        if (BackupMTDPartition(mtd, temp.c_str())) {
            files += mtd.filename;
            files += " ";
        } else {
            goto fail;
        }
    }

    // Try to backup NAND if needed.
    if (backupSystem && access(gMTDs[MTD_ROOTFS].sysfs, F_OK) == 0) {
        const char *tgz = "nand.tgz";
        cout << "* Backing up NAND..." << endl;
        JoinPath(temp, tempPath.c_str(), tgz);

        if (BackupUBI(temp.c_str(), gMTDs[MTD_ROOTFS].number, UBID_NUMBER, 
                DEV_NAND, MOUNT_NAND)) {
            files += tgz;
            files += " ";
        } else {
            goto fail;
        }
    }

    // Try to backup system if needed.
    if (backupSystem) {
        const char *tgz = "system.tgz";
        cout << "* Backing up system..." << endl;
        JoinPath(temp, tempPath.c_str(), tgz);

        if (BackupMountpoint(temp.c_str(), DEV_SYSTEM, MOUNT_SYSTEM)) {
            files += tgz;
            files += " ";
        } else {
            goto fail;
        }
    }

    // Try to backup data if needed.
    if (backupData) {
        const char *tgz = "data.tgz";
        cout << "* Backing up data..." << endl;
        JoinPath(temp, tempPath.c_str(), tgz);

        if (BackupMountpoint(temp.c_str(), DEV_DATA, MOUNT_DATA)) {
            files += tgz;
            files += " ";
        } else {
            goto fail;
        }
    }

    cout << "* Preparing final backup..." << endl;
    if (Tar(true, backupPath.c_str(), tempPath.c_str(), files.c_str(), false, false))
        goto success;
    else
        goto fail;

fail:
    failed = true;

success:
cleanup:
    // Remove the (possibly) partial backup archive.
    if (failed && backupPath.length() != 0) {
        cout << "* Cleaning up failed backup archive..." << endl;
        failed |= !Remove(backupPath.c_str(), false, true);
    }

    // Remove the entire temporary folder containing the
    // individual backups.
    cout << "* Cleaning up temporary folder used for backup creation..." << endl;
    failed |= !Remove(tempPath.c_str(), true, true);

    if (!failed)
        cout << "Success!" << endl;

    NotifyWaitForButton();
    return false;
}

// Check if there may be insufficient space for backup restoration.
inline bool VerifyBackupRestorationSpace(const char *archive, const char *tempPath) {
    return GetFileSize(archive) < 
        GetMountpointFreeSpace(tempPath);
}

// Restores the backup specified by user. While restoring the backup
// the temporary directory provided by MakeTempFolder() is used.
// Calls RestoreUBI(), RestorepMountpoint() and RestoreMTDPartition().
static bool RestoreBackupFromFile() {
    FileWindow fw;
    string tempPath;
    DIR *dir = NULL;
    dirent *ent = NULL;
    vector<string> filters;
    bool verifyPhase = true, extractPhase = false,
        modified = false, warning = false, failed = false;

    gTerminal.clear();

    // Get a temporary folder.
    if (!MakeTempFolder(tempPath, GetDefaultPath())) {
        cout << "Unable to create a temporary folder for backup restoration." << endl;
        NotifyWaitForButton();
        return false;
    }

    cout << "Press the HOME key to select a folder to place the " << endl
        << "backup in, or any other key to cancel." << endl;

    if (GetButtonPress() != KEY_HOME)
        goto cleanup;

    filters.push_back("*.mfw");

    // Get the output directory.
    fw.SetPath(GetDefaultPath());
    fw.SetFilters(filters);
    fw.Show();

    if (!VerifyBackupRestorationSpace(fw.GetSelectedPath(), tempPath.c_str())) {
        cout << "WARNING: There may be insufficient space for restoring " << endl
            << "the backup. Press HOME key to continue, any other key" << endl
            << "to exit. Your device has not yet been modified." << endl;

        if (GetButtonPress() != KEY_HOME) {
            // To avoid the message shown on a failure reset both flags.
            verifyPhase = extractPhase = false;
            goto fail;
        }
    }

    cout << "* Extracing archived backup..." << endl;
    if (!Tar(false, fw.GetSelectedPath(), tempPath.c_str()))
        goto fail;

    for (;;) {
        if (!(dir = opendir(tempPath.c_str()))) {
            cout << "Unable to access temporary directory." << endl;
            goto fail;
        }

        if (verifyPhase)
            cout << "+ Verifying..." << endl;
        else
            cout << "+ Extracting..." << endl;

        while ((ent = readdir(dir)) != NULL) {
            string backupName;

            if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
                continue;

            JoinPath(backupName, tempPath.c_str(), ent->d_name);

            if (verifyPhase) {
                // In verify phase, we attempt to check if we can access the required media.
                // If not, we cancel the process before doing anything.
                if (gMTDs[MTD_KERNEL].name && !strcmp(ent->d_name, gMTDs[MTD_KERNEL].filename)) {
                    // Make sure we can access kernel.
                    if (access(gMTDs[MTD_KERNEL].sysfs, F_OK) != 0 && access(SYS_INTSD, F_OK) != 0) {
                        cout << "Unable to access kernel on NAND/MMC." << endl;
                        goto fail;
                    }

                    // Check space requirements.
                    if (gMTDs[MTD_KERNEL].size < GetFileSize(backupName.c_str())) {
                        cout << "WARNING: There may be insufficient space on 'kernel' partition." << endl;
                        warning = true;
                    }
                } else if (!strcmp(ent->d_name, "nand.tgz")) {
                    // Make sure we can access NAND.
                    if (access(gMTDs[MTD_ROOTFS].sysfs, F_OK) != 0) {
                        cout << "Unable to access NAND." << endl;
                        goto fail;
                    }

                    // Check space requirements.
                    if (512 * GetBlockDeviceSize(gMTDs[MTD_ROOTFS].sysfs) < GetFileSize(backupName.c_str())) {
                        cout << "WARNING: There may be insufficient space on NAND." << endl;
                        warning = true;
                    }
                } else if (!strcmp(ent->d_name, "system.tgz")) {
                    // Make sure we can access "/system" partition.
                    if (access(SYS_SYSTEM, F_OK) != 0) {
                        cout << "Unable to access 'system' partition on internal SD." << endl;
                        goto fail;
                    }

                    // Check space requirements.
                    if (512 * GetBlockDeviceSize(SYS_SYSTEM) < GetFileSize(backupName.c_str())) {
                        cout << "WARNING: There may be insufficient space on 'system' partition." << endl;
                        warning = true;
                    }
                } else if (!strcmp(ent->d_name, "data.tgz")) {
                    // Make sure we can access "/data" partition.
                    if (access(SYS_DATA, F_OK) != 0) {
                        cout << "Unable to access 'data' partition on internal SD." << endl;
                        goto fail;
                    }

                    // Check space requirements.
                    if (512 * GetBlockDeviceSize(SYS_DATA) < GetFileSize(backupName.c_str())) {
                        cout << "WARNING: There may be insufficient space on 'data' partition." << endl;
                        warning = true;
                    }
                } else {
                    cout << "An unknown file in the backup archive was ignored: " << ent->d_name << endl;
                    log << WARN << "Unknown file in backup: " << ent->d_name << endl;
                }
            } else {
                string temp;
                JoinPath(temp, tempPath.c_str(), ent->d_name);

                // In extract phase, we actually make changes to the user's system.
                if (gMTDs[MTD_KERNEL].name && !strcmp(ent->d_name, gMTDs[MTD_KERNEL].filename)) {
                    modified = true;
                    cout << "* Flashing kernel..." << endl;
                    if (!RestoreMTDPartition(gMTDs[MTD_KERNEL], temp.c_str()))
                        goto fail;
                } else if (!strcmp(ent->d_name, "nand.tgz")) {
                    modified = true;
                    cout << "* Restoring NAND..." << endl;
                    if (!RestoreUBI(temp.c_str(), gMTDs[MTD_ROOTFS], UBID_NUMBER, 
                            DEV_NAND, MOUNT_NAND))
                        goto fail;
                } else if (!strcmp(ent->d_name, "system.tgz")) {
                    modified = true;
                    cout << "* Restoring 'system' partition..." << endl;
                    if (!RestoreMountpoint(temp.c_str(), DEV_SYSTEM, MOUNT_SYSTEM, FS_SYSTEM))
                        goto fail;
                } else if (!strcmp(ent->d_name, "data.tgz")) {
                    modified = true;
                    cout << "* Restoring 'data' partition..." << endl;
                    if (!RestoreMountpoint(temp.c_str(), DEV_DATA, MOUNT_DATA, FS_DATA))
                        goto fail;
                }
            }
        }

        // Close the directory.
        closedir(dir);
        dir = NULL;

        if (verifyPhase && warning) {
            cout << "Press HOME key to continue, any other key to cancel restoration." << endl
                << "Your device has not yet been modified." << endl;

            if (GetButtonPress() != KEY_HOME) {
                // To avoid the message shown on a failure reset both flags.
                verifyPhase = extractPhase = false;
                goto fail;
            }

            warning = false;
        }

        if (verifyPhase && !extractPhase) {
            extractPhase = true;
            verifyPhase = false;
        } else if (extractPhase && !verifyPhase) {
            extractPhase = false;
            verifyPhase = false;
            goto success;
        } else {
            log << ERRR << "Logic error during restoration of backup." << endl;
            break;
        }
    }

fail:
    failed = true;

    // Close the directory if left open.
    if (dir) {
        closedir(dir);
        dir = NULL;
    }

    if (verifyPhase && !extractPhase)
        cout << "Because the failure occurred in the verification phase," << endl
            << "no changes were made to your device and hence it should" << endl
            << "be immediately usable." << endl;
    else if (extractPhase && !verifyPhase)
        cout << "Because the failure occurred in the extraction phase," << endl
            << "your device may be unusable till you restore another backup" << endl
            << "or flash another firmware." << endl;

success:
cleanup:
    // Remove the entire temporary folder containing the
    // individual backups.
    cout << "* Cleaning up temporary folder used for backup restoration..." << endl;
    failed |= !Remove(tempPath.c_str(), true, true);

    if (!failed)
        cout << "Success!" << endl;

    NotifyWaitForButton();
    return false;
}

static bool CreateSystemBackup() {
    return CreateBackup(true, false);
}

static bool CreateDataBackup() {
    return CreateBackup(false, true);
}

static bool CreateDataSystemBackup() {
    return CreateBackup(true, true);
}

static bool RestoreBackup() {
    return RestoreBackupFromFile();
}


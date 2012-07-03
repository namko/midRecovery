/*
 *  backups.cpp:
 *      - Actions for "Storage and Partitions" menu.
 */

// Generic function for backing up a MTD device.
// Always return false.
static bool BackupMTD(const MTD &mtd) {
    gTerminal.clear();

    if (!mtd.name) {
        cout << "Your device does not have this partition." << endl;
        NotifyWaitForButton();
        return false;
    }

    cout << "Press the HOME key to select a folder to backup '" << mtd.name << "'" << endl
        << "partition or any other key to cancel. The partition will be saved as" << endl
        << "'" << mtd.filename << "'" << endl;

    if (GetButtonPress() != KEY_HOME)
        return false;

    // Get the output directory.
    FileWindow fw;
    fw.SetPath(GetDefaultPath());
    fw.SetDirectoryMode(true);
    fw.Show();

    // Build the output file path.
    string file;
    JoinPath(file, fw.GetSelectedPath(), mtd.filename);

    // Execute.
    cout << "Backing up '" << mtd.name << "' partition..." << endl;
    if (BackupMTDPartition(mtd, file.c_str()))
        cout << "Success!" << endl;

    NotifyWaitForButton();
    return false;
}

// Generic function for flashing a MTD device.
// Always return false.
static bool FlashMTD(const MTD &mtd) {
    gTerminal.clear();

    if (!mtd.name) {
        cout << "Your device does not have this partition." << endl;
        NotifyWaitForButton();
        return false;
    }

    cout << "Press the HOME key to select a file to flash '" << mtd.name << "'" << endl 
        << "partition or any other key to cancel." << endl;

    if (GetButtonPress() != KEY_HOME)
        return false;

    vector<string> filters;
    filters.push_back(mtd.filepattern);

    // Get the output directory.
    FileWindow fw;
    fw.SetPath(GetDefaultPath());
    fw.SetFilters(filters);
    fw.Show();

    // Execute.
    cout << "Preparing to flash '" << mtd.name << "' partition..." << endl;
    if (RestoreMTDPartition(mtd, fw.GetSelectedPath()))
        cout << "Success!" << endl;

    NotifyWaitForButton();
    return false;
}

static bool BackupLogo() {
    return BackupMTD(gMTDs[MTD_LOGO]);
}

static bool BackupKernel() {
    return BackupMTD(gMTDs[MTD_KERNEL]);
}

static bool BackupRecovery() {
    return BackupMTD(gMTDs[MTD_RECOVERY]);
}

static bool BackupBootArgs() {
    return BackupMTD(gMTDs[MTD_BOOTARGS]);
}

static bool BackupParameters() {
    return BackupMTD(gMTDs[MTD_PARAM]);
}

static bool BackupBootloader() {
    return BackupMTD(gMTDs[MTD_BOOTLOADER]);
}

static bool FlashLogo() {
    return FlashMTD(gMTDs[MTD_LOGO]);
}

static bool FlashKernel() {
    return FlashMTD(gMTDs[MTD_KERNEL]);
}

static bool FlashRecovery() {
    return FlashMTD(gMTDs[MTD_RECOVERY]);
}

static bool FlashBootArgs() {
    gTerminal.clear();
    cout << "WARNING!!! Incorrectly modifiying bootloader arguments" << endl 
        << "may render your tablet unbootable! To continue, press" << endl
        << "HOME key twice or any other combination to cancel." << endl;

    if (GetButtonPress() != KEY_HOME)
        return false;

    cout << "Press HOME key again. This *will* flash the bootloader arguments." << endl;

    if (GetButtonPress() != KEY_HOME)
        return false;

    return FlashMTD(gMTDs[MTD_BOOTARGS]);
}

static bool FlashParameters() {
    return FlashMTD(gMTDs[MTD_PARAM]);
}


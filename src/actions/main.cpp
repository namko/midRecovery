/*
 *  main.cpp:
 *      - Actions for "Backup\Restore" menu.
 *      - Implementation of menu-to-menu navigation.
 */

bool FlashROM() {
    const char *rootfsNames[] = {
        "utv210_root.tgz", "utv210_root.tar", 
        "rootfs.tgz", "root.tgz", "rootfs.tar", "root.tar", 
        "utv210_rootfs.tgz", "utv210_rootfs.tar", 
        "system.tgz", "system.tar", "update.zip", NULL
    };

    string kernel, rootfs;
    bool isZip = false, warning = false, success = true;

    gTerminal.clear();
    cout << "Press the HOME key to select a folder where the " << endl
        << "ROM is stored in, or any other key to cancel." << endl;

    if (GetButtonPress() != KEY_HOME)
        return false;

    // Get the input directory.
    FileWindow fw;
    fw.SetPath(GetDefaultPath());
    fw.SetDirectoryMode(true);
    fw.Show();

    // Locate kernel if applicable.
    if (gMTDs[MTD_KERNEL].name) {
        JoinPath(kernel, fw.GetSelectedPath(), gMTDs[MTD_KERNEL].filename);

        if (!IsFileValid(kernel.c_str())) {
            cout << "WARNING: Kernel not found in ROM." << endl;
            warning = true;
        }
    }

    // Search for rootfs archive.
    for (const char **iterator = rootfsNames; *iterator; iterator++) {
        JoinPath(rootfs, fw.GetSelectedPath(), *iterator);

        if (rootfs.length() > 3)
            if (rootfs.substr(rootfs.length() - 4, 4).compare(".zip") == 0)
                isZip = true;

        if (IsFileValid(rootfs.c_str()))
            break;

        rootfs.resize(0);
        isZip = false;
    }

    if (rootfs.length() == 0) {
        cout << "ERROR: Main update archive not found in ROM." << endl
            << "Your device has not yet been modified." << endl;
        goto fail;
    }

    if (warning) {
        cout << "Press HOME key to continue, any other key to cancel flash." << endl
            << "Your device has not yet been modified." << endl;

        if (GetButtonPress() != KEY_HOME)
            return false;

        warning = false;
    }

    // Flash kernel if applicable and present.
    if (kernel.length() != 0) {
        cout << "* Preparing to flash kernel..." << endl;
        if (!RestoreMTDPartition(gMTDs[MTD_KERNEL], kernel.c_str()))
            goto fail;
    }

    // Initialize NAND if applicable and present.
    if (gMTDs[MTD_ROOTFS].name && access(gMTDs[MTD_ROOTFS].sysfs, F_OK) == 0) {
        cout << "* Preparing NAND..." << endl;
        if (!FormatAndAttachUBI(gMTDs[MTD_ROOTFS], UBID_NUMBER))
            goto fail;

        cout << "Detaching NAND..." << endl;
        UBIDetach(UBID_NUMBER);
    }

    cout << "* Formatting 'system' partition..." << endl;
    if (!Format(DEV_SYSTEM, FS_SYSTEM))
        goto fail;

    cout << "* Mounting partition(s)..." << endl;
    if (!MountRootfs())
        goto fail;

    cout << "* Installing system..." << endl;
    if (isZip) {
        success = Unzip(rootfs.c_str(), MOUNT_ROOT);

        cout << "Fixing permissions..." << endl;
        success &= ExecuteShellScript("/scripts/fixperms.sh");        
    } else {
        success = Tar(false, rootfs.c_str(), MOUNT_ROOT);
    }

    cout << "* Unmounting partition(s)..." << endl;
    UnmountRootfs();

    if (success)
        cout << "Success!" << endl;

fail:
    NotifyWaitForButton();
    return false;
}

bool ApplyPatch() {
    vector<string> filters;
    bool isZip = false, warning = false, success = true;

    gTerminal.clear();
    cout << "Press the HOME key to select the patch to apply, or " << endl
        << "any other key to cancel." << endl;

    if (GetButtonPress() != KEY_HOME)
        return false;

    filters.push_back("*.tgz");
    filters.push_back("*.tar");
    filters.push_back("*.zip");

    // Get the input directory.
    FileWindow fw;
    fw.SetPath(GetDefaultPath());
    fw.SetFilters(filters);
    fw.Show();

    cout << "* Mounting partition(s)..." << endl;
    if (!MountRootfs())
        goto fail;

    cout << "* Extracting patch..." << endl;
    if (isZip) {
        success = Unzip(fw.GetSelectedPath(), MOUNT_ROOT);

        cout << "Fixing permissions..." << endl;
        success &= ExecuteShellScript("/scripts/fixperms.sh");
    } else {
        success = Tar(false, fw.GetSelectedPath(), MOUNT_ROOT);
    }

    cout << "* Applying patch..." << endl;
    success &= ExecuteShellScript("/scripts/applypatch.sh", 1, MOUNT_ROOT);    

    cout << "* Unmounting partition(s)..." << endl;
    UnmountRootfs();

    if (success)
        cout << "Success!" << endl;

fail:
    NotifyWaitForButton();
    return false;
}

bool WipeData() {
    gTerminal.clear();
    cout << "Press the HOME key to wipe data\\factory reset, or" << endl
        << "any other key to cancel." << endl;

    if (GetButtonPress() != KEY_HOME)
        return false;

    cout << "Formatting /data..." << endl;
    if (Format(DEV_DATA, FS_DATA))
        cout << "Success!" << endl;

    NotifyWaitForButton();
    return false;
}

bool WipeCache() {
    gTerminal.clear();
    cout << "Press the HOME key to wipe cache, or any other key" << endl
        << "to cancel." << endl;

    if (GetButtonPress() != KEY_HOME)
        return false;

    cout << "Formatting /cache..." << endl;
    if (Format(DEV_CACHE, FS_CACHE))
        cout << "Success!" << endl;

    NotifyWaitForButton();
    return false;
}

bool DisplayBackupsWindow() {
    WinBackups.Reset();
    WinBackups.Show();
    return false;
}

bool DisplayPartitionsWindow() {
    WinPartitions.Reset();
    WinPartitions.Show();
    return false;
}

bool DisplayAdvancedWindow() {
    WinAdvanced.Reset();
    WinAdvanced.Show();
    return false;
}

bool Shutdown() {
    SysCall("reboot -f");
    return true;
}

bool DisplayMainWindow() {
    return true;
}


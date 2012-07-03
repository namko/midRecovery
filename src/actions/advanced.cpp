/*
 *  backups.cpp:
 *      - Actions for "Advanced" menu.
 */

// Shows a window asking for size for the named partition.
// Returns one of the selected values.
int GetPartitionSize(const char *name, vector<int> sizes) {
    for (;;) {
        // Build title.
        string 
        title = "Select size of '";
        title += name;
        title += "' partition in MB";

        // Build options.
        vector<WindowOption> opts;
        for (int i = 0; i < sizes.size(); i++)
            opts.push_back(WindowOption(NumberToString(sizes[i]), NULL));

        // Show window.
        Window win;
        win.SetTitle(title.c_str());
        win.SetOptions(opts);
        int ret = win.Show();

        if (ret < 0 || ret >= sizes.size())
            continue;

        return sizes[ret];
    }
}

// Partitions and formats the internal SD card according to the
// given layout.
bool PartitionAndFormatSDCard(int cache, int data, int system) {
    bool failed = false;
    char strCache[32], strData[32], strSystem[32];

    gTerminal.clear();

    sprintf(strCache, "%d", cache);
    sprintf(strData, "%d", data);
    sprintf(strSystem, "%d", system);

    cout << "Making partitions..." << endl;
    if (!ExecuteShellScript("/scripts/partition.sh", 4, 
            DEV_INTSD, strCache, strData, strSystem))
        return false;

    cout << "Formatting user area..." << endl;
    failed |= !FormatFat(DEV_INTSDP);

    cout << "Formatting 'cache' partition..." << endl;
    failed |= !Format(DEV_CACHE, FS_CACHE);

    cout << "Formatting 'data' partition..." << endl;
    failed |= !Format(DEV_DATA, FS_DATA);

    cout << "Formatting 'system' partition..." << endl;
    failed |= !Format(DEV_SYSTEM, FS_SYSTEM);

    return failed;
}

bool ShowLog() {
    Log::Flush();
    FileView fv;
    fv.SetFile(Log::GetPath());
    fv.Show();
    return false;
}

bool WipeDalvikCache() {
    string 
    dalvikPath = MOUNT_DATA_DALVIK;
    dalvikPath += "/*";
    bool success;

    gTerminal.clear();
    cout << "Press the HOME key to wipe Dalvik cache, or" << endl
        << "any other key to cancel." << endl;

    if (GetButtonPress() != KEY_HOME)
        return false;

    cout << "Mounting /data..." << endl;
    if (!Mount(DEV_DATA, MOUNT_DATA))
        goto fail_mount;

    cout << "Cleaning..." << endl;
    success = Remove(dalvikPath.c_str(), true, true);

    cout << "Unmounting..." << endl;
    UnmountA(MOUNT_DATA);

    if (success)
        cout << "Success!" << endl;

fail_mount:
    NotifyWaitForButton();
    return false;
}

bool PartitionSDCard() {
    vector<int> sizes;
    int sizeCache, sizeData, 
        sizeSystem, sizeDevice;

    gTerminal.clear();

    if (Log::IsInternalSD()) {
        cout << "Because the recovery log is being stored in the internal" << endl
            << "SD card, it is not possible to re-partition the SD card." << endl
            << "Insert an external SD card, reboot, and try again." << endl;
        NotifyWaitForButton();
        return false;
    }

    if ((sizeDevice = GetBlockDeviceSize(SYS_INTSD) / 2048) == 0) {
        cout << "Unable to determine size of internal SD card." << endl
            << "The partitioning utility cannot continue." << endl;
        NotifyWaitForButton();
        return false;
    }

    cout << "Press the HOME key to repartition the internal SD card," << endl
        << "or any other key to cancel. If your firmware is installed on" << endl
        << "the 'system' partition (of the internal SD card) it *WILL* be" << endl
        << "erased, and you will have to flash again or restore a backup." << endl;

    if (GetButtonPress() != KEY_HOME)
        return false;

    for (;;) {
        for (int i = 64; i <= 256; i += 32)
            sizes.push_back(i);

        sizeCache = GetPartitionSize("cache", sizes);
        sizes.clear();

        for (int i = 256; i <= 1024; i += 128)
            sizes.push_back(i);

        sizeData = GetPartitionSize("data", sizes);
        sizes.clear();

        for (int i = 128; i <= 512; i += 64)
            sizes.push_back(i);

        sizeSystem = GetPartitionSize("system", sizes);
        sizes.clear();

        int sizeIntSD = sizeDevice - (32 + sizeCache + sizeData + sizeSystem + 4);

        if (sizeIntSD <= 0) {
            cout << "There is no room left for user area on internal SD card." << endl
                << "Please enter the sizes for cache, data and system again" << endl
                << "(cache=" << sizeCache << ", data=" << sizeData << ", system=" 
                << sizeSystem << " and detected card size = " << sizeDevice << ")" << endl;
            continue;
        }

        cout << "You selected cache = " << sizeCache << " MB, data = " << sizeData 
            << " MB and system = " << sizeSystem << " MB." << endl
            << "This leaves " << sizeIntSD << " MB for the user area of internal SD." << endl
            << "Press HOME key to continue with this layout, or any other key" << endl
            << "to change the layout." << endl;

        if (GetButtonPress() == KEY_HOME)
            break;
    }

    if (PartitionAndFormatSDCard(sizeCache, sizeData, sizeSystem))
        cout << "Success!" << endl;

    NotifyWaitForButton();
    return false;
}

bool FixROMPermissions() {
    bool success;

    gTerminal.clear();
    cout << "Press the HOME key to fix ROM permissions, or" << endl
        << "any other key to cancel." << endl;

    if (GetButtonPress() != KEY_HOME)
        return false;

    if (!MountRootfs())
        goto fail;

    cout << "Fixing ROM permissions..." << endl;
    success = ExecuteShellScript("/scripts/fixperms.sh");

    UnmountRootfs();

    if (success)
        cout << "Success!" << endl;

fail:
    NotifyWaitForButton();
    return false;
}

bool ShowFileInspector() {
    FileWindow fw;
    fw.SetPath(GetDefaultPath());
    fw.Show();

    FileView fv;
    fv.SetFile(fw.GetSelectedPath());
    fv.Show();

    return false;
}


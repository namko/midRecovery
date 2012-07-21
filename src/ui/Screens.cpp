/*
 *  Screens.cpp:
 *      - Implementation of various MID recovery windows.
 */
#include <iostream>
#include <vector>
#include <string>

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/statvfs.h>
#include <linux/input.h>

#include "../include/log.h"
#include "../include/config.h"
#include "../include/syscall.h"
#include "../include/util.h"
#include "../include/Window.h"
#include "../include/FileWindow.h"
#include "../include/FileView.h"
#include "../ui/Terminal.h"
#include "../hw/mtd.h"
#include "Screens.h"

using namespace std;

static Window WinBackups;
static Window WinPartitions;
static Window WinAdvanced;
static Window WinMain;

#include "../actions/commands.cpp"
#include "../actions/common.cpp"
#include "../actions/backups.cpp"
#include "../actions/partitions.cpp"
#include "../actions/advanced.cpp"
#include "../actions/main.cpp"

Window *WinSetup() {
    WindowOptions main, backups, partitions, adv;

    backups.push_back(WindowOption("Create system backup",      CreateSystemBackup));
    backups.push_back(WindowOption("Create data backup",        CreateDataBackup));
    backups.push_back(WindowOption("Create data+system backup", CreateDataSystemBackup));
    backups.push_back(WindowOption("Restore backup",            RestoreBackup));
    backups.push_back(WindowOption("(Back)",                    DisplayMainWindow));

    partitions.push_back(WindowOption("Backup logo",            BackupLogo));
    partitions.push_back(WindowOption("Backup kernel",          BackupKernel));
    partitions.push_back(WindowOption("Backup recovery",        BackupRecovery));
    partitions.push_back(WindowOption("Backup bootargs",        BackupBootArgs));
    partitions.push_back(WindowOption("Backup parameters",      BackupParameters));
    partitions.push_back(WindowOption("Backup bootloader",      BackupBootloader));
    partitions.push_back(WindowOption("Flash logo",             FlashLogo));
    partitions.push_back(WindowOption("Flash kernel",           FlashKernel));
    partitions.push_back(WindowOption("Flash recovery",         FlashRecovery));
    partitions.push_back(WindowOption("Flash parameters",       FlashParameters));
    partitions.push_back(WindowOption("(Back)",                 DisplayMainWindow));

    adv.push_back(WindowOption("Show log",                      ShowLog));
    adv.push_back(WindowOption("Wipe dalvik-cache",             WipeDalvikCache));
    adv.push_back(WindowOption("Partition SD card",             PartitionSDCard));
    adv.push_back(WindowOption("Fix ROM Permissions",           FixROMPermissions));
    adv.push_back(WindowOption("File Inspector",                ShowFileInspector));
    adv.push_back(WindowOption("Dump Kernel Messages",          DumpKernelMessages));
    adv.push_back(WindowOption("(Back)",                        DisplayMainWindow));

    main.push_back(WindowOption("Flash ROM",                    FlashROM));
    main.push_back(WindowOption("Apply patch\\update",          ApplyPatch));
    main.push_back(WindowOption("Wipe data\\factory reset",     WipeData));
    main.push_back(WindowOption("Wipe cache",                   WipeCache));
    main.push_back(WindowOption("Backup\\Restore",              DisplayBackupsWindow));
    main.push_back(WindowOption("Partitions and Storage",       DisplayPartitionsWindow));
    main.push_back(WindowOption("Advanced",                     DisplayAdvancedWindow));
    main.push_back(WindowOption("Shutdown",                     Shutdown));

    WinBackups.SetTitle("Backup\\Restore");
    WinPartitions.SetTitle("Partitions and Storage");
    WinAdvanced.SetTitle("Advanced");
    WinMain.SetTitle("MID Recovery " RECOVERY_VERSION);

    WinBackups.SetOptions(backups);
    WinPartitions.SetOptions(partitions);
    WinAdvanced.SetOptions(adv);
    WinMain.SetOptions(main);

    return &WinMain;
}


/*
 *  main.cpp:
 *      - Entry point for MID recovery.
 */
#include <iostream>
#include "include/log.h"
#include "include/config.h"
#include "include/Window.h"
#include "ui/Screens.h"

using namespace std;

int main(const int argc, const char *argv[]) {
    if (!Log::Init(argc, argv)) {
        cout << "ERROR initializing log." << endl;
        return 1;
    }

    ConfigInit(argc, argv);

    Window *w = WinSetup();

    w->Show();

    ConfigDeInit();

    Log::Close();

    return 0;
}


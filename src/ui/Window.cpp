/*
 *  Window.cpp:
 *      - Implementation of class to choose one menu option (from many).
 */
#include <iostream>
#include <iomanip>
#include <vector>

#include "Terminal.h"
#include "../include/config.h"
#include "../include/log.h"
#include "../include/util.h"
#include "../include/Window.h"

using namespace std;

// Utility function(s).
static void TrimString(string &str, const int maxLen);

// ============================================================================
// Class constructor.
Window::Window() {
    disp_start = 
    disp_end = 
    selected = 0;
}

// Sets the Window title.
void Window::SetTitle(const char *str) {
    title.assign(str);
    TrimString(title, MAX_INNER_WIDTH - 5);
}

// Sets the Window options.
void Window::SetOptions(const WindowOptions &opts) {
    options.clear();

    for (int i = 0; i < opts.size(); i++) {
        options.push_back(opts[i]);
        TrimString(options[i].first, MAX_INNER_WIDTH - 3);
    }

    Reset();
}

// Draws the Window.
void Window::Draw() const {
    gTerminal.clear();
    gTerminal.text_reset();

    gTerminal.text_bold();
    gTerminal.text_underline();
    cout << '\t' << title << endl << endl;

    gTerminal.text_reset();

    for (int i = disp_start; i < disp_end; i++) {
        cout << "* ";

        if (selected == i) {
            gTerminal.text_inverse();
            cout << left << setw(MAX_INNER_WIDTH - 2);
        }

        cout << options[i].first << endl;

        if (selected == i)
            gTerminal.text_reset();
    }

    cout << endl;
}

// Displays the window. Returns the selected option (0-based) as well
// as fires the assigned function if present.
int Window::Show() {
    for (;;) {
        // First draw the window.
        Draw();

        // Wait for user input.
        switch (WINDOW_INPUT_FN()) {
        case WI_DOWN:
            selected = (selected + 1) % options.size();
            break;
        case WI_UP:
            selected = (selected + options.size() - 1) % options.size();
            break;
        case WI_SELECT:
            if (!options[selected].second || 
                    options[selected].second()) {
                // If there is no event associated with the menu or 
                // if the event asks us to exit the menu, do so.
                gTerminal.clear();
                return selected;
            } else {
                // Executed the event but we must continue back
                // to the menu.
                break;
            }
        default:
            // Do nothing.
            log << WARN << "Input function returned junk data!" << endl;
            break;
        }

        // Now that the pointer has been updated, make sure
        // to update the limits as well for the scroller.
        if (selected < disp_start) {
            disp_start = selected;
            disp_end = disp_start + MAX_INNER_HEIGHT;

            if (disp_end > options.size())
                disp_end = options.size();

        } else if (selected >= disp_end) {
            disp_end = selected + 1;
            disp_start = disp_end - MAX_INNER_HEIGHT;

            if (disp_start < 0)
                disp_start = 0;
        } 
    }
}

// Reset window state (selection, display, etc.)
void Window::Reset() {
    selected = 
    disp_start = 0;
    disp_end = options.size();

    // Limit to internal height.
    if (disp_end > MAX_INNER_HEIGHT)
        disp_end = MAX_INNER_HEIGHT;
}

// ============================================================================
// Trim the given string to appropriate length and single line.
// Then replace tabs with spaces to prevent overflow.
static void TrimString(string &str, const int maxLen) {
    int cr = str.find('\r');
    int lf = str.find('\n');
    int len = str.size();

    // Determine '\r' and '\n' positions.
    if (cr != -1 && lf != -1)
        len = (cr < lf) ? cr : lf;
    else if (cr != -1)
        len = cr;
    else if (lf != -1)
        len = lf;

    // Trim to maximum length.
    if (len > maxLen)
        len = maxLen;

    str.resize(len);

    // Replace tabs.
    for (int tab = str.find('\t'); tab != -1; 
            tab = str.find('\t'))
        str[tab] = ' ';
}


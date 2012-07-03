/*
 *  FileView.cpp:
 *      - Implementation of class to view text files.
 */
#include <iostream>
#include <fstream>

#include "Terminal.h"
#include "../include/log.h"
#include "../include/config.h"
#include "../include/util.h"
#include "../include/FileView.h"

using namespace std;

// ============================================================================
// Class constructor.
FileView::FileView() {
    offset = 0;
}

// Sets the current file.
bool FileView::SetFile(const char *str) {
    if (!IsFileValid(str))
        return false;

    filename = str;
    
    if (filename.length() > MAX_INNER_WIDTH - 4)
        filename.resize(MAX_INNER_WIDTH - 4);

    return true;
}

void FileView::Draw() {
    int linesLeft = offset;
    string temp;
    ifstream in(filename.c_str());

    gTerminal.clear();
    gTerminal.text_reset();
    gTerminal.text_bold();
    gTerminal.text_underline();

    if (in.fail()) {
        cout << '\t' << filename << endl << endl;
        cout << "(Error while trying to open file)" << endl;
        gTerminal.text_reset();
        goto cleanup;
    }

    if (in.eof()) {
        cout << '\t' << filename << endl << endl;
        cout << "(File is empty)" << endl;
        gTerminal.text_reset();
        goto cleanup;
    }

    // Skip 'offset' lines.
    while (!in.eof() && linesLeft != 0) {
        std::getline(in, temp);
        linesLeft--;
    }

    // Print the file name and offset.
    temp = filename + " @ ";
    temp += NumberToString(offset);

    if (temp.length() > MAX_INNER_WIDTH - 4)
        temp.resize(MAX_INNER_WIDTH - 4);

    cout << '\t' << temp << endl << endl;
    gTerminal.text_reset();

    // If there still are lines remaining, correct offset.
    while (in.eof() && linesLeft != 0) {
        linesLeft -= MAX_INNER_HEIGHT;
        offset -= MAX_INNER_HEIGHT;

        if (linesLeft < 0)
            linesLeft = 0;

        if (offset < 0)
            offset = 0;
    }

    // Try to read maximum number of lines.
    linesLeft = MAX_INNER_HEIGHT;

    // Now display the lines.
    while (in.good() && linesLeft != 0) {
        std::getline(in, temp);

        // Trim to maximum width.
        if (temp.length() > MAX_INNER_WIDTH)
            temp.resize(MAX_INNER_WIDTH);

        if (temp.find("<ERRR>") == 0 || temp.find("<WARN>") == 0 || 
                temp.find("<CMMD>") == 0 || temp.find("<INFO>") == 0) {

            // Print the tag with highlight.
            string tag = temp.substr(0, 6);
            temp.erase(0, 6);

            gTerminal.text_bold();
            gTerminal.text_underline();
            cout << tag;

            gTerminal.text_reset();
        }

        cout << temp << endl;
        linesLeft--;
    }

cleanup:
    if (in.is_open())
        in.close();
}

// Displays the window. 
int FileView::Show() {
    for (;;) {
        // First draw the window.
        Draw();

        // Wait for user input.
        switch (WINDOW_INPUT_FN()) {
        case WI_DOWN:
            offset += MAX_INNER_HEIGHT - 1;
            break;
        case WI_UP:
            offset -= MAX_INNER_HEIGHT - 1;
            break;
        case WI_SELECT:
            gTerminal.clear();
            return 0;
        default:
            // Do nothing.
            log << WARN << "Input function returned junk data!" << endl;
            break;
        }

        if (offset < 0)
            offset = 0;
    }
}

// Reset window state (offset, etc.)
void FileView::Reset() {
    offset = 0;
}

void FileView::SetTitle(const char *str) {
    return;
}

void FileView::SetOptions(const WindowOptions &opts) {
    return;
}


/*
 *  FileWindow.cpp:
 *      - Implementation of class to choose file\folders.
 */
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <dirent.h>

#include "../include/config.h"
#include "../include/util.h"
#include "../include/FileWindow.h"

using namespace std;

// Utility function(s).
static void GetParentDirectory(char *out, const char *in);
static bool IsPatternValid(const string &str);
static bool CheckPatternMatch(const char *str, const char *pattern);
static bool DirectoryEntrySort(const WindowOption i, const WindowOption j);

// ============================================================================
// Class constructor.
FileWindow::FileWindow() : path("/"), selpath("") {
    dirMode = false;
}

// Gets the current directory.
const char *FileWindow::GetSelectedPath() const {
    return selpath.c_str();
}

// Sets the current directory.
bool FileWindow::SetPath(const char *str) {
    int i;
    char temp[PATH_MAX];

    if (!IsDirectoryValid(str))
        return false;

    // NOTE realpath is used to deal with "." and "..'.
    realpath(str, temp);

    path = temp;
    Window::SetTitle(temp);
    return true;
}

// Sets the filter for files displayed.
bool FileWindow::SetFilters(const std::vector<std::string> &f) {
    filters.clear();

    for (int i = 0; i < f.size(); i++)
        if (IsPatternValid(f[i]))
            filters.push_back(f[i]);

    return filters.size() == f.size();
}

// Sets directory mode (i.e. return directory paths instead of file paths).
void FileWindow::SetDirectoryMode(bool mode) {
    dirMode = mode;
}

// Displays the window. Returns the selected option (0-based) as well
// as fires the assigned function if present.
int FileWindow::Show() {
    for (;;) {
        WindowOptions opts;
        DIR *dir = opendir(path.c_str());

        // Add all contents of the directory (matching our criteria).
        if (dir) {
            dirent *ent = NULL;

            while ((ent = readdir(dir)) != NULL) {
                bool isDir = IsSubdirectoryValid(path.c_str(), ent->d_name);

                // Skip "." and "..". We add this ourselves later.
                if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
                    continue;

                // If we are in directory mode and encounter a file, skip.
                if (dirMode && !isDir)
                    continue;

                // If this is a file, check with filters.
                if (!isDir && filters.size() != 0) {
                    bool add = false;

                    for (int i = 0; i < filters.size(); i++)
                        if (CheckPatternMatch(ent->d_name, filters[i].c_str())) {
                            add = true;
                            break;
                        }

                    if (!add)
                        continue;
                }

                // Add to list.
                opts.push_back(WindowOption(ent->d_name, NULL));

                // If this is a directory, add a "/" to distinguish it from files.
                if (isDir)
                    opts[opts.size() - 1].first += "/";            
            }

            closedir(dir);
        }

        // Add the entry to reach parent directory and then sort.
        opts.push_back(WindowOption("..", NULL));

        // Sort in ascending separating directories & files.
        sort(opts.begin(), opts.end(), DirectoryEntrySort);

        // For directory mode, add the entry to select the directory at the end.
        if (dirMode)
            opts.push_back(WindowOption("(Select this directory)", NULL));

        // Setup the window.
        Window::SetOptions(opts);
        Window::SetTitle(path.c_str());

        // Get user's choice.
        int retVal = Window::Show();
        const string &ret = opts[retVal].first;

        if (!ret.compare("..")) {
            // If ".." was selected, move one path up.
            char temp[PATH_MAX];
            GetParentDirectory(temp, path.c_str());
            SetPath(temp);
        } else if (ret[ret.length() - 1] == '/') {
            // If a directory was selected, change path.
            string temp;
            JoinPath(temp, path.c_str(), ret.c_str());
            SetPath(temp.c_str());
        } else {
            // Otherwise, save the path and return the index.
            if (dirMode)
                selpath = path;
            else
                JoinPath(selpath, path.c_str(), ret.c_str());
            return retVal;
        }
    }
}

// Reset window state (selection, display, etc.)
void FileWindow::Reset() {
    Window::Reset();
}

void FileWindow::SetTitle(const char *str) {
    return;
}

void FileWindow::SetOptions(const WindowOptions &opts) {
    return;
}

// ============================================================================
// Returns the parent directory for the given path.
static void GetParentDirectory(char *out, const char *in) {
    int i;
    string temp = in;

    // Special case: no slashes.
    if (temp.find('/') == -1) {
        strcpy(out, "..");
        return;
    }

    // Erase all trailing slashes.
    while (temp.size() > 1 && temp[temp.size() - 1] == '/')
        temp.resize(temp.size() - 1);

    // Now chop off everything after the last slash.
    if ((i = temp.rfind('/')) > 0)
        temp.resize(i);
    else if (i == 0)
        temp.resize(1);

    // Again erase all trailing slashes.
    while (temp.size() > 1 && temp[temp.size() - 1] == '/')
        temp.resize(temp.size() - 1);

    strcpy(out, temp.c_str());
}

// Returns whether the input pattern is valid or not.
static bool IsPatternValid(const string &str) {
    if (str.length() == 0)
        return false;

    // Select filters with a maximum of one star.
    if (str.find('*') != str.rfind('*'))
        return false;

    // Accept star only at the begining or at the end.
    if (str.find('*') != -1 && 
            str[0] != '*' && 
            str[str.length() - 1] != '*')
        return false;

    return true;
}

// Simple pattern matching function (works with atmost one star
// either at the begining or at the end).
static bool CheckPatternMatch(const char *str, const char *pattern) {
    if (!*str || !*pattern)
        return true;

    if (pattern[0] == '*') {
        const char *l = str, *r = pattern;
        
        while (*(l + 1))
            l++;

        while (*(r + 1))
            r++;

        while (l >= str && r != pattern && *l == *r)
            r--, l--;
        
        return r == pattern;
    } else if (pattern[strlen(pattern) - 1] == '*') {
        const char *l = str, *r = pattern;

        while (*l && *r != '*' && *l == *r)
            r++, l++;

        return *r == '*';
    } else {
        return strstr(str, pattern) != NULL;
    }
}

// Sorts two WindowOption entries each representing a directory
// entry. Uses lexicographical_compare() when applicable.
static bool DirectoryEntrySort(const WindowOption i, const WindowOption j) {
    bool dirI = (!i.first.compare(".") || 
                    !i.first.compare("..") || 
                    (i.first[i.first.length() - 1] == '/'));

    bool dirJ = (!j.first.compare(".") || 
                    !j.first.compare("..") || 
                    (j.first[j.first.length() - 1] == '/'));

    if ((dirI && dirJ) || (!dirI && !dirJ)) {
        string l = i.first, r = j.first;

        for (int n = 0; n < l.size(); n++)
            l[n] = tolower(l[n]);

        for (int n = 0; n < r.size(); n++)
            r[n] = tolower(r[n]);

        if (l.compare(r))
            return lexicographical_compare(
                l.begin(), l.end(), 
                r.begin(), r.end());
        else
            return lexicographical_compare(
                i.first.begin(), i.first.end(), 
                j.first.begin(), j.first.end());
    } else {
        return dirI;
    }
}


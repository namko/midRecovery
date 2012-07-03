/*
 *  util.h:
 *      - Utility functions.
 */
#ifndef __UTIL_H_
#define __UTIL_H_

#include <stdio.h>
#include <sys/stat.h>

// Join two (clean) paths.
// Pre-condition: out.c_str() != str1 && out.c_str() != str2
inline void JoinPath(std::string &out, const char *str1, const char *str2) {
    out = str1;
    if (out[out.length() - 1] != '/')
        out += '/';
    out += str2;
}

// Returns if the path is a regular file.
inline bool IsFileValid(const char *str) {
    struct stat st;
    return (!stat(str, &st) && S_ISREG(st.st_mode));
}

// Returns if the path is a directory.
inline bool IsDirectoryValid(const char *str) {
    struct stat st;
    return (!stat(str, &st) && S_ISDIR(st.st_mode));
}

// Returns if the directory formed by the concatentation 
// of two strings (with a slash) is valid.
inline bool IsSubdirectoryValid(const char *str1, const char *str2) {
    std::string temp;
    JoinPath(temp, str1, str2);
    return IsDirectoryValid(temp.c_str());
}

// Returns the file size if the path is a regular file.
inline size_t GetFileSize(const char *str) {
    struct stat st;
    if (!stat(str, &st) && S_ISREG(st.st_mode))
        return st.st_size;
    else
        return 0;
}

// Converts a value to string. The returned string
// is a static character array.
inline const char *NumberToString(int value) {
    static char temp[32];
    sprintf(temp, "%d", value);
    return temp;
}

#endif  //  __UTIL_H_


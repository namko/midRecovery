/*
 *  FileWindow.h:
 *      - Class to choose file\folders.
 */
#ifndef __FILE_WINDOW_H_
#define __FILE_WINDOW_H_

#include <string>
#include <vector>
#include "Window.h"
 
class FileWindow : public Window {
private:
    bool dirMode;
    std::string path;
    std::string selpath;
    std::vector<std::string> filters;

protected:
    // Redundant functions.
    virtual void SetTitle(const char *str);
    virtual void SetOptions(const WindowOptions &opts);

public:
    FileWindow();

    const char *GetSelectedPath() const;
    bool SetPath(const char *str);
    bool SetFilters(const std::vector<std::string> &f);
    void SetDirectoryMode(bool mode);
    virtual int Show();
    virtual void Reset();
};

#endif  //  __FILE_WINDOW_H_


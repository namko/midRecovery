/*
 *  FileWindow.h:
 *      - Class to view text files.
 */
#ifndef __FILE_VIEW_H_
#define __FILE_VIEW_H_

#include <string>
#include "Window.h"
 
class FileView : public Window {
private:
    int offset;
    std::string filename;

    void Draw();

protected:
    // Redundant functions.
    virtual void SetTitle(const char *str);
    virtual void SetOptions(const WindowOptions &opts);

public:
    FileView();
    bool SetFile(const char *str);
    virtual int Show();
    virtual void Reset();
};

#endif  //  __FILE_VIEW_H_


/*
 *  Window.h:
 *      - Class to choose one menu option (from many).
 */
#ifndef __WINDOW_H_
#define __WINDOW_H_

#include <string>
#include <vector>

enum WindowInput {
    WI_DOWN,
    WI_UP,
    WI_SELECT,
};

typedef WindowInput (*WindowInputFn)();
typedef bool (*WindowCallbackFn)();
typedef std::pair<std::string, WindowCallbackFn> WindowOption;
typedef std::vector<WindowOption> WindowOptions;

class Window {
private:
    int selected;
    int disp_start;
    int disp_end;

    std::string title;
    WindowOptions options;

    void Draw() const;

public:
    Window();

    virtual void SetTitle(const char *str);
    virtual void SetOptions(const WindowOptions &opts);
    virtual int Show();
    virtual void Reset();
};

#endif  //  __WINDOW_H_


/*
 *  s3c-button.h:
 *      - Class for input from s3c-button.
 */
#ifndef __S3C_BUTTON_H_
#define __S3C_BUTTON_H_

#include "../include/Window.h"

class S3CButton {
private:
    static int fd;
    static bool init;

public:
    static bool Initialize();
    static WindowInput GetKeyPress();

};

#endif  //  __S3C_BUTTON_H_


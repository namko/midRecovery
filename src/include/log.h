/*
 *  log.h:
 *      - MID recovery logger.
 */
#ifndef __LOG_H_
#define __LOG_H_

#include <fstream>
#include <string>

extern std::ofstream log;

inline std::ostream &ERRR(std::ostream &out) {
    return out << "<ERRR>";
}

inline std::ostream &WARN(std::ostream &out) {
    return out << "<WARN>";
}

inline std::ostream &CMMD(std::ostream &out) {
    return out << "<CMMD>";
}

inline std::ostream &INFO(std::ostream &out) {
    return out << "<INFO>";
}

class Log {
private:
    static std::string logPath;
    static bool isInternal;

public:
    static bool Init(const int argc, const char *argv[]);
    static void Flush();
    static void Close();

    static bool IsInternalSD();
    static const char *GetPath();
};

#endif  //  __LOG_H_


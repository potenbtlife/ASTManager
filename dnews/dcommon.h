#ifndef DNEWS_DCOMMON_H
#define DNEWS_DCOMMON_H
#include <string>
#include <stdarg.h>

using namespace std;

/*ȥ��string���ߵĿո񡢻��з�*/
string& stringTrim(string& src);

inline std::string& rtrim(std::string& str, const char* whitespaces = " \t\f\v\n\r") {
    std::string::size_type pos = str.find_last_not_of(whitespaces);

    if (pos != std::string::npos) {
        str.erase(pos + 1);

    } else {
        str.clear();
    }

    return str;
}

inline std::string& ltrim(std::string& str, const char* whitespaces = " \t\f\v\n\r") {
    return str.erase(0, str.find_first_not_of(whitespaces));
}

inline
std::string& trim(std::string& str, const char* whitespaces = " \t\f\v\n\r") {
    return ltrim(rtrim(str, whitespaces), whitespaces);
}

class Logger{
public:
    Logger();
    ~Logger();

    //���ļ���return: 0:�ɹ����أ�>0������
    int open(string logfile);

    void wirte(const char *format,...);
    void close();

private:
    string m_logFileName;
    FILE* fileStream;
};

#endif


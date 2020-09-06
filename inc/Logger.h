#ifndef _LOGGER_H
#define _LOGGER_H

#include <condition_variable>
#include <fstream>
#include <map>
#include <mutex>
#include <sstream>
#include <string>

using namespace std;

namespace log {

enum class LogLevel { FATAL = 0, ERROR, WARN, INFO, DEBUG };

class nocopyable {
   protected:
    nocopyable() = default;
    ~nocopyable() = default;

   private:
    nocopyable(const nocopyable&) = delete;
    nocopyable& operator=(const nocopyable&) = delete;
};

class Logger : nocopyable {
   public:
    Logger(string FILE, string FUNCTION, int LINE);
    Logger(string FILE, string FUNCTION, int LINE, LogLevel loglevel);
    ~Logger();

    Logger& operator<<(string s);

    static void Init();
    static void Init(bool t, bool f);
    static void Stop();
    static bool HasLog();
    static void SetWriteChannel(bool t, bool f);
    static void SetColor(bool color);

   private:
    LogLevel loglevel_;

    string log_line_;

    string FILE_;
    string FUNCTION_;
    int LINE_;

    static bool write_terminal_;
    static bool write_file_;

    static string file_name_;
    static fstream file_;

    static stringstream busy_buf_;
    static stringstream free_buf_;

    static condition_variable cond_;
    static mutex mtx_;
    static mutex mtx_f_;
    static bool looping_;
    static int interval_;

    static bool color_;

    static void ThreadFunc();

    static void SetFileName();
    static void SetFileName(const string file_name);

    void Format(const string& s);
};

#define LOG() Logger(__FILE__, __FUNCTION__, __LINE__)
#define LOGL(log_level) Logger(__FILE__, __FUNCTION__, __LINE__, log_level)
#define FATAL() Logger(__FILE__, __FUNCTION__, __LINE__, LogLevel::FATAL)
#define ERROR() Logger(__FILE__, __FUNCTION__, __LINE__, LogLevel::ERROR)
#define WARN() Logger(__FILE__, __FUNCTION__, __LINE__, LogLevel::WARN)
#define INFO() Logger(__FILE__, __FUNCTION__, __LINE__, LogLevel::INFO)
#define DEBUG() Logger(__FILE__, __FUNCTION__, __LINE__, LogLevel::DEBUG)

}  // namespace log

#endif
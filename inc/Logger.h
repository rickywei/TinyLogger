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

map<LogLevel, string> loglevel_str = {
    {FATAL, "FATAL"}, {ERROR, "ERROR"}, {WARN, "WARN"},
    {INFO, "INFO"},   {DEBUG, "DEBUG"},
};

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
    Logger();
    Logger(LogLevel loglevel);
    ~Logger();

    Logger& operator<<(string s);

    static void Init(bool t = false, bool f = true);
    static void Stop();
    static bool HasLog();
    static void SetWriteChannel(bool t = false, bool f = true);

   private:
    LogLevel loglevel_;

    string log_line_;

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

    static void ThreadFunc();

    static void SetFileName(const string file_name = "log.log");

    void Format(const string& s);
};

}  // namespace log

#endif
#include "Logger.h"

#include <chrono>
#include <ctime>
#include <iostream>
#include <thread>

using namespace log;

#define MAX_LOG_LINE_LEN 512
#define MAX_LOG_BUF_SIZE 1024000
#define WAIT_INTERVAL 2

map<LogLevel, string> loglevel_str = {
    {LogLevel::FATAL, "FATAL"}, {LogLevel::ERROR, "ERROR"},
    {LogLevel::WARN, "WARN "},  {LogLevel::INFO, "INFO "},
    {LogLevel::DEBUG, "DEBUG"},
};

Logger::Logger(string FILE, string FUNCTION, int LINE)
    : loglevel_(LogLevel::INFO),
      log_line_(),
      FILE_(FILE),
      FUNCTION_(FUNCTION),
      LINE_(LINE) {
    log_line_.reserve(MAX_LOG_LINE_LEN);
}

Logger::Logger(string FILE, string FUNCTION, int LINE, LogLevel loglevel)
    : loglevel_(loglevel),
      log_line_(),
      FILE_(FILE),
      FUNCTION_(FUNCTION),
      LINE_(LINE) {
    log_line_.reserve(MAX_LOG_LINE_LEN);
}

Logger::~Logger() {
    lock_guard<mutex> lk(mtx_);
    if (Logger::write_terminal_) {
        cerr << log_line_ << flush;
    }
    Logger::busy_buf_ << log_line_;
}

Logger& Logger::operator<<(string s) {
    Format(s);
    if (Logger::busy_buf_.str().size() >= MAX_LOG_BUF_SIZE) {
        // local scope
        {
            lock_guard<mutex> lk(Logger::mtx_f_);
            swap(busy_buf_, free_buf_);
        }
        Logger::cond_.notify_one();
    }
    return *this;
}

void Logger::Init() {
    Logger::SetFileName();
    Logger::SetWriteChannel(false, true);

    Logger::busy_buf_.str().reserve(MAX_LOG_BUF_SIZE);
    Logger::free_buf_.str().reserve(MAX_LOG_BUF_SIZE);

    thread th(Logger::ThreadFunc);
    if (th.joinable()) {
        th.join();
    }
}

void Logger::Init(bool t, bool f) {
    Logger::SetFileName();
    Logger::SetWriteChannel(t, f);

    Logger::busy_buf_.str().reserve(MAX_LOG_BUF_SIZE);
    Logger::free_buf_.str().reserve(MAX_LOG_BUF_SIZE);

    thread th(Logger::ThreadFunc);
    if (th.joinable()) {
        th.detach();
    }
}

void Logger::Stop() { Logger::looping_ = false; }

bool Logger::HasLog() { return free_buf_.str().size() > 0; }

void Logger::SetWriteChannel(bool t, bool f) {
    Logger::write_terminal_ = t;
    Logger::write_file_ = f;
}

bool Logger::write_terminal_ = false;
bool Logger::write_file_ = true;

string Logger::file_name_ = "log.log";
fstream Logger::file_ = fstream();

stringstream Logger::busy_buf_ = stringstream();
stringstream Logger::free_buf_ = stringstream();

condition_variable Logger::cond_;
mutex Logger::mtx_;
mutex Logger::mtx_f_;
bool Logger::looping_ = false;
int Logger::interval_ = WAIT_INTERVAL;

void Logger::ThreadFunc() {
    while (looping_) {
        unique_lock<mutex> ulk(mtx_f_);
        cond_.wait_for(ulk, Logger::interval_ * 1s,
                       [] { return Logger::HasLog(); });
        if (Logger::write_file_ && file_.is_open()) {
            Logger::file_ << Logger::free_buf_.str() << flush;
        }
        free_buf_.str().clear();
    }
    if (Logger::file_.is_open()) {
        if (Logger::write_file_ && file_.is_open()) {
            if (free_buf_.str().size()) {
                Logger::file_ << Logger::free_buf_.str() << flush;
            }
            if (busy_buf_.str().size()) {
                Logger::file_ << Logger::busy_buf_.str() << flush;
            }
        }
        file_.close();
    }
}

void Logger::SetFileName() {
    string file_name = "log.log";
    if (file_name != Logger::file_name_ || !Logger::file_.is_open()) {
        if (Logger::file_.is_open()) {
            file_.close();
        }
        file_ = fstream(file_name, ios::app);
    }
}

void Logger::SetFileName(const string file_name) {
    if (file_name != Logger::file_name_ || !Logger::file_.is_open()) {
        if (Logger::file_.is_open()) {
            file_.close();
        }
        file_ = fstream(file_name, ios::app);
    }
}

void Logger::Format(const string& s) {
    time_t rawtime;
    time(&rawtime);
    tm timeinfo;
    localtime_r(&rawtime, &timeinfo);
    char cs[64] = {'\0'};
    strftime(cs, 64, "%Y%m%d %X ", &timeinfo);
    log_line_ = string(cs) + loglevel_str[loglevel_] + " " + FILE_ + " " +
                FUNCTION_ + " " + to_string(LINE_) + " " + s + "\n";
}

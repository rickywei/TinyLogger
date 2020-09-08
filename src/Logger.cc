#include "Logger.h"

#include <string.h>

#include <chrono>
#include <ctime>
#include <iostream>
#include <thread>

using namespace log;

#define MAX_LOG_LINE_LEN 512
#define MAX_LOG_BUF_SIZE 102400
#define WAIT_INTERVAL 2

const map<LogLevel, string> loglevel_str = {
    {LogLevel::FATAL, "FATAL"}, {LogLevel::ERROR, "ERROR"},
    {LogLevel::WARN, "WARN "},  {LogLevel::INFO, "INFO "},
    {LogLevel::DEBUG, "DEBUG"},
};

Logger::Logger(string FILE, string FUNCTION, int LINE, LogLevel loglevel)
    : loglevel_(loglevel),
      log_line_(),
      FILE_(FILE),
      FUNCTION_(FUNCTION),
      LINE_(LINE) {
    log_line_.reserve(MAX_LOG_LINE_LEN);
    CheckDate();
    Format();
}

Logger::~Logger() {
    CheckDate();
    if (Logger::busy_buf_.size() >= MAX_LOG_BUF_SIZE) {
        // local scope
        {
            lock_guard<mutex> lk(Logger::mtx_);
            swap(Logger::busy_buf_, Logger::free_buf_);
        }
        Logger::cond_.notify_one();
    }
    log_line_ += "\n";
    busy_buf_ += log_line_;

    if (Logger::write_terminal_) {
        AddColor();
        cerr << log_line_ << flush;
    }
}

Logger& Logger::operator<<(string s) {
    log_line_ += " " + s;
    return *this;
}

void Logger::Init(bool isterminal) {
    Logger::write_terminal_ = isterminal;

    Logger::busy_buf_.reserve(MAX_LOG_BUF_SIZE);
    Logger::free_buf_.reserve(MAX_LOG_BUF_SIZE);

    thread th(Logger::ThreadFunc);
    if (th.joinable()) {
        th.detach();
    }
}

void Logger::Stop() { Logger::looping_ = false; }

bool Logger::HasLog() { return Logger::free_buf_.size(); }

void Logger::SetColor(bool color) { Logger::color_ = color; }

bool Logger::write_terminal_ = false;

string Logger::file_name_ = string();
fstream Logger::file_ = fstream();
string Logger::date_ = string();

string Logger::busy_buf_ = string();
string Logger::free_buf_ = string();

condition_variable Logger::cond_;
mutex Logger::mtx_;
mutex Logger::mtx_f_;
bool Logger::looping_ = true;
int Logger::interval_ = WAIT_INTERVAL;

bool Logger::color_ = false;

void Logger::ThreadFunc() {
    while (looping_) {
        if (!file_.is_open()) {
            file_ = fstream(Logger::file_name_, ios::app);
        }
        unique_lock<mutex> ulk(mtx_);
        cond_.wait_for(ulk, Logger::interval_ * 1s,
                       [] { return Logger::HasLog(); });

        if (Logger::busy_buf_.size()) {
            swap(Logger::busy_buf_, Logger::free_buf_);
            // local scope
            {
                lock_guard<mutex> lk(Logger::mtx_f_);
                Logger::file_ << Logger::free_buf_ << flush;
            }
            Logger::free_buf_.clear();
            // cout << HasLog() << "busy " << Logger::free_buf_.str() << endl;
        }
    }

    if (Logger::free_buf_.size() || busy_buf_.size()) {
        if (!Logger::file_.is_open()) {
            file_ = fstream(Logger::file_name_, ios::app);
        }
    }
    if (Logger::free_buf_.size()) {
        {
            lock_guard<mutex> lk(Logger::mtx_f_);
            Logger::file_ << Logger::free_buf_ << flush;
        }
    }
    if (busy_buf_.size()) {
        {
            lock_guard<mutex> lk(Logger::mtx_f_);
            Logger::file_ << Logger::free_buf_ << flush;
        }
    }

    if (file_.is_open()) {
        file_.close();
    }
}

void Logger::CheckDate() {
    if (Logger::date_ < Logger::GetNowDate()) {
        lock_guard<mutex> lk(Logger::mtx_f_);
        if (Logger::date_ < Logger::GetNowDate()) {
            Logger::date_ = Logger::GetNowDate();
            Logger::file_name_ = Logger::date_ + ".log";
            fstream new_file = fstream(Logger::file_name_, ios::app);
            Logger::file_.swap(new_file);
            if (new_file.is_open()) {
                new_file.close();
            }
        }
    }
}

string Logger::GetNowDate() {
    string s;
    // year
    for (int i = 7; i <= 10; ++i) {
        s += __DATE__[i];
    }

    // month
    string m(__DATE__, 3);
    if (m == "Jan") {
        s += "01";
    } else if (m == "Feb") {
        s += "02";
    } else if (m == "Mar") {
        s += "03";
    } else if (m == "Apr") {
        s += "04";
    } else if (m == "May") {
        s += "05";
    } else if (m == "Jun") {
        s += "06";
    } else if (m == "Jul") {
        s += "07";
    } else if (m == "Aug") {
        s += "08";
    } else if (m == "Sep") {
        s += "09";
    } else if (m == "Oct") {
        s += "10";
    } else if (m == "Nov") {
        s += "11";
    } else if (m == "Dec") {
        s += "12";
    }

    for (int i = 4; i <= 5; ++i) {
        s += __DATE__[i] == ' ' ? '0' : __DATE__[i];
    }

    return move(s);
}

void Logger::Format() {
    log_line_ += Logger::date_ + " " + __TIME__ + " " +
                 loglevel_str.at(loglevel_) + " " + FILE_ + " " + FUNCTION_ +
                 " " + to_string(LINE_);
}

void Logger::AddColor() {
    if (Logger::color_) {
        switch (Logger::loglevel_) {
            case LogLevel::FATAL:
                log_line_ = "\033[31m" + log_line_ + "\033[0m";
                break;
            case LogLevel::ERROR:
                log_line_ = "\033[31m" + log_line_ + "\033[0m";
                break;
            case LogLevel::WARN:
                log_line_ = "\033[33m" + log_line_ + "\033[0m";
                break;
            case LogLevel::INFO:
                break;
            case LogLevel::DEBUG:
                log_line_ = "\033[32m" + log_line_ + "\033[0m";
                break;
            default:
                break;
        }
    }
}

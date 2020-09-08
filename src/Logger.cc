#include "Logger.h"

#include <chrono>
#include <ctime>
#include <iostream>
#include <thread>

using namespace log;

#define MAX_LOG_LINE_LEN 512
#define MAX_LOG_BUF_SIZE 102400
#define WAIT_INTERVAL 2

map<LogLevel, string> loglevel_str = {
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
    Format();
}

Logger::~Logger() {
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
        AddColorBegin();
        AddColorEnd();
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

string Logger::file_name_ = "log.log";
fstream Logger::file_ = fstream();

string Logger::busy_buf_ = string();
string Logger::free_buf_ = string();

condition_variable Logger::cond_;
mutex Logger::mtx_;
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
            Logger::file_ << Logger::free_buf_ << flush;
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
        Logger::file_ << Logger::free_buf_ << flush;
    }
    if (busy_buf_.size()) {
        Logger::file_ << Logger::busy_buf_ << flush;
    }
    file_.close();
}

void Logger::SetFileName(const string file_name) {
    if (file_name != Logger::file_name_) {
        file_name_ = file_name;
        if (Logger::file_.is_open()) {
            file_.close();
        }
    }
    if (!file_.is_open()) {
        file_ = fstream(file_name, ios::app);
    }
}

void Logger::Format() {
    time_t rawtime;
    time(&rawtime);
    tm timeinfo;
    localtime_r(&rawtime, &timeinfo);
    char cs[64] = {'\0'};
    strftime(cs, 64, "%Y%m%d %X ", &timeinfo);
    log_line_ += string(cs) + loglevel_str[loglevel_] + " " + FILE_ + " " +
                 FUNCTION_ + " " + to_string(LINE_);
}

void Logger::AddColorBegin() {
    if (Logger::color_) {
        switch (Logger::loglevel_) {
            case LogLevel::FATAL:
                log_line_ = "\033[31m" + log_line_;
                break;
            case LogLevel::ERROR:
                log_line_ = "\033[31m" + log_line_;
                break;
            case LogLevel::WARN:
                log_line_ = "\033[33m" + log_line_;
                break;
            case LogLevel::INFO:
                break;
            case LogLevel::DEBUG:
                log_line_ = "\033[32m" + log_line_;
                break;
            default:
                break;
        }
    }
}

void Logger::AddColorEnd() {
    if (Logger::color_) {
        log_line_ += "\033[0m";
    }
}

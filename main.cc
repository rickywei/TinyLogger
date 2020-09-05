#include <unistd.h>

#include <iostream>
#include <thread>

#include "Logger.h"
using namespace std;
using namespace log;

int main() {
    Logger::Init(true, true);

    LOG() << "LOG";
    LOGL(LogLevel::INFO) << "LOGL";
    FATAL() << "FATAL";
    ERROR() << "ERROR";
    WARN() << "WARN";
    INFO() << "INFO";
    DEBUG() << "DEBUG";

    sleep(1);

    Logger::Stop();

    return 0;
}
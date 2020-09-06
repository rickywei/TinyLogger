#include "Logger.h"
using namespace std;
using namespace log;

int main() {
    Logger::Init(true, false);
    Logger::SetColor(true);

    LOG() << "LOG";
    LOGL(LogLevel::INFO) << "LOGL";
    FATAL() << "FATAL";
    ERROR() << "ERROR";
    WARN() << "WARN";
    INFO() << "INFO";
    DEBUG() << "DEBUG";

    Logger::Stop();

    return 0;
}
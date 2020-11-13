#include <unistd.h>

#include "Logger.h"

using namespace tiny;

int main() {
  Logger::Init(true);
  Logger::SetColor(true);

  LOG() << "LOG"
        << "test";
  LOGL(LogLevel::INFO) << "LOGL";
  FATAL() << "FATAL";
  ERROR() << "ERROR";
  WARN() << "WARN";
  INFO() << "INFO";
  DEBUG() << "DEBUG";

  sleep(3);

  Logger::Stop();

  return 0;
}
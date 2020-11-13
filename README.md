# TingLogger

A C++ Asynchronous Logging library

![GitHub](https://img.shields.io/github/license/RickyWei/TinyLogger)
![GitHub last commit](https://img.shields.io/github/last-commit/RickyWei/TinyLogger)
![Travis (.com)](https://img.shields.io/travis/com/RickyWei/TinyLogger)

## Features

- C++ string stream sytle
- Splited frontend and backend log buffer
- Write logs back to disk by time and size
- Colorful command line output
- [https://rickywei.github.io/blog/design/logger.html](https://rickywei.github.io/blog/design/logger.html)

## example

```cpp
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
```

![example](./example.jpg)

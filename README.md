# TingLogger
A C++ Asynchronous Logging library

![GitHub](https://img.shields.io/github/license/RickyWei/TinyLogger)
![GitHub last commit](https://img.shields.io/github/last-commit/RickyWei/TinyLogger)
![Travis (.com)](https://img.shields.io/travis/com/RickyWei/TinyLogger)


## How to use

Just inculde the hear file and source file in your project

## example

```cpp
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
```

![example](./example.jpg)


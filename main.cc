#include <iostream>
#include <thread>

#include "Logger.h"
using namespace std;
using namespace log;

int main() {
    Logger::Init(true, false);

    Logger() << "123";

    return 0;
}
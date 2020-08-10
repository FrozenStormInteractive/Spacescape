#include <iostream>

#include "Logging/ConsoleLogOutput.h"

using namespace spacescape;

void ConsoleLogOutput::Write(const LogMessage& message)
{
#if defined(Q_OS_WIN)
    std::cout << message.ToString().toLocal8Bit().constData();
#else
    std::cout << message.ToString().toUtf8().constData();
#endif
    std::cout << std::endl;
}

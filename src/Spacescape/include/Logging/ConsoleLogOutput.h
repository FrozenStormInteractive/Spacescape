#ifndef SPACESCAPE_CONSOLELOGOUTPUT_H
#define SPACESCAPE_CONSOLELOGOUTPUT_H

#include "LogOutput.h"

namespace spacescape
{
    class ConsoleLogOutput : public ILogOutput
    {
        void Write(const LogMessage &message) override;
    };
}

#endif //SPACESCAPE_CONSOLELOGOUTPUT_H

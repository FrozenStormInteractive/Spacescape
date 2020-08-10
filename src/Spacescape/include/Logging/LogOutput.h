#ifndef SPACESCAPE_LOGOUTPUT_H
#define SPACESCAPE_LOGOUTPUT_H

#include "Log.h"

namespace spacescape
{
    class ILogOutput
    {
    public:
        virtual void Write(const LogMessage &message) = 0;
    };
}

#endif //SPACESCAPE_LOGOUTPUT_H

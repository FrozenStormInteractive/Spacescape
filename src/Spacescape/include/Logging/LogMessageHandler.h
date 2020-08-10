#ifndef SPACESCAPE_LOGMESSAGEHANDLER_H
#define SPACESCAPE_LOGMESSAGEHANDLER_H

#include <QSharedPointer>

#include "LogOutput.h"

namespace spacescape
{
    void InitLogMessageHandler(QSharedPointer<ILogOutput> output);

    QtMessageHandler CreateLogMessageHandler(QSharedPointer<ILogOutput> output);
}

#endif //SPACESCAPE_LOGMESSAGEHANDLER_H

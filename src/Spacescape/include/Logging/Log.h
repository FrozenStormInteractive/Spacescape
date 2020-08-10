#ifndef SPACESCAPE_LOG_H
#define SPACESCAPE_LOG_H

#include <QDateTime>
#include <QMessageLogContext>

namespace spacescape
{
    struct LogMessage
    {
        QtMsgType Type;
        QDateTime Time;
        QString MessageString;

        QString ToString() const;
    };
}

#endif //SPACESCAPE_LOG_H

#include "Logging/LogMessageHandler.h"

using namespace spacescape;

QSharedPointer<ILogOutput> CurrentLogOutput = nullptr;

void InternalLogMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    CurrentLogOutput->Write(LogMessage { type, QDateTime::currentDateTime(), msg });
}

namespace spacescape
{
    QtMessageHandler CreateLogMessageHandler(QSharedPointer<ILogOutput> output) {
        CurrentLogOutput = output;
        return InternalLogMessageHandler;
    }
}

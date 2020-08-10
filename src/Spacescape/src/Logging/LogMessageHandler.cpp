#include "Logging/LogMessageHandler.h"

using namespace spacescape;

QSharedPointer<ILogOutput> CurrentLogOutput = nullptr;

void InternalLogMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    CurrentLogOutput->Write(LogMessage { type, QDateTime::currentDateTime(), msg });
}

namespace spacescape
{
    void InitLogMessageHandler(QSharedPointer<ILogOutput> output) {
        qInstallMessageHandler(spacescape::CreateLogMessageHandler(output));
    }

    QtMessageHandler CreateLogMessageHandler(QSharedPointer<ILogOutput> output) {
        CurrentLogOutput = output;
        return InternalLogMessageHandler;
    }
}

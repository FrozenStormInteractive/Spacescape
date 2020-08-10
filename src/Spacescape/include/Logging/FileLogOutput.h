#ifndef SPACESCAPE_FILELOGOUTPUT_H
#define SPACESCAPE_FILELOGOUTPUT_H

#include <QScopedPointer>
#include <QFile>

#include "LogOutput.h"

namespace spacescape
{
    class FileLogOutput : public ILogOutput
    {
    public:
        FileLogOutput(const QString& filename);

        void Write(const LogMessage &message) override;

    private:
        QScopedPointer<QFile> LogFile;
    };
}

#endif //SPACESCAPE_FILELOGOUTPUT_H

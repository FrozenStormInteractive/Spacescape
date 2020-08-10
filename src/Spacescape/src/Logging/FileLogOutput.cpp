#include <iostream>

#include <QTextStream>

#include "Logging/FileLogOutput.h"

using namespace spacescape;

FileLogOutput::FileLogOutput(const QString &filename)
{
    LogFile.reset(new QFile(filename));
    LogFile->open(QFile::Append | QFile::Text);
}

void FileLogOutput::Write(const LogMessage& message)
{
    QTextStream out(LogFile.data());
    out << message.ToString() << endl;
    out.flush();
}

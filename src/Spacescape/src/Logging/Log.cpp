#include "Logging/Log.h"

using namespace spacescape;


QString LogMessage::ToString() const
{
    QString out = Time.toString("[yyyy-MM-dd hh:mm:ss.zzz]");

    out += '[';
    switch (Type)
    {
        case QtInfoMsg: {
            out += "Info";
            break;
        }
        case QtDebugMsg: {
            out += "Debug";
            break;
        }
        case QtWarningMsg: {
            out += "Warning";
            break;
        }
        case QtCriticalMsg: {
            out += "Critical";
            break;
        }
        case QtFatalMsg: {
            out += "Fatal";
            break;
        }
    }
    out += ']';

    out += ' ' + MessageString;

    return out;
}
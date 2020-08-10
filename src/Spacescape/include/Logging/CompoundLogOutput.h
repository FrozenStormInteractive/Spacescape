#ifndef SPACESCAPE_COMPOUNDLOGOUTPUT_H
#define SPACESCAPE_COMPOUNDLOGOUTPUT_H

#include <initializer_list>

#include <QSharedPointer>
#include <QVector>

#include "LogOutput.h"

namespace spacescape
{
    class CompoundLogOutput : public ILogOutput
    {
    public:
        CompoundLogOutput(std::initializer_list<QSharedPointer<ILogOutput>> outputs);

        void Write(const LogMessage &message) override;

    private:
        QVector<QSharedPointer<ILogOutput>> Outputs;
    };
}

#endif //SPACESCAPE_COMPOUNDLOGOUTPUT_H

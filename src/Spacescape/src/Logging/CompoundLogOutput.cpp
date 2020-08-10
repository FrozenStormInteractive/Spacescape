#include "Logging/CompoundLogOutput.h"

using namespace spacescape;

CompoundLogOutput::CompoundLogOutput(std::initializer_list<QSharedPointer<ILogOutput>> outputs):
    Outputs(outputs)
{
}

void CompoundLogOutput::Write(const LogMessage& message)
{
    for(const QSharedPointer<ILogOutput>& output : Outputs)
    {
        output->Write(message);
    }
}

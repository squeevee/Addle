#ifndef FORMATMISMATCHMESSAGE_HPP
#define FORMATMISMATCHMESSAGE_HPP

#include "data/formatid.hpp"
#include "interfaces/tasks/itaskmessage.hpp"

/**
 * @class FormatMismatchMessage
 * @brief Indicates that a formatted object was implied to be one format, but
 * proved to be another.
 * 
 * 
 */
class FormatMismatchMessage : public ITaskMessage
{
    TASK_MESSAGE_BOILERPLATE(FormatMismatchMessage)
public:
    FormatMismatchMessage(FormatId impliedFormat, FormatId inferredFormat)
        : _impliedFormat(impliedFormat), _inferredFormat(inferredFormat)
    {
    }
    virtual ~FormatMismatchMessage() = default;

    FormatId getImpliedFormat() const { return _impliedFormat; }
    FormatId getInferredFormat() const { return _inferredFormat; }

private:
    FormatId _impliedFormat;
    FormatId _inferredFormat;
};

#endif // FORMATMISMATCHMESSAGE_HPP
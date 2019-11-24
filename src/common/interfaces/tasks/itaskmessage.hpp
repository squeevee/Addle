#ifndef ITASKMESSAGE_HPP
#define ITASKMESSAGE_HPP

#include <typeinfo>

class ITaskMessage
{
public:
    virtual ~ITaskMessage() = default;

    virtual const std::type_info& type_info() = 0;
};

#define TASK_MESSAGE_BOILERPLATE(T) \
public: \
    const std::type_info& type_info() { return typeid(T); }

#endif // ITASKMESSAGE_HPP
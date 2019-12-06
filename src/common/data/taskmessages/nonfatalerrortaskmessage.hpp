#ifndef NONFATALERRORTASKMESSAGE_HPP
#define NONFATALERRORTASKMESSAGE_HPP

#include "interfaces/tasks/itaskmessage.hpp"
#include "interfaces/iaddleexception.hpp"

#include <QSharedPointer>

class NonFatalErrorTaskMessage : public ITaskMessage
{
    TASK_MESSAGE_BOILERPLATE(NonFatalErrorTaskMessage)
public:
    NonFatalErrorTaskMessage(QSharedPointer<IAddleException> exception)
        : _exception(exception)
    {
    }
    virtual ~NonFatalErrorTaskMessage() = default;

    QSharedPointer<IAddleException> getException() const { return _exception; }
    const std::type_info& getExceptionTypeInfo() const { return _exception ? _exception->type_info() : typeid(void); }

private:
    QSharedPointer<IAddleException> _exception;
};

#endif // NONFATALERRORTASKMESSAGE_HPP
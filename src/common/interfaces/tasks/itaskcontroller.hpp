#ifndef ITASKCONTROLLER_HPP
#define ITASKCONTROLLER_HPP

#include "interfaces/iaddleexception.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "itask.hpp"
#include "interfaces/traits/makeable_trait.hpp"

class ITaskController 
{
public:
    virtual ~ITaskController() = default;

    virtual void initialize(ITask* task) = 0;

    virtual ITask* getTask() = 0;

    virtual QSharedPointer<IAddleException> getError() = 0;

signals:
    virtual void started(ITask* task) = 0;
    virtual void done(ITask* task) = 0;

public slots:
    virtual void run() = 0;
};

DECL_MAKEABLE(ITaskController)
DECL_EXPECTS_INITIALIZE(ITaskController)
DECL_IMPLEMENTED_AS_QOBJECT(ITaskController)

#endif // ITASKCONTROLLER_HPP
#ifndef ITASKSERVICE_HPP
#define ITASKSERVICE_HPP

#include "iservice.hpp"

#include <QSharedPointer>

#include "common/interfaces/tasks/itask.hpp"

#include "common/interfaces/traits/initialize_traits.hpp"
#include "common/interfaces/traits/qobject_trait.hpp"
#include "common/interfaces/servicelocator/imakeable.hpp"

#include "common/interfaces/tasks/itaskcontroller.hpp"

class ITaskService : public virtual IService,  public virtual IMakeable
{
public:
    virtual ~ITaskService() = default;

    virtual void queueTask(ITask* task) = 0;
};

DECL_IMPLEMENTED_AS_QOBJECT(ITaskService)

#endif // ITASKSERVICE_HPP
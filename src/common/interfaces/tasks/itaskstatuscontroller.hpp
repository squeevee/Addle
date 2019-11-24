#ifndef ITASKSTATUSCONTROLLER_HPP
#define ITASKSTATUSCONTROLLER_HPP

#include <QSharedPointer>
#include "itaskmessage.hpp"

class ITaskStatusController
{
public:
    virtual ~ITaskStatusController() = default;

    virtual double getProgress() = 0;
    virtual void setProgress(double progress) = 0;

    virtual void postMessage(QSharedPointer<ITaskMessage> message) = 0;
};

#endif // ITASKSTATUSCONTROLLER_HPP
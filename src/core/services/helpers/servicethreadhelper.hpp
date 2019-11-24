#ifndef SERVICETHREADHELPER_HPP
#define SERVICETHREADHELPER_HPP

#include "common/interfaces/services/iservice.hpp"

#include <typeinfo>
#include <QObject>
#include <QThread>
#include <QCoreApplication>

namespace ServiceThreadHelper_options
{
    enum ThreadOption {
        own_thread,
        application_thread
    };
}

template<class Service>
class ServiceThreadHelper
{
    typedef ServiceThreadHelper_options::ThreadOption ThreadOption;

public:
    ServiceThreadHelper(Service* owner, ThreadOption threadOption = ThreadOption::own_thread)
        : _owner(owner), _serviceThread(nullptr), _threadOption(threadOption)
    {
        static_assert(
            std::is_base_of<QObject, Service>::value,
            "Service must inherit QObject"
        );
        static_assert(
            std::is_base_of<IService, Service>::value,
            "Service must inherit IService"
        );

        auto q_owner = static_cast<QObject*>(owner);

        switch (_threadOption)
        {
            case ThreadOption::own_thread:
            {
                _serviceThread = new QThread;
                _serviceThread->setObjectName(
                    IService::SERVICE_THREAD_NAME_TEMPLATE
                        .arg(q_owner->metaObject()->className())
                );
                q_owner->moveToThread(_serviceThread);
                _serviceThread->start();
                break;
            }
            case ThreadOption::application_thread:
            {
                QThread* applicationThread = QCoreApplication::instance()->thread();
                if (q_owner->thread() != applicationThread)
                    q_owner->moveToThread(applicationThread);
                break;
            }
        }
        
    }

    ~ServiceThreadHelper()
    {
        if (_serviceThread)
        {
            _serviceThread->quit();
            _serviceThread->wait();
            _serviceThread->deleteLater();
        }
    }

    QThread* getServiceThread() { return _serviceThread; }

    void assertThread()
    {
        if (QThread::currentThread() != _owner->thread())
            throw "a fit"; //TODO
    }

private:
    Service* _owner;
    QThread* _serviceThread;

    const ThreadOption _threadOption;
};

#endif // SERVICETHREADHELPER_HPP
#ifndef QOBJECTFACTORY_HPP
#define QOBJECTFACTORY_HPP

#include <QObject>

#include <QApplication>
#include <QThread>

#include <type_traits>

#include "utilities/configuration/tfactory.hpp"

#include "exceptions/baseaddleexception.hpp"

namespace QObjectFactory_options
{
    enum ThreadingOptions
    {
        permissive,
        main_thread_only
    };
}

template<class Interface, class Impl>
class QObjectFactory : public TFactory<Interface, Impl>
{
    static_assert(
        std::is_base_of<QObject, Impl>::value,
        "Impl must inherit QObject"
    );
    typedef QObjectFactory_options::ThreadingOptions ThreadingOptions;

#ifdef ADDLE_DEBUG
public:
    QObjectFactory(ThreadingOptions threading = ThreadingOptions::permissive)
        : _threading(threading)
    {
    }
#else
public:
    QObjectFactory(ThreadingOptions threading = ThreadingOptions::permissive)
        : _threading(threading)
    {
    }
#endif
public:
    virtual ~QObjectFactory() = default;

    void* make()
    {
        if (_threading == ThreadingOptions::main_thread_only
            && QThread::currentThread() != QApplication::instance()->thread())
        {
#ifdef ADDLE_DEBUG
            FactoryException ex(FactoryException::Why::wrong_thread,
                getProductTypeName(),
                getFactoryTypeName()
            );
#else   
            FactoryException ex(FactoryException::Why::wrong_thread);
#endif
            ADDLE_THROW(ex);
        }

        return TFactory<Interface, Impl>::make();
    }

#ifdef ADDLE_DEBUG
    const char* getFactoryTypeName()
    {
        static const QByteArray factoryName = QStringLiteral("QObjectFactory<%1>").arg(getProductTypeName()).toUtf8();
        return factoryName.constData();
    }
    const char* getProductTypeName()
    {
        static const char* productName = Impl::staticMetaObject.className();
        return productName;
    }
#endif

private:
    const ThreadingOptions _threading;

};

#define REGISTER_QOBJECT_FACTORY(Interface, Impl, ...) \
    BaseServiceConfiguration::registerFactory<Interface, QObjectFactory<Interface, Impl>, Impl>(__VA_ARGS__);

#endif // QOBJECTFACTORY_HPP
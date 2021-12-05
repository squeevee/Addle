#include <boost/core/demangle.hpp>
#include <boost/stacktrace/frame.hpp>

#include <QCoreApplication>
#include <QThread>

#include "./debugstring.hpp"
#include "./qdebug_extensions.hpp"
#include "./stacktrace_config.hpp"

#if defined(ADDLE_DEBUG) || defined(ADDLE_TEST)

QByteArray Addle::aux_debug::_typeNameHelper(const std::type_info& type)
{
    return QByteArray::fromStdString( boost::core::demangle(type.name()) );
}

QDebug Addle::aux_debug::operator<<(QDebug debug, CurrentThreadInfo_t)
{
    QString name;
    if (QThread::currentThread())
        name = QThread::currentThread()->objectName();
    
    if (!name.isEmpty())
        debug << name;
    
    if (name.isEmpty() || debug.verbosity() > QDebug::DefaultVerbosity)
        debug << QThread::currentThreadId();
    
    
    if (QCoreApplication::instance() && QCoreApplication::instance()->thread() == QThread::currentThread())
    {
        QDebugStateSaver s(debug);
        debug.noquote() << ' ' << (
            //% "(Main Thread)"
            qtTrId("debug-messages.util.main-thread")
        );
    }
    
    return debug;
}

void Addle::aux_debug::_functionHelper(QDebug& d, const void* function)
{
    boost::stacktrace::frame f(function);
    if (d.verbosity() <= QDebug::DefaultVerbosity)
    {
        auto name = f.name();
        if (!name.empty())
            d << name.data();
        else
            d << function;
    }
    else
    {
        d << to_string(f).data();
    }
}


#endif // defined(ADDLE_DEBUG) || defined(ADDLE_TEST)

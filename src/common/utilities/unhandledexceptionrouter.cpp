#include <QCoreApplication>
#include "unhandledexceptionrouter.hpp"

using namespace Addle;

UnhandledExceptionRouter* UnhandledExceptionRouter::_instance = nullptr;

void UnhandledExceptionRouter::initialize()
{
    if (!_instance)
    {
        _instance = new UnhandledExceptionRouter();
        QThread* mainThread = QCoreApplication::instance()->thread();
        if (_instance->thread() != mainThread)
            _instance->moveToThread(mainThread);
    }
}

void UnhandledExceptionRouter::report(AddleUnhandledException& ex)
{
    if (!_instance) return;
    emit _instance->unhandledException(&ex);
}
    
void UnhandledExceptionRouter::report(AddleException& ex, Severity severity)
{
    if (!_instance) return;
    AddleUnhandledException unhandled(ex, severity);
    emit _instance->unhandledException(&unhandled);
}

void UnhandledExceptionRouter::report(std::exception& ex, Severity severity)
{
    if (!_instance) return;
    AddleUnhandledException unhandled(ex, severity);
    emit _instance->unhandledException(&unhandled);
}

#ifdef ADDLE_DEBUG
void UnhandledExceptionRouter::reportFallthrough(const char* function, const char* file, int line, Severity severity)
#else
void UnhandledExceptionRouter::reportFallthrough(Severity severity)
#endif
{
    AddleUnhandledException unhandled(severity);
#ifdef ADDLE_DEBUG
    try { unhandled.debugRaise(function, file, line); } catch (...) {}
#endif
    emit _instance->unhandledException(&unhandled);
}
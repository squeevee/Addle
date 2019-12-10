#ifndef GLOBALEXCEPTIONHANDLER_HPP
#define GLOBALEXCEPTIONHANDLER_HPP

#include "interfaces/iaddleexception.hpp"
#include "exceptions/baseaddleexception.hpp"
#include <exception>
#include <QCoreApplication>
#include <QObject>

class AddleUnhandledException;

/**
 * @class UnhandledExceptionRouter
 * @brief Utility for reporting unhandled exceptions
 * 
 * Typically when a runtime error occurs, there will be a contexually
 * appropriate means for dealing with it -- i.e., reporting it to the user via
 * a relevant part of the UI with rich information.
 * 
 * UnhandledExceptionRouter is not rich or intelligent error handling, but
 * rather a failsafe against extremely exceptional conditions -- e.g., logic
 * errors, failed sanity checks, an uncaught throw from lower-level code.
 * 
 * UnhandledExceptionRouter does not perform any logic on its own, it only emits
 * its `unhandledException()` signal when an exception is reported to it. Any
 * code that is concerned with information about these errors, (for example the 
 * ApplicationService or a testing utility) may to connect to this signal.
 * 
 * Exceptions should generally be reported using the ADDLE_FALLBACK_CATCH macro,
 * or the ADDLE_FALLBACK_CATCH__SEVERITY if desired. These expand to a series of
 * catch blocks, so place them after a try block containing the protected code,
 * and after any other catch blocks.
 *  
 * @example
void Class::slotName()
{
    try
    {
        ...
    }
    catch (KnownRuntimeError& ex)
    {
        ...
    }
    ADDLE_FALLBACK_CATCH
}
 *
 * Exceptions are not allowed to bleed into the Qt event loop, so all slots and
 * event handlers should be wrapped in such a construct. It also may be
 * appropriate to add similar protection to other parts of the code.
 *  
 * Hopefully, UnhandledExceptionRouter will be most relevant to automated tests
 * to aid the debugging of logic errors if they occur. In a perfect world, it
 * would never be invoked in production code.
 * 
 * The reporting code may specify a Severity for the unhandled exception event.
 * Consuming code may use this information as it sees fit.
 * 
 * - Normal (the default) implies the user should be alerted immediately that an
 * error occurred as some operation is likely to have failed as a result of it.
 * - Trivial implies that the user may not need to be direclty notified and that
 * an operation is not likely to have failed because of this error.
 * - Serious implies that the state of the program (beyond the current
 * operation) may have become irreparably inconsistent or unstable as a result
 * of or in connection with this error and that the program should be
 * terminated.
 * 
 * Bear in mind that this class has no jurisdiction over the much lower-level
 * C error signals.
 */
class UnhandledExceptionRouter : public QObject
{
    Q_OBJECT 
public:
    enum Severity
    {
        normal,
        trivial,
        serious
    };

    virtual ~UnhandledExceptionRouter() = default;

    // Call this function after the QApplication has been created so
    // UnhandledExceptionRouter can live on the main thread.
    static void initialize();

    const UnhandledExceptionRouter* getInstance() { return _instance; }

    static void report(AddleUnhandledException& ex);
    static void report(IAddleException& ex, Severity severity = Severity::normal);
    static void report(std::exception& ex, Severity severity = Severity::normal);

#ifdef ADDLE_DEBUG
    static void reportFallthrough(const char* function, const char* file, int line, Severity severity = Severity::normal);
#else
    static void reportFallthrough(Severity severity = Severity::normal);
#endif

signals: 
    // The AddleUnhandledException object pointed to by this signal is
    // transient. For best results, the receiver should live on the main thread
    // and connect to this signal with Qt::DirectConnection. A
    // Qt::QueuedBlockingConnection may also work if the receiver needs to live
    // on a different thread, but bear in mind that if that thread calls
    // `report` then this may result in a deadlock.
    void unhandledException(AddleUnhandledException* ex) const;

private: 
    UnhandledExceptionRouter() = default;
    static UnhandledExceptionRouter* _instance;
};

/**
 * @class AddleUnhandledException
 * @brief Represents an error that was unhandled somewhere
 */
class AddleUnhandledException : public BaseAddleException
{
    ADDLE_EXCEPTION_BOILERPLATE(AddleUnhandledException)
    typedef UnhandledExceptionRouter::Severity Severity;
public:
    AddleUnhandledException(Severity severity = Severity::normal)
        : BaseAddleException(
#ifdef ADDLE_DEBUG
            QCoreApplication::translate(
                "UnhandledAddleException",
                "An unhandled exception occurred."
            )
#endif
        ),
        _severity(severity)
    {
    }

    AddleUnhandledException(IAddleException& innerException, Severity severity = Severity::normal)
        : AddleUnhandledException(severity)
    {
        _innerAddleException = innerException.clone();
    }

    AddleUnhandledException(std::exception& innerException, Severity severity = Severity::normal)
        : AddleUnhandledException(severity)
    {
        _innerStdException = new std::exception(innerException);
    }

    AddleUnhandledException(const AddleUnhandledException& other)
        : AddleUnhandledException(other._severity)
    {
        if (other._innerAddleException)
            _innerAddleException = other._innerAddleException->clone();
        if (other._innerStdException)
            _innerStdException = new std::exception(*other._innerStdException);
    }

    virtual ~AddleUnhandledException()
    {
        if (_innerAddleException) delete _innerAddleException;
        if (_innerStdException) delete _innerStdException;
    }

    bool getSeverity() { return _severity; }

    IAddleException* getInnerAddleException() { return _innerAddleException; }
    std::exception* getInnerStdException() { return _innerStdException; }

private:
    IAddleException* _innerAddleException = nullptr;
    std::exception* _innerStdException = nullptr;
    Severity _severity;
};

#ifdef ADDLE_DEBUG
#define __ADDLE_UNHANDLED_EXCEPTION_FALLTHROUGH(x) UnhandledExceptionRouter::reportFallthrough(Q_FUNC_INFO, __FILE__, __LINE__, x);
#else
#define __ADDLE_UNHANDLED_EXCEPTION_FALLTHROUGH(x) UnhandledExceptionRouter::reportFallthrough(x);
#endif 

#define ADDLE_FALLBACK_CATCH__SEVERITY(x) \
catch (IAddleException& ex) \
{ \
    UnhandledExceptionRouter::report(ex, x); \
} \
catch (std::exception& ex) \
{ \
    UnhandledExceptionRouter::report(ex, x); \
} \
catch(...) \
{ \
__ADDLE_UNHANDLED_EXCEPTION_FALLTHROUGH(x) \
}

#define ADDLE_FALLBACK_CATCH ADDLE_FALLBACK_CATCH__SEVERITY(UnhandledExceptionRouter::Severity::normal)

#endif // GLOBALEXCEPTIONHANDLER_HPP
/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifdef ADDLE_DEBUG

#include "addleexception.hpp"

#include <cstring>  // std::strlen
#include <cmath>    // std::log10

#include <boost/stacktrace/frame.hpp>
#include <boost/core/demangle.hpp>

#include <QStringBuilder>
#include <QTextStream>
#include <QRegularExpression>

#include "utils.hpp"

#ifdef Q_OS_UNIX
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#include <QThread>
#include <QtDebug>
#include "utilities/debugging/debugbehavior.hpp"
#include <utilities/debugging/qdebug_extensions.hpp>

using namespace Addle;

AddleException::AddleException(AddleExceptionBuilder builder)
    : _innerExceptions(std::move(builder.innerExceptions))
    , _what(std::move(builder.what))
    , _properties(std::move(builder.properties))
{
}

AddleException::AddleException(const AddleException& other)
    : _innerExceptions(other._innerExceptions)
    , _history(other._history)
    , _what(other._what)
    , _properties(other._properties)
{
}

AddleException::~AddleException()
{
}

void AddleException::debugRaise(const char* function, const char* file, const int line)
{
    QString threadName;
    
    auto thread = QThread::currentThread();
    if (thread) threadName = thread->objectName();
    
    if (threadName.isEmpty() && QCoreApplication::instance() && thread == QCoreApplication::instance()->thread())
    {
        //% "<Main Thread>"
        threadName = qtTrId("debug-messages.main-thread-name");
    }
    
    HistoryEntry entry { function, file, line, QDateTime::currentDateTime(), threadName, QThread::currentThreadId() };

    _history.append(entry);
    dataChanged();

    if (isLogicError())
    {
        if (_history.size() == 1)
        {
            qWarning("%s", qUtf8Printable(
                //% "A logic error has occurred: %1"
                qtTrId("debug-messages.logic-error-occurred")
                    .arg(what_p(2))
            ));
        }

        if (DebugBehavior::test(DebugBehavior::AbortOnLogicErrorThrown))
        {
            std::abort();
        }
    }

    raise();
    Q_UNREACHABLE();
}

const char* AddleException::what() const noexcept
{    
    static const char* invalid = "";
    
    if (Q_UNLIKELY(!_whatBytesMutex.tryLock())) return invalid;
    
    if (!_whatBytes.isEmpty())
    {
        const char* result = _whatBytes.constData();
        _whatBytesMutex.unlock();
        return result;
    }
    
    try
    {
        _whatBytes = what_p(INT_MAX).toUtf8();
        
        if (Q_UNLIKELY(_whatBytes.isEmpty()))
            _whatBytes.append('\0');
            // technically no longer empty
        
        const char* result = _whatBytes.constData();
        _whatBytesMutex.unlock();
        return result;
    }
    catch(...)
    {
        _whatBytesMutex.unlock();
        return invalid;
    }
}

AddleException::debug_data_t AddleException::debugData() const 
{
    debug_data_t result;
    result.reserve(_properties.size());
    for (auto it = _properties.cbegin(); it != _properties.cend(); ++it)
        result.append({ it.key(), (*it)(*this) });
    
    return result;
}

AddleException::debug_data_t AddleException::debugData_p() const 
{
    debug_data_t result;
    
    QMutexLocker lock(&_debugDataCacheMutex);
    if (_debugDataCacheIsSet)
        return (result = _debugDataCache);
    
    result = debugData();
    _debugDataCacheIsSet = true;

    return (_debugDataCache = result);
}

void AddleException::dataChanged()
{
    _whatBytesMutex.lock();
    _debugDataCacheMutex.lock();
    
    _whatBytes.clear();
    _debugDataCache.clear();
    
    _debugDataCacheMutex.unlock();
    _whatBytesMutex.unlock();
}

// TODO I should implement my own terminate handler for debug builds that can 
// interface with the exception and tell it to insert color codes and fancy 
// formatting in its explanatory message. Implementing `std::exception::what` 
// may end up just being a formality that is unlikely to be used.

// TODO for the benefit of logging, I might want to have an exception's debug 
// data be serializable.

QString AddleException::what_p(int verbosity) const
{
    if (verbosity <= 0) return QString();
    
    if (verbosity == 1) return _what;
    
    auto className = boost::core::demangle(typeid(*this).name());
    
    if (verbosity == 2) 
    {
        QString dataString;
        
        for (const auto& entry : noDetach(debugData_p()))
        {
            dataString += QStringLiteral(", ") % entry.first % ": " % entry.second;
        }
        
        return QStringLiteral("%1(\"%2\"%3)")
            .arg(className.data())
            .arg(_what.leftRef(256))
            .arg(dataString);
    }
    
    QString result;
    QTextStream stream(&result);
    int blockwidth = 0;
    bool color = false;
    
#ifdef Q_OS_UNIX
    if (isatty(STDOUT_FILENO))
    {
        struct winsize w;
        if (!ioctl(STDOUT_FILENO, TIOCGWINSZ, &w))
        {
            blockwidth = w.ws_col;
            stream << "-\n";
        }
        
//         color = verbosity > QDebug::MaximumVerbosity 
//             && !std::strcmp(getenv("TERM"), "xterm-256color");
//         // obviously not very portable but this is just sugar so it's fine
    }
#endif

    if (color) stream << "\x1B[46m\x1B[30m\x1B[1m"; // background cyan, foreground black, bold
    if (!blockwidth) stream << " ";
    stream.setFieldWidth(blockwidth);
    stream << Qt::center 
        << (
            //% "=== Instance of exception type `%1` ==="
            qtTrId("debug-messages.exception.instance-of-header")
                .arg(className.data())
        )
        << Qt::reset;
        
    if (color) stream << "\x1B[0m\x1B[31m"; // reset, foreground red
        
    stream << "\n\n" << Qt::left << _what << "\n\n";

    if (isLogicError())
    {
        stream << (
                //% "(this is a logic error)"
                qtTrId("debug-messages.exception.is-logic-error")
            ) << "\n";
    }
    
    if (isRuntimeError())
    {
        stream << (
                //% "(this is a runtime error)"
                qtTrId("debug-messages.exception.is-runtime-error")
            ) << "\n";
    }
    
    if (color) stream << "\x1B[0m"; // reset
    
    if (verbosity >= 3)
    {
        auto data = debugData_p();
        if (!data.isEmpty())
        {
            if (color) stream << "\x1B[37m\x1B[1m"; // foreground cyan, bold
            
            if (!blockwidth) stream << "  ";
            stream.setFieldWidth(blockwidth);
            stream << Qt::center 
                << (
                    //% "==== Exception data ===="
                    qtTrId("debug-messages.exception.data-header")
                )
                << Qt::reset << "\n\n";
                
            
            if (color) stream << "\x1B[0m"; // reset
            
            int nameFieldWidth = 0;
            for (const auto& field : noDetach(data))
            {
                if (field.first.size() > nameFieldWidth) 
                    nameFieldWidth = field.first.size();
            }
            
            for (const auto& field : noDetach(data))
            {
                stream << Qt::left << '\t';
                
                stream.setFieldWidth(nameFieldWidth);
                stream << Qt::right 
                    << field.first 
                    << Qt::reset << Qt::left << " :\t"
                    << field.second << "\n";
            }
            stream << "\n";
        }
        
        if (!_innerExceptions.isEmpty())
        {
            if (color) stream << "\x1B[37m\x1B[1m"; // foreground cyan, bold
            
            if (!blockwidth) stream << "  ";
            stream.setFieldWidth(blockwidth);
            stream << Qt::center 
                << (
                    //% "==== Inner exceptions ===="
                    qtTrId("debug-messages.exception.inner-exceptions-header")
                ) << Qt::reset << "\n\n";
                
            if (color) stream << "\x1B[0m"; // reset
            
            for (const auto& inner : noDetach(_innerExceptions))
            {
                struct visitor
                {
                    void operator()(const QSharedPointer<AddleException>& ex) const
                    {
                        if (ex)
                            stream_ << ex->what_p(2);
                        else
                            stream_ << (
                                    //% "(null QSharedPointer<AddleException>)"
                                    qtTrId("debug-messages.exception.null-inner-addleexception")
                                );
                    }
                    
                    void operator()(const std::exception_ptr& ex) const
                    {
                        try 
                        { 
                            if (ex)
                                std::rethrow_exception(ex); 
                            else
                                stream_ << (
                                        //% "(null std::exception_ptr)"
                                        qtTrId("debug-messages.exception.null-std-exception_ptr")
                                    );
                        }
                        catch(const AddleException& ex_)
                        {
                            stream_ << ex_.what_p(2) << '\n';
                        }
                        catch(const std::exception& ex_)
                        {
                            auto className = boost::core::demangle(typeid(ex_).name());
                            stream_ << className.data();
                            
                            const char* what = ex_.what();
                            stream_ << "(\"" 
                                << QByteArray::fromRawData(what, std::min<int>(std::strlen(what), 256))
                                << "\")\n";
                        }
                        catch(...) 
                        {
                            stream_ << (
                                    //% "(instance of unknown exception type)"
                                    qtTrId("debug-messages.exceptions.unknown-inner-exception-type")
                                );
                        }
                    }
                    
                    QTextStream& stream_;
                };
                
                stream << " - ";
                std::visit(visitor { stream }, inner);
            }
        }
    
        if (!_history.isEmpty())
        {
            if (color) stream << "\x1B[37m\x1B[1m"; // foreground cyan, bold
            
            if (!blockwidth) stream << "  ";
            stream.setFieldWidth(blockwidth);
            stream << Qt::center 
                << (
                    //% "==== Throw history ===="
                    qtTrId("debug-messages.exception.history-header")
                ) << Qt::reset << "\n\n";
                
            if (color) stream << "\x1B[0m"; // reset
                
            const auto begin = _history.cbegin();
            const auto end = _history.cend();
            
            int indexFieldWidth = std::log10(_history.size()) + 1;
            
            for (auto it = begin; it != end; ++it)
            {
                const auto& entry = *it;
                
                if (color) stream << "\x1B[1m"; // bold
                
                stream << ' ';
                stream.setFieldWidth(indexFieldWidth);
                stream << Qt::right << (it - begin + 1)
                    << Qt::reset << '\t';
                    
                if (color) stream << "\x1B[0m"; // reset
                    
                stream << entry.dateTime.toString(Qt::ISODateWithMs) << '\t'
                    << (
                        //% "on thread \"%1\" (%2)"
                        qtTrId("debug-messages.exception.history-thread")
                            .arg(entry.threadName)
                            .arg(aux_debug::debugString(entry.threadId))
                        ) << "\n\n";
                
                if (color) stream << "\x1B[1m"; // bold    
                stream << (
                        //% "    Location:"
                        qtTrId("debug-messages.exception.history-location")
                    ) << '\n';
                if (color) stream << "\x1B[0m"; // reset
                stream << entry.function << '\n';
                if (color) stream << "\x1B[0m\x1B[37m"; // foreground cyan
                stream << entry.file << ':' << entry.line << "\n\n";
                if (color) stream << "\x1B[0m"; // reset
                
                if (verbosity >= 4 || it == begin)
                {
                    if (color) stream << "\x1B[1m"; // bold
                    stream << (
                            //% "    Backtrace:"
                            qtTrId("debug-messages.exception.history-backtrace")
                        ) << '\n';
                    if (color) stream << "\x1B[0m"; // reset style
                    
                    // The backtrace may contain one or more helper functions 
                    // like `_addle_throw_helper` or `debugRaise`, which are not
                    // useful diagnostic information. The most reliable way to 
                    // detect these I've found is by checking the source 
                    // filenames.
                    static const std::string skipFiles[] = { "addleexception.hpp", "addleexception.cpp" };
                        
                    bool skipping = true;
                    for (const auto& frame : noDetach(entry.backTrace))
                    {
                        if (skipping 
                            && (skipping &= std::any_of( 
                                std::begin(skipFiles), std::end(skipFiles),
                                [&] (const std::string& skipFile) {
                                    const auto sourceFile = frame.source_file();
                                    return !sourceFile.empty() 
                                        && sourceFile.rfind(skipFile) == sourceFile.length() - skipFile.length();
                                }
                            ))
                        )
                        {
                            continue;
                        }
                        
                        auto frameString = boost::stacktrace::to_string(frame);
                        stream << " - " << frameString.data() << '\n';
                    }
                    
                    stream << "\n\n";
                }
            }
        }
        
        
    }
    
    return result;
}
   
#endif // ADDLE_DEBUG

/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include <QStringBuilder>
#include <QRegularExpression>

#include "addleexception.hpp"
#include "utils.hpp"

#ifdef ADDLE_DEBUG

#ifdef __GLIBC__
#include <execinfo.h>
#endif

#ifdef Q_CC_GNU
#include <cxxabi.h>
#endif

#include <QtDebug>
#include "utilities/debugging/debugbehavior.hpp"

using namespace Addle;

#ifdef Q_CC_GNU
static QByteArray _demangleFunctionName(const char* mangledName)
{
    // https://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-html-USERS-4.3/a01696.html
    QByteArray result;
    size_t length;
    int status;
    
    char* rawDemangledName = abi::__cxa_demangle(
        mangledName,
        nullptr,
        &length,
        &status
    );

    if (!status)
    {
        result = QByteArray(rawDemangledName);
    }
    free(rawDemangledName);

    return result;
}
#endif

AddleException::AddleException(const QString& what)
    : _what(what)
{
    updateWhat();
}

void AddleException::debugRaise(const char* function, const char* file, const int line)
{
    Location loc;
    loc.function = function;
    loc.file = file;
    loc.line = line;
    _locations.push(loc);

    if (_locations.size() == 1)
    {
        if (!_firstRaiseBacktrace.isEmpty())
            //% "The exception unexpectedly already had a backtrace."
            qWarning() << qUtf8Printable(qtTrId("debug-messages.exception-already-has-backtrace"));

#if defined(__GLIBC__)
    // https://www.gnu.org/software/libc/manual/html_node/Backtraces.html
        void* traceData[255];
        int traceDataSize = backtrace(traceData, 255);

        char** traceStrings = backtrace_symbols(traceData, traceDataSize);

        if (traceStrings)
        {
            _firstRaiseBacktrace.reserve(traceDataSize);
            for (int i = 0; i < traceDataSize; ++i)
            {
                QString traceFrameString(traceStrings[i]);
#ifdef Q_CC_GNU
                // group 1 matches the mangled function name as output by glibc
                static const QRegularExpression matchMangledFunctionName(
                    QStringLiteral("\\((\\S+)\\+0x[\\da-fA-F]+\\)")
                );
                auto match = matchMangledFunctionName.match(traceFrameString);
                if (match.hasMatch())
                {
                    QByteArray mangledName = match.captured(1).toLatin1();
                    QByteArray demangledName = _demangleFunctionName(mangledName.constData());

                    if (!demangledName.isEmpty())
                    {
                        traceFrameString.replace(
                            match.capturedStart(1),
                            match.capturedLength(1),
                            demangledName.constData()
                        );
                    }
                }
#endif // Q_CC_GNU
                _firstRaiseBacktrace.append(traceFrameString);
            }
            free(traceStrings);
        }
#endif
    }

    updateWhat();

    if (isLogicError())
    {
        if (_locations.size() == 1)
        {
            qWarning() << qUtf8Printable(
                //% "A logic error occurred:\n"
                //% "%1"
                qtTrId("debug-messages.logic-error-occurred")
                    .arg(what())
            );
        }

        if (DebugBehavior::test(DebugBehavior::AbortOnLogicErrorThrown))
        {
            std::abort();
        }
    }

    raise();
    Q_UNREACHABLE();
}

void AddleException::updateWhat()
{
    QString what = _what;

    if (!_locations.isEmpty())
    {
        //% "  thrown from:"
        what = what % QStringLiteral("\n\n") % qtTrId("debug-messages.exception-throw-header") % '\n';

        //% " %1: %2 (%3:%4)"
        what = what % qtTrId("debug-messages.exception-location-formatter")
            .arg(1, 2)
            .arg(_locations.constFirst().function)
            .arg(_locations.constFirst().file)
            .arg(_locations.constFirst().line)
            % '\n';

        if (!_firstRaiseBacktrace.isEmpty())
        {
            //% "  backtrace:"
            what = what % '\n' % qtTrId("debug-messages.exception-backtrace-header") % '\n';

            for (QString backtrace : noDetach(_firstRaiseBacktrace))
            {
                what = what % backtrace % '\n';
            }
        }

        if (_locations.size() > 1)
        {
            //% "  rethrown from:"
            what = what % '\n' % qtTrId("debug-messages.exception-rethrow-header") % '\n';

            int index = 2;
            auto&& start = _locations.constBegin() + 1;
            auto&& end = _locations.constEnd();
            for (auto i = start; i != end; ++i)
            {
                what = what % qtTrId("debug-messages.exception-location-formatter")
                    .arg(index, 2)
                    .arg((*i).function)
                    .arg((*i).file)
                    .arg((*i).line)
                    % '\n';
                ++index;
            }
        }
    }

    _whatBytes = what.toUtf8();
}

#endif // ADDLE_DEBUG
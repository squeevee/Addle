#include <QtDebug>
#include "baseaddleexception.hpp"
#include <QStringBuilder>

#include "utilities/qt_extensions/translation.hpp"

#ifdef ADDLE_DEBUG

#include "utilities/debugging.hpp"

BaseAddleException::BaseAddleException(const QString& what)
    : _what(what)
{
    updateWhat();
}

void BaseAddleException::debugRaise(const char* function, const char* file, const int line)
{
    Location loc;
    loc.function = function;
    loc.file = file;
    loc.line = line;
    _locations.push(loc);
    updateWhat();

    if (isLogicError())
    {
        if (_locations.size() <= 1)
        {
            // Don't repeat this warning for re-thrown exceptions.
            qWarning() << qUtf8Printable(
                fallback_translate(
                    "BaseAddleException",
                    "warn-logic-error",
                    QStringLiteral(
                        "A logic error occurred:\n"
                        "%1"
                    )).arg(what())
                );
        }

        if (DebugBehavior::test(DebugBehavior::abort_on_logic_error))
        {
            std::abort();
        }
    }

    raise();
}

void BaseAddleException::updateWhat()
{
    QString what = _what;
    if (!_locations.isEmpty())
    {
        what = what % QCoreApplication::translate(
            "BaseAddleException",
            "\nthrown from:\n"
        );
        for (auto& loc : _locations)
        {
            what = what % fallback_translate(
                "BaseAddleException",
                "unroll-location",
                QStringLiteral(
                    "%1 (%2:%3)")
                    .arg(loc.function)
                    .arg(loc.file)
                    .arg(loc.line)
            );
        }
    }
    _whatBytes = what.toUtf8();
    _what_cstr = _whatBytes.constData();
}

#endif
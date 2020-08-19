#ifndef COMMANDLINEEXCEPTIONS_HPP
#define COMMANDLINEEXCEPTIONS_HPP

#include <QCoreApplication>

#include <QObject>
#include <QStringBuilder>

#include "addleexception.hpp"
namespace Addle {

DECL_RUNTIME_ERROR(CommandLineException)
class ADDLE_COMMON_EXPORT CommandLineException : public AddleException
{
public:
    CommandLineException(
            const QString errorText
        )
        : _errorText(errorText)
#ifdef ADDLE_DEBUG
        , AddleException(errorText)
#endif
    {
    }

    virtual ~CommandLineException() = default;

    QString errorText() { return _errorText; }

protected:
    QString _errorText;
};

DECL_RUNTIME_ERROR(CommandLineParserException)
class ADDLE_COMMON_EXPORT CommandLineParserException : public CommandLineException
{
    ADDLE_EXCEPTION_BOILERPLATE(CommandLineParserException)

public:
    CommandLineParserException(const QString errorText)
        : CommandLineException(
#ifdef ADDLE_DEBUG
            //% "The command-line parser encountered an error: \"%1\""
            qtTrId("error-messages.command-line.parser").arg(errorText)
#else
            errorText
#endif
        )
    {
    }
};

DECL_RUNTIME_ERROR(MultipleStartModesException)
class ADDLE_COMMON_EXPORT MultipleStartModesException : public CommandLineException
{
    ADDLE_EXCEPTION_BOILERPLATE(MultipleStartModesException)
public: 
    MultipleStartModesException()
        : CommandLineException(
            //% "You may not specify multiple modes to start Addle in."
            qtTrId("error-messages.command-line.multiple-modes")
        )
    {
    }
};
} // namespace Addle

#endif // COMMANDLINEEXCEPTIONS_HPP
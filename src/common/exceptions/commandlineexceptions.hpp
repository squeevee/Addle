/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

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
            qtTrId("cli-messages.parser-error").arg(errorText)
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
            qtTrId("cli-messages.multiple-modes-error")
        )
    {
    }
};

} // namespace Addle

#endif // COMMANDLINEEXCEPTIONS_HPP

#ifndef COMMANDLINEEXCEPTIONS_HPP
#define COMMANDLINEEXCEPTIONS_HPP

#include <QCoreApplication>

#include <QObject>
#include <QStringBuilder>

#include "utilities/qtextensions/translation.hpp"

#include "baseaddleexception.hpp"
namespace Addle {

DECL_RUNTIME_ERROR(CommandLineException)
class ADDLE_COMMON_EXPORT CommandLineException : public BaseAddleException
{
public:
    CommandLineException(
            const QString errorText
        )
        : _errorText(errorText)
#ifdef ADDLE_DEBUG
        , BaseAddleException(errorText)
#endif
    {
    }

    virtual ~CommandLineException() = default;

    QString getErrorText() { return _errorText; }

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
            fallback_translate(
                "CommandLineParserException",
                "what",
                QStringLiteral("The command-line parser encountered an error: \"%1\"")
                ).arg(errorText)
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
        : CommandLineException( QCoreApplication::translate(
            "MultipleStartModesException",
            "You may not specify multiple modes to start Addle in."
            )
        )
    {
    }
};
} // namespace Addle

#endif // COMMANDLINEEXCEPTIONS_HPP
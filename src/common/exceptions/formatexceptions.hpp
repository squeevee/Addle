#ifndef FORMATEXCEPTIONS_HPP
#define FORMATEXCEPTIONS_HPP

#include <QObject>
#include <QStringBuilder>

#include "baseaddleexception.hpp"

#include "utilities/importexportinfo.hpp"

#include "utilities/qt_extensions/translation.hpp"

class FormatException : public BaseAddleException
{
public:
#ifdef ADDLE_DEBUG
    FormatException(const QString& what, FormatId format)
        : BaseAddleException(what), _format(format)
    {
    }

    FormatException(const ImportExportInfo& importExportInfo, FormatId format, const QString& what)
        : BaseAddleException(what), _importExportInfo(importExportInfo), _format(format)
    {
    }

#else
    FormatException() = default;
    FormatException(const ImportExportInfo& importExportInfo, FormatId format)
        : _importExportInfo(importExportInfo), _format(format)
    {
    }
#endif
    virtual ~FormatException() = default;

    const ImportExportInfo& getimportExportInfo() const { return _importExportInfo; }
    FormatId getFormatId() const { return _format; }

private:
    const ImportExportInfo _importExportInfo;
    const FormatId _format;
};

DECL_LOGIC_ERROR(FormatModelNotSupportedException)
class FormatModelNotSupportedException : public FormatException
{
    ADDLE_EXCEPTION_BOILERPLATE(FormatModelNotSupportedException)
public:

#ifdef ADDLE_DEBUG
    FormatModelNotSupportedException (
        const ImportExportInfo& importExportInfo,
        const char* modelname
        ) 
        : FormatException(
            importExportInfo,
            FormatId(),
            fallback_translate(
                "FormatModelNotSupportedException",
                "what", 
                QStringLiteral(
                    "Importing is not supported for the given format model.\n"
                    " Model name: \"%1\""))
            .arg(modelname)
        ), _modelname(modelname)
    {
    }
#else
public:
    FormatModelNotSupportedException( const ImportExportInfo& importExportInfo )
        : FormatException(importExportInfo, FormatId())
    {
    }
#endif
public:
    virtual ~FormatModelNotSupportedException() = default;

private:
    const char* _modelname;
};

DECL_RUNTIME_ERROR(FormatNotSupportedException)
class FormatNotSupportedException : public FormatException
{
    ADDLE_EXCEPTION_BOILERPLATE(FormatNotSupportedException)
public:
#ifdef ADDLE_DEBUG
    FormatNotSupportedException(const ImportExportInfo& importExportInfo, FormatId format)
        : FormatException(
        importExportInfo,
        format,
        fallback_translate(
            "ImportNotSupportedException",
            "what",
            QStringLiteral(
                "There is no driver for the given format.\n"
                "format type: \"%1\"\n"
                "   filename: \"%2\""))
            .arg(format.getMimeType())
            .arg(importExportInfo.getFilename())
        )
    {
    }
#else
public:
    FormatNotSupportedException(const ImportExportInfo& importExportInfo, FormatId format)
        : FormatException(importExportInfo, format)
    {
    }
#endif
public:
    virtual ~FormatNotSupportedException() = default;
};

class IFormatDriver;

DECL_RUNTIME_ERROR(ImportNotSupportedException)
class ImportNotSupportedException : public FormatException
{
    ADDLE_EXCEPTION_BOILERPLATE(ImportNotSupportedException)
public:
#ifdef ADDLE_DEBUG
    ImportNotSupportedException(const ImportExportInfo& importExportInfo, FormatId format, IFormatDriver* driver = nullptr)
        : FormatException(
        importExportInfo,
        format,
        fallback_translate(
            "ImportNotSupportedException",
            "what",
            QStringLiteral(
                "The driver for the format does not support importing.\n"
                "format type: \"%1\"\n"
                "   filename: \"%2\""))
            .arg(format.getMimeType())
            .arg(importExportInfo.getFilename())
        ), _driver(driver)
    {
    }
#else
public:
    ImportNotSupportedException(const ImportExportInfo& importExportInfo, FormatId format, IFormatDriver* driver = nullptr)
        : FormatException(importExportInfo, format), _driver(driver)
    {
    }
#endif
public:
    virtual ~ImportNotSupportedException() = default;
private:
    const IFormatDriver* _driver;
};

DECL_RUNTIME_ERROR(FormatInferrenceFailedException)
class FormatInferrenceFailedException : public FormatException
{
    ADDLE_EXCEPTION_BOILERPLATE(FormatInferrenceFailedException)
public:
#ifdef ADDLE_DEBUG
    FormatInferrenceFailedException(const ImportExportInfo& importExportInfo)
        : FormatException(
        importExportInfo,
        FormatId(), 
            fallback_translate(
            "FormatInferrenceFailedException",
            "what",
            QStringLiteral(
                "A format could not be inferred from the information given.\n"
                "   filename: \"%1\""))
            .arg(importExportInfo.getFilename())
        )
    {
    }
#else
public:
    FormatInferrenceFailedException(const ImportExportInfo& importExportInfo)
        : FormatException(importExportInfo, FormatId())
    {
    }
#endif
public:
    virtual ~FormatInferrenceFailedException() = default;
};

#endif // FORMATEXCEPTIONS_HPP
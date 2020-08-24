/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef FORMATEXCEPTIONS_HPP
#define FORMATEXCEPTIONS_HPP

#include <QObject>
#include <QStringBuilder>

#include "addleexception.hpp"

#include "utilities/format/importexportinfo.hpp"
#include "utilities/format/genericformat.hpp"

namespace Addle {

class ADDLE_COMMON_EXPORT FormatException : public AddleException
{
public:
#ifdef ADDLE_DEBUG
    FormatException(const QString& what, GenericFormatId format)
        : AddleException(what), _format(format)
    {
    }

    FormatException(const GenericImportExportInfo& importExportInfo, GenericFormatId format, const QString& what)
        : AddleException(what), _importExportInfo(importExportInfo), _format(format)
    {
    }

#else
    FormatException() = default;
    FormatException(const GenericImportExportInfo& importExportInfo, GenericFormatId format)
        : _importExportInfo(importExportInfo), _format(format)
    {
    }
#endif
    virtual ~FormatException() = default;

    const GenericImportExportInfo& importExportInfo() const { return _importExportInfo; }
    GenericFormatId formatId() const { return _format; }

private:
    const GenericImportExportInfo _importExportInfo;
    const GenericFormatId _format;
};

DECL_LOGIC_ERROR(FormatModelNotSupportedException)
class ADDLE_COMMON_EXPORT FormatModelNotSupportedException : public FormatException
{
    ADDLE_EXCEPTION_BOILERPLATE(FormatModelNotSupportedException)
public:

#ifdef ADDLE_DEBUG
    FormatModelNotSupportedException (
        const GenericImportExportInfo& importExportInfo,
        const char* modelname
        ) 
        : FormatException(
            importExportInfo,
            GenericFormatId(),
            //% "Importing is not supported for the given format model.\n"
            //% " Model name: \"%1\""
            qtTrId("debug-messages.format-error.model-not-supported")
                .arg(modelname)
        ), _modelname(modelname)
    {
    }
#else
public:
    FormatModelNotSupportedException( const GenericImportExportInfo& importExportInfo )
        : FormatException(importExportInfo, GenericFormatId())
    {
    }
#endif
public:
    virtual ~FormatModelNotSupportedException() = default;

private:
    const char* _modelname;
};

DECL_RUNTIME_ERROR(FormatNotSupportedException)
class ADDLE_COMMON_EXPORT FormatNotSupportedException : public FormatException
{
    ADDLE_EXCEPTION_BOILERPLATE(FormatNotSupportedException)
public:
#ifdef ADDLE_DEBUG
    FormatNotSupportedException(const GenericImportExportInfo& importExportInfo, GenericFormatId format)
        : FormatException(
        importExportInfo,
        format,
            //% "There is no driver for the given format.\n"
            //% " format type: \"%1\"\n"
            //% "    filename: \"%2\""
            qtTrId("debug-messages.format-error.format-not-supported")
                .arg(format.mimeType())
                .arg(importExportInfo.filename())
        )
    {
    }
#else
public:
    FormatNotSupportedException(const GenericImportExportInfo& importExportInfo, GenericFormatId format)
        : FormatException(importExportInfo, format)
    {
    }
#endif
public:
    virtual ~FormatNotSupportedException() = default;
};

// template<class ModelType>
// class IFormatDriver;

//DECL_RUNTIME_ERROR(ImportNotSupportedException)
// class ADDLE_COMMON_EXPORT ImportNotSupportedException : public FormatException
// {
//     ADDLE_EXCEPTION_BOILERPLATE(ImportNotSupportedException)
// public:
// #ifdef ADDLE_DEBUG
//     ImportNotSupportedException(const GenericImportExportInfo& importExportInfo, GenericFormatId format, IFormatDriver* driver = nullptr)
//         : FormatException(
//         importExportInfo,
//         format,
//         //% "The driver for the format does not support importing.\n"
//         //% "format type: \"%1\"\n"
//         //% "   filename: \"%2\""
//         qtTrId("debug-messages.format-error.import-not-supported")
//             .arg(format.key())
//             .arg(importExportInfo.filename())
//         ), _driver(driver)
//     {
//     }
// #else
// public:
//     ImportNotSupportedException(const GenericImportExportInfo& importExportInfo, GenericFormatId format, IFormatDriver* driver = nullptr)
//         : FormatException(importExportInfo, format), _driver(driver)
//     {
//     }
// #endif
// public:
//     virtual ~ImportNotSupportedException() = default;
// private:
//     const IFormatDriver* _driver;
// };

// DECL_RUNTIME_ERROR(FormatInferrenceFailedException)
// class ADDLE_COMMON_EXPORT FormatInferrenceFailedException : public FormatException
// {
//     ADDLE_EXCEPTION_BOILERPLATE(FormatInferrenceFailedException)
// public:
// #ifdef ADDLE_DEBUG
//     FormatInferrenceFailedException(const GenericImportExportInfo& importExportInfo)
//         : FormatException(
//         importExportInfo,
//         GenericFormatId(), 
//             //% "A format could not be inferred from the information given.\n"
//             //% "   filename: \"%1\""
//             qtTrId("debug-messages.format-error.inferrence-failed")
//                 .arg(importExportInfo.filename())
//         )
//     {
//     }
// #else
// public:
//     FormatInferrenceFailedException(const GenericImportExportInfo& importExportInfo)
//         : FormatException(importExportInfo, GenericFormatId())
//     {
//     }
// #endif
// public:
//     virtual ~FormatInferrenceFailedException() = default;
// };

} // namespace Addle

#endif // FORMATEXCEPTIONS_HPP
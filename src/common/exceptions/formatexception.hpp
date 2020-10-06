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
#include <QMetaEnum>

#include "addleexception.hpp"

#include "utilities/format/importexportinfo.hpp"

namespace Addle {

class ADDLE_COMMON_EXPORT FormatException : public AddleException
{
    Q_GADGET
    ADDLE_EXCEPTION_BOILERPLATE(FormatException)
public:
    enum Why
    {
        FormatNotRecognized,
        EmptyResource,
        WrongModelType
    };
    Q_ENUM(Why)

    FormatException(Why why, GenericFormatId format, const ImportExportInfo& importExportInfo = ImportExportInfo())
        : AddleException(
#ifdef ADDLE_DEBUG
        //% "A format exception occurred:\n"
        //% "       why: %1\n"
        //% "    format: %2\n"
        //% " file path: %3"
        qtTrId("debug-messages.format-exception")
            .arg(QMetaEnum::fromType<Why>().valueToKey(why))
            .arg(format.key())
            .arg(importExportInfo.filename())
#endif
        ),
        _why(why),
        _importExportInfo(importExportInfo),
        _format(format)
    {
    }
    virtual ~FormatException() = default;

    Why why() const { return _why; }

    const ImportExportInfo& importExportInfo() const { return _importExportInfo; }
    GenericFormatId formatId() const { return _format; }

private:
    const ImportExportInfo _importExportInfo;
    const GenericFormatId _format;
    const Why _why;
};


} // namespace Addle

#endif // FORMATEXCEPTIONS_HPP

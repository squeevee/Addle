/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IFORMATDRIVER_HPP
#define IFORMATDRIVER_HPP

#include <QString>
#include <QList>
#include <QByteArray>
#include <QIODevice>
#include <type_traits>

#include "interfaces/traits.hpp"

#include "utilities/model/importexportinfo.hpp"

#include "idtypes/formatid.hpp"

#include "iformatmodel.hpp"
namespace Addle {

class IFormatDriver
{
public:
    virtual ~IFormatDriver() = default;

    //virtual QString name() = 0;

    virtual bool supportsImport() const = 0;
    virtual bool supportsExport() const = 0;

    virtual FormatId id() const = 0;

    virtual IFormatModel* importModel(QIODevice& device, ImportExportInfo info) = 0;
    virtual void exportModel(IFormatModel* model, QIODevice& device, ImportExportInfo info) = 0;
};

DECL_PERSISTENT_OBJECT_TYPE(IFormatDriver, FormatId);

} // namespace Addle
#endif // IFORMATDRIVER_HPP
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

#include "utilities/format/importexportinfo.hpp"

#include "idtypes/formatid.hpp"

namespace Addle {

template<class ModelType>
class IFormatDriver
{
public:
    virtual ~IFormatDriver() = default;

    //virtual QString name() = 0;

    virtual bool supportsImport() const = 0;
    virtual bool supportsExport() const = 0;

    virtual FormatId<ModelType> id() const = 0;

    virtual ModelType* importModel(QIODevice& device, const ImportExportInfo& info) = 0;
    virtual void exportModel(ModelType* model, QIODevice& device, const ImportExportInfo& info) = 0;
};

namespace aux_IDocumentFormatDriver { using namespace config_detail::generic_id_parameter; }
using IDocumentFormatDriver = IFormatDriver<IDocument>;
ADDLE_DECL_SINGLETON_REPO_MEMBER(IDocumentFormatDriver,  DocumentFormatId)

namespace aux_IBrushFormatDriver { using namespace config_detail::generic_id_parameter; }
using IBrushFormatDriver = IFormatDriver<IBrush>;
ADDLE_DECL_SINGLETON_REPO_MEMBER(IBrushFormatDriver,     BrushFormatId)

namespace aux_IPaletteFormatDriver { using namespace config_detail::generic_id_parameter; }
using IPaletteFormatDriver = IFormatDriver<IPalette>;
ADDLE_DECL_SINGLETON_REPO_MEMBER(IPaletteFormatDriver,   PaletteFormatId)

} // namespace Addle
#endif // IFORMATDRIVER_HPP

/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef FILESERVICE_HPP
#define FILESERVICE_HPP

#include <QHash>
#include <QList>
#include <QSet>

#include "compat.hpp"
#include "utils.hpp"
#include "interfaces/format/iformatdriver.hpp"
#include "interfaces/services/iformatservice.hpp"

#include "idtypes/formatid.hpp"
namespace Addle {


class ADDLE_CORE_EXPORT FormatService : public virtual IFormatService
{
public:
    FormatService();
    virtual ~FormatService() = default;

protected:
    GenericFormatModel importModel_p(QIODevice& device, const GenericImportExportInfo& info);

private:
    template <class ModelType>
    void setupFormat();

    QHash<QString, GenericFormatId> _formats_byExtension;
    QHash<QString, GenericFormatId> _formats_byMimeType;
    QHash<QByteArray, GenericFormatId> _formats_bySignature;
    QHash<std::type_index, QSet<GenericFormatId>> _formats_byModelType;
    QHash<GenericFormatId, GenericFormatDriver> _drivers_byFormat;

    GenericFormatId inferFormatFromSignature(QIODevice& device);

    int _maxSignatureLength = 0;
};

} // namespace Addle
#endif // FILESERVICE_HPP
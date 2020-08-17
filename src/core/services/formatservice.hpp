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
    //IDocument* loadFile(QString filename);
    IFormatModel* importModel_p(const std::type_info& modelType, QIODevice& device, const ImportExportInfo& info);

private:

    QHash<QString, FormatId> _formats_byExtension;
    QHash<QString, FormatId> _formats_byMimeType;
    QHash<QByteArray, FormatId> _formats_bySignature;
    QHash<std::type_index, QSet<FormatId>> _formats_byModelType;

    QHash<FormatId, IFormatDriver*> _drivers_byFormat;

    FormatId inferFormatFromSignature(QIODevice& device);

    int _maxSignatureLength = 0;
};

} // namespace Addle
#endif // FILESERVICE_HPP
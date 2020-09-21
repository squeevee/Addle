/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef QTIMAGEFORMATDRIVER_HPP
#define QTIMAGEFORMATDRIVER_HPP

#include "compat.hpp"

#include <QByteArray>

#include "interfaces/models/idocument.hpp"
#include "interfaces/format/iformatdriver.hpp"
#include "globals.hpp"

namespace Addle {

// An image format driver that uses Qt's image processing functionality as its
// backend
class ADDLE_CORE_EXPORT QtImageFormatDriver : public IFormatDriver<IDocument>
{
public:
    QtImageFormatDriver(DocumentFormatId id);
    virtual ~QtImageFormatDriver() = default;

    bool supportsImport() const { return _supportsImport; }
    bool supportsExport() const { return _supportsExport; }

    DocumentFormatId id() const { return _id; }

    IDocument* importModel(QIODevice& device, DocumentImportExportInfo info);
    void exportModel(IDocument* model, QIODevice& device, DocumentImportExportInfo info);

    static bool idIsSupported(DocumentFormatId id)
    { 
        return supportsImport_p(id) || supportsExport_p(id);
    }
    
private:    
    const DocumentFormatId _id;
    
    QByteArray _name;
    bool _supportsImport;
    bool _supportsExport;
    
    static bool supportsImport_p(DocumentFormatId id);
    static bool supportsExport_p(DocumentFormatId id);
};

} // namespace Addle
#endif // QTIMAGEFORMATDRIVER_HPP

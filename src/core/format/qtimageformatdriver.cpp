/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "qtimageformatdriver.hpp"

#include "interfaces/models/idocument.hpp"
#include "servicelocator.hpp"

#include <QImageReader>
#include <QImageWriter>
#include <QImage>
#include <QString>

using namespace Addle;

QtImageFormatDriver::QtImageFormatDriver(DocumentFormatId id)
    : _id(id)
{
    auto mimeType = _id.mimeType().toUtf8();
    _supportsImport = supportsImport_p(_id);
    _supportsExport = supportsExport_p(_id);
    
    ADDLE_ASSERT(_supportsImport);
    
    _name = QImageReader::imageFormatsForMimeType(mimeType).constFirst();
}

IDocument* QtImageFormatDriver::importModel(QIODevice& device, const ImportExportInfo& info)
{
    QImage image;
    image.load(&device, _name);

    // if (status)
    //     status->setProgress(1.00);

//     return ServiceLocator::make<IDocument>(
//         DocumentBuilder()
//             .setUrl(QUrl::fromLocalFile(info.filename()))
//             .addLayer(
//                 LayerNodeBuilder()
//                     .setImage(image)
//                     .setBoundary(image.rect())
//             )
//     );
}


void QtImageFormatDriver::exportModel(IDocument* model, QIODevice& device, const ImportExportInfo& info)
{

}

bool QtImageFormatDriver::supportsImport_p(DocumentFormatId id)
{
    auto mimeType = id.mimeType().toUtf8();
    return QImageReader::supportedMimeTypes().contains(mimeType);
}

bool QtImageFormatDriver::supportsExport_p(DocumentFormatId id)
{
    auto mimeType = id.mimeType().toUtf8();
    return QImageWriter::supportedMimeTypes().contains(mimeType);
}

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

#include <QImage>
#include <QString>

using namespace Addle;

IDocument* QtImageFormatDriver::importModel(QIODevice& device, DocumentImportExportInfo info)
{
    DocumentBuilder documentBuilder;
    documentBuilder.setFilename(info.filename());

    QImage image;
    image.load(&device, _name);

    LayerBuilder layerBuilder;
    layerBuilder.setImage(image);
    layerBuilder.setBoundary(image.rect());
    documentBuilder.addLayer(layerBuilder);

    // if (status)
    //     status->setProgress(1.00);

    return ServiceLocator::make<IDocument>(documentBuilder);
}


void QtImageFormatDriver::exportModel(IDocument* model, QIODevice& device, DocumentImportExportInfo info)
{

}
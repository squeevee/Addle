#include "qtimageformatdriver.hpp"

#include "interfaces/models/idocument.hpp"
#include "servicelocator.hpp"

#include <QImage>
#include <QString>

using namespace Addle;

IFormatModel* QtImageFormatDriver::importModel(QIODevice& device, ImportExportInfo info)
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


void QtImageFormatDriver::exportModel(IFormatModel* model, QIODevice& device, ImportExportInfo info)
{

}
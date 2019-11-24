#include "qtimageformatdriver.hpp"

#include "common/interfaces/models/idocument.hpp"
#include "common/servicelocator.hpp"

#include <QImage>
#include <QString>

IFormatModel* QtImageFormatDriver::importModel(QIODevice& device, ImportExportInfo info, ITaskStatusController* status)
{
    DocumentBuilder documentBuilder;
    documentBuilder.setFilename(info.getFilename());

    QImage image;
    image.load(&device, _qtFormatName);

    LayerBuilder layerBuilder;
    layerBuilder.setImage(image);
    layerBuilder.setBoundary(image.rect());
    documentBuilder.addLayer(layerBuilder);

    if (status)
        status->setProgress(1.00);

    return ServiceLocator::make<IDocument>(documentBuilder);
}


void QtImageFormatDriver::exportModel(IFormatModel* model, QIODevice& device, ImportExportInfo info, ITaskStatusController* status)
{

}
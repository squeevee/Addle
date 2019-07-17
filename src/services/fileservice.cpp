#include "fileservice.h"

#include <QSharedPointer>

#include "servicelocator.h"

IDocument* FileService::loadImage(QString filename)
{
    QImage image(filename);

    if (image.isNull())
    {
        return nullptr;
    }

    DocumentBuilder documentBuilder;
    documentBuilder.filename = filename;
    
    LayerBuilder layerBuilder;
    layerBuilder.image = image;
    layerBuilder.boundary = image.rect();

    documentBuilder.layers.append(layerBuilder);

    return ServiceLocator::make<IDocument>(documentBuilder);
}
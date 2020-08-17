#include "servicelocator.hpp"

#include "document.hpp"
#include <QImage>
#include <QPainter>
using namespace Addle;

void Document::initialize(DocumentBuilder& builder)
{
    const Initializer init(_initHelper);

    _filename = builder.filename();
    _backgroundColor = builder.backgroundColor();

    for (LayerBuilder& layerBuilder : builder.layers())
    {
        ILayer* layer = ServiceLocator::make<ILayer>(layerBuilder);
        _layers.append(QSharedPointer<ILayer>(layer));
    }

    _size = unitedBoundary().size();
}

void Document::initialize()
{
    _initHelper.initializeBegin(); //blank
    _initHelper.initializeEnd();
}

void Document::render(QRect area, QPaintDevice* device) const
{
    _initHelper.check();

}

void Document::layersChanged(QList<ILayer*> layers)
{
    QRect changed = unitedBoundary();
    if (!changed.isNull())
    {
        //emit renderChanged(changed);
    }
}

QRect Document::unitedBoundary()
{
    QRect result;

    for (QSharedPointer<ILayer> layer : _layers)
    {
        QRect bound = layer->boundary();
        if (!bound.isNull())
        {
            if (result.isNull())
            {
                result = bound;
            }
            else
            {
                result = result.united(bound);
            }
        }
    }

    return result;
}

QImage Document::exportImage()
{
    _initHelper.check();
    QImage result(size(), QImage::Format::Format_ARGB32);
    
    render(QRect(QPoint(), size()), &result);

    return result;
}
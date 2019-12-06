#include "servicelocator.hpp"

#include "document.hpp"
#include <QImage>
#include <QPainter>

void Document::initialize(DocumentBuilder& builder)
{
    _initHelper.initializeBegin();

    _filename = builder.getFilename();

    for (LayerBuilder& layerBuilder : builder.getLayers())
    {
        ILayer* layer = ServiceLocator::make<ILayer>(layerBuilder);
        _layers.append(layer);
    }

    updateGeometry();
    _initHelper.initializeEnd();
}

void Document::initialize(InitEmptyOptions emptyOption)
{
    _initHelper.initializeBegin();
    
    if (emptyOption == InitEmptyOptions::useDefaults)
    {
        ILayer* layer = ServiceLocator::make<ILayer>();
        _layers.append(layer);

        updateGeometry();
    }
    _initHelper.initializeEnd();
}

/*void Document::applyOperation(IDrawingOperation& operation)
{
    
}*/

void Document::render(QRect area, QPaintDevice* device)
{
    _initHelper.assertInitialized();

}

void Document::addNewLayer(LayerBuilder& builder, int insertBefore)
{
    addNewLayers({ builder }, insertBefore);
}

void Document::addNewLayers(QList<LayerBuilder> builders, int insertBefore)
{
    _initHelper.assertInitialized();

    emit layersAdding(builders.count());

}

void Document::addNewEmptyLayer(int insertBefore)
{
    addNewEmptyLayers(1, insertBefore);
}

void Document::addNewEmptyLayers(int count, int insertBefore)
{
    _initHelper.assertInitialized();
    emit layersAdding(count);

    QList<ILayer*> newLayers;
    int startIndex = insertBefore;

    for (int i = 0; i < count; i++)
    {
        ILayer* layer = ServiceLocator::make<ILayer>();
        newLayers.append(layer);
    }

    if (insertBefore == -1)
    {
        startIndex = _layers.count();
        _layers.append(newLayers);
    }
    else
    {
        int insertAt = insertBefore - 1;
        for (ILayer* layer : newLayers)
        {
            _layers.insert(insertAt, layer);
            insertAt++;
        }
    }
    
    layersChanged(newLayers);

    emit layersAdded(startIndex, count);
}

void Document::deleteLayer(int index)
{
    deleteLayers({index});
}

void Document::deleteLayers(QList<int> indices)
{
    _initHelper.assertInitialized();

    emit layersDeleting(indices);

    QList<ILayer*> layersToDelete;

    for (int index : indices)
    {
        ILayer* layer = _layers.at(index);
        layersToDelete.append(layer);
        _layers.removeAt(index);
    }

    layersChanged(layersToDelete);
    int count = layersToDelete.count();

    for (ILayer* layer : layersToDelete)
    {
        delete layer;
    }

    emit layersDeleted(count);
}

void Document::reorderLayer(int from, int to)
{
    _initHelper.assertInitialized();

    emit layerReordering(from, to);

    _layers.move(from, to);

    emit layerReordered(from, to);
}


QList<ILayer*> Document::getLayers()
{
    _initHelper.assertInitialized();
    return _layers;
}

ILayer* Document::getLayer(int index)
{
    _initHelper.assertInitialized();
    return _layers.at(index);
}

int Document::indexOfLayer(ILayer* layer)
{
    _initHelper.assertInitialized();
    return _layers.indexOf(layer);
}

int Document::layerCount()
{
    _initHelper.assertInitialized();
    return _layers.count();
}

void Document::layersChanged(QList<ILayer*> layers)
{
    updateGeometry();

    QRect changed = unitedBoundary(layers);
    if (!changed.isNull())
    {
        emit renderChanged(changed);
    }
}

void Document::updateGeometry()
{
    _empty = true;
    for (ILayer* layer : _layers)
    {
        if (!layer->isEmpty())
        {
            _empty = false;
            break;
        }
    }

    QRect newBoundary = unitedBoundary(_layers);

    if (newBoundary.isNull())
    {
        return;
    }

    QPoint newTopLeft = newBoundary.topLeft();
    QSize newSize = newBoundary.size();
    bool sizeChanging = newSize != _size;

    QRect boundaryChange(newTopLeft, newSize);
    if (!newTopLeft.isNull() || sizeChanging)
    {
        emit boundaryChanging(boundaryChange);
    }

    _size = newSize;

    if (!newTopLeft.isNull())
    {
        for (ILayer* layer : _layers)
        {
            if (layer->isEmpty())
            {
                continue;
            }

            QPoint layerTL = layer->getTopLeft();
            layer->setTopLeft(layerTL - newTopLeft);
        }
    }

    if (!newTopLeft.isNull() || sizeChanging)
    {
        emit boundaryChanged(boundaryChange);
    }
}

QRect Document::unitedBoundary(QList<ILayer*> layers)
{
    QRect result;

    for (ILayer* layer : layers)
    {
        if (layer->isEmpty())
        {
            continue;
        }

        QRect bound = layer->getBoundary();
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
    _initHelper.assertInitialized();
    QImage result(getSize(), QImage::Format::Format_ARGB32);
    
    render(QRect(QPoint(), getSize()), &result);

    return result;
}
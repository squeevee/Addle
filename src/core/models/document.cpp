#include "servicelocator.hpp"

#include "document.hpp"
#include <QImage>
#include <QPainter>
using namespace Addle;

void Document::initialize(DocumentBuilder& builder)
{
    _initHelper.initializeBegin();

    _filename = builder.filename();
    _backgroundColor = builder.backgroundColor();

    for (LayerBuilder& layerBuilder : builder.layers())
    {
        ILayer* layer = ServiceLocator::make<ILayer>(layerBuilder);
        _layers.append(QSharedPointer<ILayer>(layer));
    }

    _size = unitedBoundary().size();
    //updateGeometry();
    _initHelper.initializeEnd();
}

void Document::initialize()
{
    _initHelper.initializeBegin(); //blank
    _initHelper.initializeEnd();
}

void Document::render(QRect area, QPaintDevice* device)
{
    _initHelper.check();

}

void Document::addNewLayer(LayerBuilder& builder, int insertBefore)
{
    addNewLayers({ builder }, insertBefore);
}

void Document::addNewLayers(QList<LayerBuilder> builders, int insertBefore)
{
    _initHelper.check();

    emit layersAdding(builders.count());

}

// void Document::addNewEmptyLayer(int insertBefore)
// {
//     addNewEmptyLayers(1, insertBefore);
// }

// void Document::addNewEmptyLayers(int count, int insertBefore)
// {
//     _initHelper.check();
//     emit layersAdding(count);

//     QList<ILayer*> newLayers;
//     int startIndex = insertBefore;

//     for (int i = 0; i < count; i++)
//     {
//         ILayer* layer = ServiceLocator::make<ILayer>();
//         newLayers.append(layer);
//     }

//     if (insertBefore == -1)
//     {
//         startIndex = _layers.count();
//         _layers.append(newLayers);
//     }
//     else
//     {
//         int insertAt = insertBefore - 1;
//         for (ILayer* layer : newLayers)
//         {
//             _layers.insert(insertAt, layer);
//             insertAt++;
//         }
//     }
    
//     layersChanged(newLayers);

//     emit layersAdded(startIndex, count);
// }

// void Document::deleteLayer(int index)
// {
//     deleteLayers({index});
// }

// void Document::deleteLayers(QList<int> indices)
// {
//     _initHelper.check();

//     emit layersDeleting(indices);

//     QList<ILayer*> layersToDelete;

//     for (int index : indices)
//     {
//         ILayer* layer = _layers.at(index);
//         layersToDelete.append(layer);
//         _layers.removeAt(index);
//     }

//     layersChanged(layersToDelete);
//     int count = layersToDelete.count();

//     for (ILayer* layer : layersToDelete)
//     {
//         delete layer;
//     }

//     emit layersDeleted(count);
// }

// void Document::reorderLayer(int from, int to)
// {
//     _initHelper.check();

//     emit layerReordering(from, to);

//     _layers.move(from, to);

//     emit layerReordered(from, to);
// }


QList<QSharedPointer<ILayer>> Document::layers()
{
    _initHelper.check();
    return _layers;
}

// ILayer* Document::layer(int index)
// {
//     _initHelper.check();
//     return _layers.at(index);
// }

// int Document::indexOfLayer(ILayer* layer)
// {
//     _initHelper.check();
//     return _layers.indexOf(layer);
// }

int Document::layerCount()
{
    _initHelper.check();
    return _layers.count();
}

void Document::layersChanged(QList<ILayer*> layers)
{
    updateGeometry();

    QRect changed = unitedBoundary();
    if (!changed.isNull())
    {
        //emit renderChanged(changed);
    }
}

void Document::updateGeometry()
{
    //I'm honestly not sure what this function is trying to do. -E 3/2020

    // maybe concern over the document being "empty" should be relegated to the
    // presenter
    // _empty = true;
    // for (QSharedPointer<ILayer> layer : _layers)
    // {
    //     if (!layer->isEmpty())
    //     {
    //         _empty = false;
    //         break;
    //     }
    // }

    // QRect newBoundary = unitedBoundary();

    // if (newBoundary.isNull())
    // {
    //     return;
    // }

    // QPoint newTopLeft = newBoundary.topLeft();
    // QSize newSize = newBoundary.size();
    // bool sizeChanging = newSize != _size;

    // QRect boundaryChange(newTopLeft, newSize);
    // if (!newTopLeft.isNull() || sizeChanging)
    // {
    //     emit boundaryChanging(boundaryChange);
    // }

    // _size = newSize;

    // if (!newTopLeft.isNull())
    // {
    //     for (QSharedPointer<ILayer> layer : _layers)
    //     {
    //         if (layer->isEmpty())
    //         {
    //             continue;
    //         }

    //         QPoint layerTL = layer->topLeft();
    //         layer->setTopLeft(layerTL - newTopLeft);
    //     }
    // }

    // if (!newTopLeft.isNull() || sizeChanging)
    // {
    //     emit boundaryChanged(boundaryChange);
    // }
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
/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "servicelocator.hpp"

#include "document.hpp"
#include <QImage>
#include <QPainter>
#include <QStack>

using namespace Addle;

Document::Document(
        const DocumentBuilder& builder, 
        const IFactory<ILayer>& layerFactory,
        const IFactory<ILayerGroup>& layerGroupFactory
    )
    : _backgroundColor(builder.backgroundColor()),
    _url(builder.url()),
    _layerFactory(layerFactory), 
    _layerGroupFactory(layerGroupFactory)
{
    using LayerNode = LayersTree::Node;
   
    // TODO DataTree echo
//     for (const LayerNodeBuilder& layerNodeBuilder : builder.layerNodes())
//     {
//         if (layerNodeBuilder.isLayer())
//         {
//             LayerNode& node = _layers.root().addLeaf();
//             auto layer = _layerFactory.makeShared(*this, node, layerNodeBuilder);
//             node.setLeafValue(layer);
//         }
//         else
//         {
//             LayerNode& node = _layers.root().addBranch();
//             auto layerGroup = _layerGroupFactory.makeShared(*this, node, layerNodeBuilder);
//             node.setBranchValue(layerGroup);
//         }
//     }
    
// 
//     _size = unitedBoundary().size();
}

// void Document::render(QRect area, QPaintDevice* device) const
// {
// }

// void Document::layersChanged(QList<ILayer*> layers)
// {
//     QRect changed = unitedBoundary();
//     if (!changed.isNull())
//     {
//         //emit renderChanged(changed);
//     }
// }
// 
// QRect Document::unitedBoundary()
// {
//     QRect result;
// 
//     for (QSharedPointer<ILayer> layer : _layers)
//     {
//         QRect bound = layer->boundary();
//         if (!bound.isNull())
//         {
//             if (result.isNull())
//             {
//                 result = bound;
//             }
//             else
//             {
//                 result = result.united(bound);
//             }
//         }
//     }
// 
//     return result;
// }

// QImage Document::exportImage()
// {
//     ASSERT_INIT();
//     QImage result(size(), QImage::Format::Format_ARGB32);
//     
//     render(QRect(QPoint(), size()), &result);
// 
//     return result;
// }

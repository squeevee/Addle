/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "servicelocator.hpp"

#include "utilities/datatree/nestedobjectadapter.hpp"
#include "utilities/datatree/echo.hpp"

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
    
    aux_datatree::echo_tree(
            aux_datatree::make_root_range_nested_object_adapter(
                builder.layerNodes(),
                &LayerNodeBuilder::childLayerNodes
            ),
            _layers,
            aux_datatree::echo_default_node_value_tag {},
            [&] (auto adapterNodeHandle, LayerNode* node) {
                if (aux_datatree::node_is_root(adapterNodeHandle))
                    node->setValue(_layerGroupFactory.makeShared(*this, *node));
                else if((*adapterNodeHandle).value().isGroup())
                    node->setValue(_layerGroupFactory.makeShared(*this, *node, (*adapterNodeHandle).value()));
                else
                    node->setValue(_layerFactory.makeShared(*this, *node, (*adapterNodeHandle).value()));
            }
        );
    
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

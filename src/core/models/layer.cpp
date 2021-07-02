/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "layer.hpp"
#include "interfaces/editing/irastersurface.hpp"

#include "utilities/qobject.hpp"

#include "servicelocator.hpp"

#include <QPainter>
using namespace Addle;

Layer::Layer(
        IDocument& document,  
        LayerNodeRef layerNode,
        const LayerNodeBuilder& builder
    )
    : _document(document),
    _layerNode(std::move(layerNode)),
    _boundary(builder.boundary()),
    _name(builder.name()),
    _skirtColor(builder.skirtColor()),
    _compositionMode(builder.compositionMode()),
    _opacity(builder.opacity())
{   
    // TODO: validate naive builder values
}

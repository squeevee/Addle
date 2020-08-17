#include "layer.hpp"
#include "interfaces/editing/irastersurface.hpp"

#include "utilities/qtextensions/qobject.hpp"

#include "servicelocator.hpp"

#include <QPainter>
using namespace Addle;

void Layer::initialize()
{
    const Initializer init(_initHelper);
    LayerBuilder builder;
    initialize(builder);
}

void Layer::initialize(LayerBuilder& builder)
{
    const Initializer init(_initHelper);

    _boundary = builder.boundary();
    _empty = true;
    
    _rasterSurface = ServiceLocator::makeShared<IRasterSurface>(builder.image());
}
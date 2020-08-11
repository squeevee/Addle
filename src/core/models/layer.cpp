#include "layer.hpp"
#include "interfaces/editing/irastersurface.hpp"

#include "utilities/qtextensions/qobject.hpp"

#include "servicelocator.hpp"

#include <QPainter>
using namespace Addle;

void Layer::initialize()
{
    _initHelper.initializeBegin();
    LayerBuilder builder;
    initialize(builder);
    _initHelper.initializeEnd();
}

void Layer::initialize(LayerBuilder& builder)
{
    _initHelper.initializeBegin();

    _boundary = builder.boundary();
    _empty = true;
    
    _rasterSurface = ServiceLocator::makeShared<IRasterSurface>(builder.image());
    //qobject_interface_cast(_rasterSurface.data())->setParent(this);

    _initHelper.initializeEnd();
}
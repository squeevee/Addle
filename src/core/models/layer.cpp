#include "layer.hpp"
#include "interfaces/editing/operations/irasteroperation.hpp"
#include "interfaces/editing/surfaces/irastersurface.hpp"

#include "utilities/qtextensions/qobject.hpp"

#include "servicelocator.hpp"

#include <QPainter>

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

    _boundary = builder.getBoundary();
    _empty = true;
    
    _rasterSurface = ServiceLocator::makeShared<IRasterSurface>(builder.getImage());
    //qobject_interface_cast(_rasterSurface.data())->setParent(this);

    _initHelper.initializeEnd();
}
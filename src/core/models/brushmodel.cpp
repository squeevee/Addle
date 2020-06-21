#include "brushmodel.hpp"
#include <QtDebug>

void BrushModel::initialize(const BrushBuilder& builder)
{
    _initHelper.initializeBegin();

    _id = builder.id();
    _engineId = builder.engine();
    _customEngineParameters = builder.engineParameters();

    _icon = builder.icon();

    _info = BrushInfo::fromDefaults();

    _initHelper.initializeEnd();
}
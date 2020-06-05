#include "brushpresenter.hpp"
#include "servicelocator.hpp"

#include "interfaces/models/ibrushmodel.hpp"

void BrushPresenter::initialize(IBrushModel& model)
{
    _initHelper.initializeBegin();

    _model = &model;
    _id = _model->id();

    _initHelper.initializeEnd();
}

void BrushPresenter::initialize(BrushId id)
{
    _initHelper.initializeBegin();

    _id = id;
    _model = &ServiceLocator::get<IBrushModel>(id);

    _initHelper.initializeEnd();
}

#include "interfaces/services/ibrushrepository.hpp"
#include "brushpresenter.hpp"
#include "servicelocator.hpp"

void BrushPresenter::initialize(BrushId id)
{
    _initHelper.initializeBegin();

    initialize(ServiceLocator::get<IBrushRepository>().get(id));

    _initHelper.initializeEnd();
}

void BrushPresenter::initialize(QSharedPointer<IBrush> model)
{
    _initHelper.initializeBegin();

    _model = model;

    _initHelper.initializeEnd();
}


#include "brushpresenter.hpp"
#include "servicelocator.hpp"

void BrushPresenter::initialize(BrushId id)
{
    _initHelper.initializeBegin();

    _id = id;

    _initHelper.initializeEnd();
}

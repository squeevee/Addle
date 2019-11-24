#include "toolpresenterbase.hpp"

void ToolPresenterBase::initialize_p(IHaveToolsPresenter* owner)
{
    _initHelper.initializeBegin();

    _owner = owner;

    _initHelper.initializeEnd();
}
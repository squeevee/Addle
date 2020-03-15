#include "canvaspresenter.hpp"

void CanvasPresenter::initialize(IMainEditorPresenter* mainEditorPresenter)
{
    _initHelper.initializeBegin();

    _mainEditorPresenter = mainEditorPresenter;

    _initHelper.initializeEnd();
}
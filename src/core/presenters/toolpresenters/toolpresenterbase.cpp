#include "toolpresenterbase.hpp"

#include "idtypes/brushid.hpp"

void ToolPresenterBase::initialize_p(IMainEditorPresenter* mainEditorPresenter)
{
    _initHelper.initializeBegin();

    _mainEditorPresenter = mainEditorPresenter;
    _viewPortPresenter = _mainEditorPresenter->getViewPortPresenter();

    _initHelper.initializeEnd();
}

void ToolPresenterBase::select()
{
    _mainEditorPresenter->selectTool(getId());
}

QCursor ToolPresenterBase::getCursorHint()
{

}


#include "toolpresenterbase.hpp"

#include "idtypes/brushid.hpp"

void ToolPresenterBase::initialize_p(IDocumentPresenter* documentPresenter)
{
    _initHelper.initializeBegin();

    _documentPresenter = documentPresenter;
    _viewPortPresenter = _documentPresenter->getViewPortPresenter();

    _initHelper.initializeEnd();
}

void ToolPresenterBase::select()
{
    _documentPresenter->setCurrentTool(getId());
}

QCursor ToolPresenterBase::getCursorHint()
{

}


#include "navigatetoolpresenter.hpp"

void NavigateToolPresenter::initialize(IDocumentPresenter* owner)
{
    _initHelper.initializeBegin();

    _documentPresenter = owner;
    _viewPortPresenter = _documentPresenter->getViewPortPresenter();
    ToolPresenterBase::initialize_p(owner);

    _initHelper.initializeEnd();
}

void NavigateToolPresenter::pointerEngage(QPoint pos)
{
    _initHelper.assertInitialized();

    if (!_dragHelper.isEngaged())
    {
        _anchor = _viewPortPresenter->getPosition();
        _dragHelper.engage(pos);
    }
}
void NavigateToolPresenter::pointerMove(QPoint pos)
{
    _initHelper.assertInitialized();
    if (_dragHelper.isEngaged())
    {
        _dragHelper.move(pos);
        QTransform ontoCanvas = _viewPortPresenter->getOntoCanvasTransform();
        QPointF diff = ontoCanvas.map(QPointF(_dragHelper.getFirstPosition()))
             - ontoCanvas.map(QPointF(_dragHelper.getLastPosition()));
        _viewPortPresenter->setPosition(_anchor + diff);
    }
}
void NavigateToolPresenter::pointerDisengage(QPoint pos)
{
    _initHelper.assertInitialized();
    _dragHelper.disengage(pos);
}
#include "brushoperationpresenter.hpp"

void BrushOperationPresenter::addPainterData(BrushPainterData data)
{

}

void BrushOperationPresenter::finalize()
{
    _isFinalized = false;
    emit isFinalizedChanged(_isFinalized);
}

QPainterPath BrushOperationPresenter::getMask()
{
    return QPainterPath();
}

void BrushOperationPresenter::render(QPainter& painter, QRect rect)
{

}

void BrushOperationPresenter::do_()
{

}

void BrushOperationPresenter::undo()
{

}
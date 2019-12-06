#include "toolpresenterbase.hpp"

#include "data/brushtipid.hpp"

uint qHash( const QVariant& var)
{
    if (var.canConvert<BrushTipId>())
    {
        return qHash(var.value<BrushTipId>());
    }
    else if (var.canConvert<int>())
    {
        return qHash(var.toInt());
    }
    // invalid?
}

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
    if (_engagedCursor != QCursor() && _toolPathHelper.isEngaged())
    {
        return _engagedCursor;
    } 
    else if (_defaultCursor != QCursor())
    {
        return _defaultCursor;
    }
    else
    {
        return QCursor(Qt::CursorShape::ArrowCursor);
    }
}

void ToolPresenterBase::pointerEngage(QPointF canvasPos)
{
    _initHelper.assertInitialized();

    if (!_toolPathHelper.isEngaged())
    {
        if (_engagedCursor != QCursor())
        {
            emit cursorHintChanging();
        }

        QPoint viewPortPos = _viewPortPresenter->getFromCanvasTransform().map(canvasPos).toPoint();
        _toolPathHelper.engage(viewPortPos, canvasPos);
        onPointerEngage();

        if (_engagedCursor != QCursor())
        {
            emit cursorHintChanged();
        }
    }
}
void ToolPresenterBase::pointerMove(QPointF canvasPos)
{
    _initHelper.assertInitialized();
    if (_toolPathHelper.isEngaged())
    {
        QPoint viewPortPos = _viewPortPresenter->getFromCanvasTransform().map(canvasPos).toPoint();
        _toolPathHelper.move(viewPortPos, canvasPos);
        onPointerMove();
    }
}
void ToolPresenterBase::pointerDisengage(QPointF canvasPos)
{
    _initHelper.assertInitialized();
    if (_toolPathHelper.isEngaged())
    {
        if (_engagedCursor != QCursor())
        {
            emit cursorHintChanging();
        }

        QPoint viewPortPos = _viewPortPresenter->getFromCanvasTransform().map(canvasPos).toPoint();
        _toolPathHelper.disengage(viewPortPos, canvasPos);
        onPointerDisengage();

        if (_engagedCursor != QCursor())
        {
            emit cursorHintChanged();
        }
    }
}
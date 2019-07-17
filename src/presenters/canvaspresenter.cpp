#include "canvaspresenter.h"

#include <QPainter>
#include <QColor>

CanvasPresenter::CanvasPresenter()
    : _initHelper(this)
{
}

void CanvasPresenter::initialize(IEditorPresenter* editorPresenter)
{
    _initHelper.initializeBegin();
    _editorPresenter = editorPresenter;
    _initHelper.initializeEnd();
}

void CanvasPresenter::setDocument(IDocument* document)
{
    _initHelper.assertInitialized();
    _document = document;
}

bool CanvasPresenter::isEmpty()
{
    _initHelper.assertInitialized();
    if (_document)
        return _document->isEmpty();
    else   
        return false;
}

QSize CanvasPresenter::getSize()
{
    _initHelper.assertInitialized();
    if (_document)
        return _document->getSize();
    else
        return QSize();
}

QColor CanvasPresenter::getBackgroundColor()
{
    _initHelper.assertInitialized();
    if (_document)
        return _document->getBackgroundColor();
    else
        return DEFAULT_BACKGROUND_COLOR;
}

const QPixmap CanvasPresenter::getRender()
{
    _initHelper.assertInitialized();
    if (!_renderIsValid && _document)
    {
        _render = QPixmap(_document->getSize());
        _document->render(QRect(QPoint(), _document->getSize()), &_render);
        _renderIsValid = true;
    }

    return _render;
}
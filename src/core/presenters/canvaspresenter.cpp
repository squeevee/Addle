#include "canvaspresenter.hpp"
#include "common/servicelocator.hpp"
#include "common/utilities/qt_extensions/qobject.hpp"

#include <QPainter>
#include <QColor>

void CanvasPresenter::initialize(IDocumentPresenter* documentPresenter)
{
    _initHelper.initializeBegin();
    _documentPresenter = documentPresenter;
    connect_interface(_documentPresenter,
        SIGNAL(documentChanged(QSharedPointer<IDocument>)),
        this,
        SLOT(onDocumentChanged(QSharedPointer<IDocument>)));

    _initHelper.initializeEnd();
}

IDocumentPresenter* CanvasPresenter::getDocumentPresenter()
{
    _initHelper.assertInitialized();
    return _documentPresenter; 
}

ICanvasView* CanvasPresenter::getView()
{
    _initHelper.assertInitialized();
    if (!_view)
    {
        _view = ServiceLocator::make<ICanvasView>(this);
    }
    return _view;
}

void CanvasPresenter::onDocumentChanged(QSharedPointer<IDocument> document)
{
    _initHelper.assertInitialized();
    _document = document;
    _renderIsValid = false;
    if (_view)
        _view->update();
}

bool CanvasPresenter::isEmpty()
{
    _initHelper.assertInitialized();
    if (_document)
        return _document->isEmpty();
    else   
        return true;
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
#include "viewport.hpp"

void ViewPort::initialize(IViewPortPresenter* presenter, ICanvasPresenter* canvasPresenter)
{
    _initHelper.initializeBegin();

    setResizeMode(QQuickWidget::SizeRootObjectToView);
    setSource(QUrl("qrc:///qml/canvas/viewport.qml"));

    _initHelper.initializeEnd();
}
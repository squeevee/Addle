#include "layerpresenter.hpp"
#include "servicelocator.hpp"
#include "interfaces/views/ilayerview.hpp"

void LayerPresenter::initialize(IDocumentPresenter* documentPresenter, ILayer* layer)
{
    _initHelper.initializeBegin();

    _documentPresenter = documentPresenter;
    _layer = layer;

    _initHelper.initializeEnd();
}


ILayerView* LayerPresenter::getView()
{
    _initHelper.assertInitialized();
    if (!_view)
    {
        _view = ServiceLocator::make<ILayerView>(this);
    }
    return _view;
}

void LayerPresenter::render(QPainter& painter, QRect area)
{
    _layer->render(painter, area);

    if (_rasterOperation)
    {
        _rasterOperation->render(painter, area);
    }
}
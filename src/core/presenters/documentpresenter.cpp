#include "documentpresenter.hpp"
#include "servicelocator.hpp"

#include "interfaces/presenters/ilayerpresenter.hpp"

void DocumentPresenter::initialize(EmptyInitOptions option)
{
    _initHelper.initializeBegin();

    switch(option)
    {
    case initNoModel:
        break;

    case initEmptyModel:
        initialize(ServiceLocator::makeShared<IDocument>());
        break;

    case initBlankDefaults:
        initialize(QSize(800, 600), Qt::white); // TODO: not this
        break;
    }

    _initHelper.initializeEnd();
}

void DocumentPresenter::initialize(QSize size, QColor backgroundColor)
{
    _initHelper.initializeBegin();

    LayerBuilder layer;
    layer.setBoundary(QRect(QPoint(), size));
    
    DocumentBuilder doc;
    doc.addLayer(layer);
    doc.setBackgroundColor(backgroundColor);

    initialize(ServiceLocator::makeShared<IDocument>(doc));

    _initHelper.initializeEnd();
}

void DocumentPresenter::initialize(QSharedPointer<IDocument> model)
{
    _initHelper.initializeBegin();

    //assert model
    _model = model;

    for (auto layer : _model->getLayers())
    {
        QSharedPointer<ILayerPresenter> layerPresenter = ServiceLocator::makeShared<ILayerPresenter>(this, layer);
        _layers.append(layerPresenter);
    }
    
    _initHelper.initializeEnd();
}
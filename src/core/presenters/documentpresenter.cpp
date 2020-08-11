#include "documentpresenter.hpp"
#include "servicelocator.hpp"


#include "utils.hpp"
#include "interfaces/presenters/ilayerpresenter.hpp"
using namespace Addle;

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

    _layersHelper.initialize(_model->getLayers());
    _initHelper.setCheckpoint(InitCheckpoints::Layers);

    _initHelper.initializeEnd();
}

// void DocumentPresenter::addLayer(int index)
// {

// }

// void DocumentPresenter::removeLayer(int index, int count)
// {

// }

// void DocumentPresenter::removeLayer(QSharedPointer<ILayerPresenter> layer)
// {

// }

// void DocumentPresenter::removeLayers(QList<QSharedPointer<ILayerPresenter>> layers)
// {

// }

// void DocumentPresenter::moveLayer(QSharedPointer<ILayerPresenter> layer, int destination)
// {

// }

// void DocumentPresenter::moveLayers(QList<QSharedPointer<ILayerPresenter>> layers, int destination)
// {

// }

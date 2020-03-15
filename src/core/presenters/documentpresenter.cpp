#include "documentpresenter.hpp"
#include "servicelocator.hpp"

void DocumentPresenter::initialize(EmptyInitOptions option)
{
    _initHelper.initializeBegin();

    switch(option)
    {
    case initNoModel:
        break;

    case initEmptyModel:
        _model = ServiceLocator::makeShared<IDocument>();
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

    _model = ServiceLocator::makeShared<IDocument>(doc);

    _initHelper.initializeEnd();
}

void DocumentPresenter::initialize(QWeakPointer<IDocument> model)
{
    _initHelper.initializeBegin();

    //assert model
    _model = model;
    
    _initHelper.initializeEnd();
}
/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "documentpresenter.hpp"
#include "servicelocator.hpp"

#include "utilities/model/documentbuilder.hpp"

#include "utils.hpp"
#include "interfaces/presenters/ilayerpresenter.hpp"
using namespace Addle;

void DocumentPresenter::initialize(EmptyInitOptions option)
{
    const Initializer init(_initHelper);

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
}

void DocumentPresenter::initialize(QSize size, QColor backgroundColor)
{
    const Initializer init(_initHelper);

    initialize(ServiceLocator::makeShared<IDocument>(
        DocumentBuilder()
            .setBackgroundColor(backgroundColor)
            .addLayer(
                LayerBuilder()
                    .setBoundary(QRect(QPoint(), size))
            )
    ));
}

void DocumentPresenter::initialize(QSharedPointer<IDocument> model)
{
    const Initializer init(_initHelper);

    //assert model
    _model = model;

    _layersHelper.initialize(_model->layers());
    _initHelper.setCheckpoint(InitCheckpoints::Layers);
}


void DocumentPresenter::save(QSharedPointer<FileRequest> request)
{
}

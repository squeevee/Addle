#include "layergrouppresenter.hpp"

using namespace Addle;

LayerGroupPresenter::LayerGroupPresenter(
            IDocumentPresenter& document, 
            LayerNodeRef layerNode,
            QSharedPointer<ILayerGroup> model
        )
    : _document(document),
    _layerNode(std::move(layerNode)),
    _model(model)
{
    HelperBase::initialize();
}

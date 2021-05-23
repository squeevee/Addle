#include "layergrouppresenter.hpp"

using namespace Addle;

LayerGroupPresenter::LayerGroupPresenter(
            IDocumentPresenter& document, 
            LayerNode& layerNode,
            QSharedPointer<ILayerGroup> model
        )
    : _document(document),
    _layerNode(layerNode),
    _model(model)
{
    HelperBase::initialize();
}

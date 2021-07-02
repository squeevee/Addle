#include "documentlayersitemmodel.hpp"

#include "interfaces/presenters/ilayerpresenter.hpp"
#include "interfaces/presenters/ilayergrouppresenter.hpp"
#include "interfaces/presenters/ilayernodepresenter.hpp"

#include <QItemSelectionModel>

#include <QtDebug>

using namespace Addle;

DocumentLayersItemModel::DocumentLayersItemModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

void DocumentLayersItemModel::setPresenter(PresenterAssignment<IDocumentPresenter> presenter)
{
    _presenter = presenter;
    if (_presenter)
    {
        this->setTree(_presenter->layers());
        
        _presenter.connect( 
                SIGNAL(layerNodesChanged(DataTreeNodeEvent)),
            this, SLOT(onPresenterLayerNodesChanged(DataTreeNodeEvent))
        );
    }
    else
    {
        this->unsetTree();
    }
}

void DocumentLayersItemModel::setSelectionModel(QItemSelectionModel* selectionModel)
{
    if (_selectionModel)
    {
        disconnect(_selectionModel, &QItemSelectionModel::selectionChanged,
            this, &DocumentLayersItemModel::onSelectionChanged);
    }
    
    _selectionModel = selectionModel;
    
    if (_selectionModel)
    {
        connect(_selectionModel, &QItemSelectionModel::selectionChanged,
            this, &DocumentLayersItemModel::onSelectionChanged);
    }
}

void DocumentLayersItemModel::onSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
    if(!_presenter) return;
    
    
// 
//     QSet<IDocumentPresenter::LayerNode*> newSelection;
// 
//     for (QModelIndex index : selected.indexes())
//     {
//         newSelection.insert(DocumentLayersItemModel::nodeAt(index));
//     }
// 
//     _presenter->setLayerSelection(newSelection);
}

QModelIndex DocumentLayersItemModel::index(int row, int column, const QModelIndex& parent) const
{
    Q_UNUSED(column);
    return this->index_impl(row, parent);
}

QModelIndex DocumentLayersItemModel::parent(const QModelIndex& index) const
{
    return this->parent_impl(index);
}

int DocumentLayersItemModel::rowCount(const QModelIndex& parent) const
{
    return this->rowCount_impl(parent);
}

int DocumentLayersItemModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant DocumentLayersItemModel::data(const QModelIndex& index, int role) const
{
    const LayerNode* node = this->getNode(index);
    if (!node || node->isRoot()) // no data on root node
        return QVariant();
    
    assert(node->value());
    ILayerNodePresenter& nodePresenter = *(node->value());
    
    switch(role)
    {
    case Qt::DisplayRole:
        return nodePresenter.name();
    default:
        return QVariant();
    }
}

void DocumentLayersItemModel::onPresenterLayerNodesChanged(DataTreeNodeEvent event)
{
    this->onNodesChanged(event);
}

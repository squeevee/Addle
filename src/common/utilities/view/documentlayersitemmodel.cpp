#include "documentlayersitemmodel.hpp"

#include "interfaces/presenters/ilayerpresenter.hpp"
#include "interfaces/presenters/ilayergrouppresenter.hpp"
#include "interfaces/presenters/ilayernodepresenter.hpp"

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
        NostalgicHelperBase::setTree(_presenter->layers());
        
        _presenter.connect( 
                SIGNAL(layerNodesAdded(IDocumentPresenter::LayerNodesAddedEvent)),
            this, SLOT(onPresenterLayerNodesAdded(IDocumentPresenter::LayerNodesAddedEvent))
        );
        _presenter.connect( 
                SIGNAL(layerNodesRemoved(IDocumentPresenter::LayerNodesRemovedEvent)),
            this, SLOT(onPresenterLayerNodesRemoved(IDocumentPresenter::LayerNodesRemovedEvent))
        );
    }
    else
    {
        NostalgicHelperBase::unsetTree();
    }
}

QModelIndex DocumentLayersItemModel::index(int row, int column, const QModelIndex& parent) const
{
    Q_UNUSED(column);
    return NostalgicHelperBase::index_impl(row, parent);
}

QModelIndex DocumentLayersItemModel::parent(const QModelIndex& index) const
{
    return NostalgicHelperBase::parent_impl(index);
}

int DocumentLayersItemModel::rowCount(const QModelIndex& parent) const
{
    return NostalgicHelperBase::rowCount_impl(parent);
}

int DocumentLayersItemModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant DocumentLayersItemModel::data(const QModelIndex& index, int role) const
{
    const LayerNode* node = NostalgicHelperBase::getNode(index);
    if (!node || !node->parent()) // no data on root node
        return QVariant();
    
    QSharedPointer<ILayerNodePresenter> nodePresenter = node->value();
    
    switch(role)
    {
    case Qt::DisplayRole:
        return nodePresenter->name();
    default:
        return QVariant();
    }
}

// void DocumentLayersItemModel::onPresenterLayerNodesAdded(IDocumentPresenter::LayerNodesAddedEvent added)
// {
// }
// 
// void DocumentLayersItemModel::onPresenterLayerNodesRemoved(IDocumentPresenter::LayerNodesRemovedEvent removed)
// {
// }

// void DocumentLayersItemModel::onPresenterLayersAdded(QList<IDocumentPresenter::LayerNode*> added)
// {
//     assert _presenter
//     assert !_nodeHelper
//     
//     _nodeHelper.nodesInserted(added);
// 
//     const LayerNode* parent;
//     int first;
//     int last;
//     while (_nodeHelper.insertArgs(&parent, &first, &last))
//     {
//         QModelIndex index = createIndex(parent, _nodeHelper.indexOf(parent));
//         beginInsertRows(index, first, last);
//         _nodeHelper.nextInsert();
//         endInsertRows();
//     };
// 
//     _nodeHelper.clear();
// }

// void DocumentLayersItemModel::onPresenterLayersRemoved(QList<LayerNodeRemoved> removed)
// {
//     assert _presenter
//     assert !_nodeHelper
// 
//     _nodeHelper.nodesRemoved(removed);
// 
//     const LayerNode* parent;
//     int first;
//     int last;
//     while (_nodeHelper.removeArgs(&parent, &first, &last))
//     {
//         QModelIndex index = createIndex(parent, _nodeHelper.indexOf(parent));
//         beginRemoveRows(index, first, last);
//         _nodeHelper.nextRemove();
//         endRemoveRows();
//     };
// 
//     _nodeHelper.clear();
// }

// void DocumentLayersItemModel::onPresenterLayersMoved(QList<IDocumentPresenter::LayerNode*> moved)
// {
// 
// }

// QModelIndex DocumentLayersItemModel::indexOf(const LayerNode* node) const
// {
//     return createIndex(node, node->index());
// }

// DocumentLayersItemModel::LayerNode* DocumentLayersItemModel::nodeAt(const QModelIndex& index)
// {
//     return static_cast<LayerNode*>(index.internalPointer());
// }

// const DocumentLayersItemModel::LayerNode* DocumentLayersItemModel::constNodeAt(const QModelIndex& index)
// {
//     return static_cast<const LayerNode*>(index.internalPointer());
// }

// QModelIndex DocumentLayersItemModel::createIndex(const LayerNode* node, int row) const
// {
//     if (!node->parent())
//         return QModelIndex();
//     else
//         return QAbstractItemModel::createIndex(row, 0, const_cast<void*>(static_cast<const void*>(node)));
// }

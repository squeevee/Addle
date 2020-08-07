#include "documentlayersitemmodel.hpp"

#include "interfaces/presenters/ilayerpresenter.hpp"
#include "utilities/model/layergroupinfo.hpp"

#include <QtDebug>

DocumentLayersItemModel::DocumentLayersItemModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

void DocumentLayersItemModel::setPresenter(PresenterAssignment<IDocumentPresenter> presenter)
{
    beginResetModel();

    _nodeHelper.clear();

    _presenter = presenter;
    if (_presenter)
    {
        _presenter.connect( 
                SIGNAL(layersAdded(QList<IDocumentPresenter::LayerNode*>)),
            this, SLOT(onPresenterLayersAdded(QList<IDocumentPresenter::LayerNode*>))
        );
        _presenter.connect( 
                SIGNAL(layersRemoved(QList<IDocumentPresenter::LayerNodeRemoved>)),
            this, SLOT(onPresenterLayersRemoved(QList<IDocumentPresenter::LayerNodeRemoved>))
        );
    }
    
    endResetModel();
}

QModelIndex DocumentLayersItemModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!_presenter)
        return QModelIndex();

    const LayerNode* parentNode;
    if (parent.isValid())
        parentNode = nodeAt(parent);
    else
        parentNode = &_presenter->layers().const_root();

    if (_nodeHelper.isNostalgic())    
    {
        return createIndex(_nodeHelper.nodeAt(parentNode, row), row);
    }
    else
    {
        const LayerNode* node = &parentNode->at(row);
        return createIndex(node, node->index());
    }
}

QModelIndex DocumentLayersItemModel::parent(const QModelIndex& index) const
{
    if (!_presenter)
        return QModelIndex();

    if (_nodeHelper.isNostalgic())
    {
        const LayerNode* parent = nodeAt(index)->parent();
        return createIndex(parent, _nodeHelper.indexOf(parent));
    }
    else 
    {
        return indexOf(nodeAt(index)->parent());
    }
}

int DocumentLayersItemModel::rowCount(const QModelIndex& parent) const
{
    if (!_presenter)
        return 0;

    const LayerNode* parentNode;
    if (parent.isValid())
        parentNode = nodeAt(parent);
    else
        parentNode = &_presenter->layers().const_root();

    if (_nodeHelper.isNostalgic())
    {
        return _nodeHelper.sizeOf(parentNode);
    }
    else 
    {
        return parentNode->size();
    }
}

int DocumentLayersItemModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant DocumentLayersItemModel::data(const QModelIndex& index, int role) const
{
    if (!_presenter)
        return QVariant();

    const LayerNode* node = nodeAt(index);

    switch(role)
    {
    case Qt::DisplayRole:
        {
            if (node->isGroup())
            {
                // assert can convert
                LayerGroupInfo info = node->metaData().value<LayerGroupInfo>();

                return info.name();
            }
            else
            {
                return node->asValue()->name();
            }
        }
    default:
        return QVariant();
    }
}

void DocumentLayersItemModel::onPresenterLayersAdded(QList<IDocumentPresenter::LayerNode*> added)
{
    // assert _presenter
    // assert !_nodeHelper
    
    _nodeHelper.nodesInserted(added);

    const LayerNode* parent;
    int first;
    int last;
    while (_nodeHelper.getInsertArgs(&parent, &first, &last))
    {
        QModelIndex index = createIndex(parent, _nodeHelper.indexOf(parent));
        beginInsertRows(index, first, last);
        _nodeHelper.nextInsert();
        endInsertRows();
    };

    _nodeHelper.clear();
}

void DocumentLayersItemModel::onPresenterLayersRemoved(QList<LayerNodeRemoved> removed)
{
    // assert _presenter
    // assert !_nodeHelper

    _nodeHelper.nodesRemoved(removed);

    const LayerNode* parent;
    int first;
    int last;
    while (_nodeHelper.getRemoveArgs(&parent, &first, &last))
    {
        QModelIndex index = createIndex(parent, _nodeHelper.indexOf(parent));
        beginRemoveRows(index, first, last);
        _nodeHelper.nextRemove();
        endRemoveRows();
    };

    _nodeHelper.clear();
}

void DocumentLayersItemModel::onPresenterLayersMoved(QList<IDocumentPresenter::LayerNode*> moved)
{

}

QModelIndex DocumentLayersItemModel::indexOf(const LayerNode* node) const
{
    return createIndex(node, node->index());
}

DocumentLayersItemModel::LayerNode* DocumentLayersItemModel::nodeAt(const QModelIndex& index)
{
    return static_cast<LayerNode*>(index.internalPointer());
}

const DocumentLayersItemModel::LayerNode* DocumentLayersItemModel::constNodeAt(const QModelIndex& index)
{
    return static_cast<const LayerNode*>(index.internalPointer());
}

QModelIndex DocumentLayersItemModel::createIndex(const LayerNode* node, int row) const
{
    if (!node->parent())
        return QModelIndex();
    else
        return QAbstractItemModel::createIndex(row, 0, const_cast<void*>(static_cast<const void*>(node)));
}
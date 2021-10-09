#include <boost/range/adaptor/filtered.hpp>

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
        
        _presenter.connect( 
                    SIGNAL(layerSelectionChanged(QSet<ILayerNodePresenter::LayerNodeRef>)),
                this, SLOT(onPresenterSelectionChanged(QSet<ILayerNodePresenter::LayerNodeRef>))
            );
        
        updateSelection();
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
        
        updateSelection();
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
    assert(_selectionModel);
    if(!_presenter) return;
    
    auto lock = _presenter->layers().lockForRead();
    
    QSet<ILayerNodePresenter::LayerNodeRef> newSelection;
    newSelection.reserve(selected.size());
    
    for (QModelIndex index : selected.indexes())
    {
        auto node = const_cast<ILayerNodePresenter::LayerNode*>(getNode(index));
        
        if (Q_UNLIKELY(!node)) continue;
        
        newSelection.insert(node->nodeRef());
    }
    
    lock->unlock();
    
    emit _presenter->setLayerSelection(newSelection);
    
    Q_UNUSED(deselected);
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
    auto lock = _presenter->layers().lockForRead();
    
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
    
    if (_selectionNeedsUpdate)
    {
        _selectionNeedsUpdate = false;
        updateSelection();
    }
}

void DocumentLayersItemModel::onPresenterSelectionChanged(QSet<ILayerNodePresenter::LayerNodeRef> selection)
{
    if (!_selectionModelRecentlyChanged)
    {
        updateSelection();
    }
    _selectionModelRecentlyChanged = false;
}

void DocumentLayersItemModel::updateSelection()
{
    if (!_presenter || !_selectionModel) return;
    
    auto lock = _presenter->layers().lockForRead();
    
    if (_presenter->layerSelection().isEmpty())
    {
        _selectionModel->clear();
        return;
    }
    
    // make a copy of the presenter's layer selection. Any nodes that don't
    // exist in the model's working state or in the layer tree's current real
    // state will be problematic, so we will filter them out and indicate that
    // the selection must be updated at next state change.
    
    QVarLengthArray<DataTreeNodeAddress, 256> workingList;
    workingList.reserve(_presenter->layerSelection().size());
    
    for (const ILayerNodePresenter::LayerNodeRef& ref : noDetach(_presenter->layerSelection()))
    {
        if (Q_UNLIKELY(!ref.isValid()))
        {
            _selectionNeedsUpdate = true;
            continue;
        }
        
        auto address = _state.mapFromCurrent((*ref).address());
        
        if (Q_UNLIKELY(!address))
        {
            _selectionNeedsUpdate = true;
            continue;
        }
        
        workingList.push_back(std::move(*address));
    }
    
    if (Q_UNLIKELY(workingList.empty()))
    {
        _selectionModel->clear();
        return;
    }
    
    // Order the working list so that sequences of siblings will be adjacent to 
    // each other
    
    std::sort(
            workingList.begin(), 
            workingList.end(),
            aux_datatree::NodeAddressBFSComparator {}
        );
    
    QItemSelection selection;
    
    QVarLengthArray<DataTreeNodeAddress, 256>::const_iterator i, j, end;
    
    i = workingList.cbegin();
    j = i;
    end = workingList.cend();
    
    DataTreeNodeAddress prev;
    
    // search for runs of continuous siblings and add them to the selection
    while (true)
    {
        prev = *j;
        ++j;
        
        if (j == end 
            || (*j).parent() != prev.parent()
            || (*j).lastIndex() != prev.lastIndex() + 1
        )
        {
            selection.select(
                    this->getIndex(*i), 
                    this->getIndex(prev)
                );
            
            if (j == end) break;
            i = j;
        }
    }
    
    _selectionModel->select(selection, QItemSelectionModel::ClearAndSelect);
}

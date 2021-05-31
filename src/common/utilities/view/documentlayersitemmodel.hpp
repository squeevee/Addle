#ifndef DOCUMENTLAYERSITEMMODEL_HPP
#define DOCUMENTLAYERSITEMMODEL_HPP

#include <map>
#include <set>
#include <memory>

#include <QHash>
#include <QAbstractItemModel>
#include "compat.hpp"

#include "../presenter/presenterassignment.hpp"

#include "interfaces/presenters/idocumentpresenter.hpp"

// #include "utilities/datatree/observers.hpp"

#include "./nostalgicitemmodelhelperbase.hpp"

namespace Addle {

// template<typename NodeType>
// class NodeModelHelper;

class ADDLE_COMMON_EXPORT DocumentLayersItemModel 
    : public QAbstractItemModel,
    private NostalgicItemModelHelperBase<DocumentLayersItemModel, IDocumentPresenter::LayersTree>
{
    Q_OBJECT
    
    using NostalgicHelperBase = NostalgicItemModelHelperBase<DocumentLayersItemModel, IDocumentPresenter::LayersTree>; 
    using LayerNode = IDocumentPresenter::LayersTree::Node;
public:
    DocumentLayersItemModel(QObject* parent = nullptr);
    virtual ~DocumentLayersItemModel() = default;

    void setPresenter(PresenterAssignment<IDocumentPresenter> presenter);
    
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

//     QModelIndex indexOf(const LayerNode* node) const;

//     static LayerNode* nodeAt(const QModelIndex& index);
//     static const LayerNode* constNodeAt(const QModelIndex& index);

// private slots:
//     void onPresenterLayerNodesAdded(IDocumentPresenter::LayerNodesAddedEvent added);
//     void onPresenterLayerNodesRemoved(IDocumentPresenter::LayerNodesRemovedEvent removed);
//     void onPresenterLayersMoved(QList<IDocumentPresenter::LayerNode*> added);

private:
    PresenterAssignment<IDocumentPresenter> _presenter;
    
    friend class NostalgicItemModelHelperBase<DocumentLayersItemModel, IDocumentPresenter::LayersTree>;
};

} // namespace Addle

#endif // DOCUMENTLAYERSITEMMODEL_HPP

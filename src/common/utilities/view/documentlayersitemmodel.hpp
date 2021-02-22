#ifndef DOCUMENTLAYERSITEMMODEL_HPP
#define DOCUMENTLAYERSITEMMODEL_HPP

#include <map>
#include <set>
#include <memory>

#include <QHash>
#include <QAbstractItemModel>
#include "compat.hpp"

#include "../presenter/presenterassignment.hpp"
#include "nodemodelhelper.hpp"

#include "interfaces/presenters/idocumentpresenter.hpp"
namespace Addle {

// template<typename NodeType>
// class NodeModelHelper;

class ADDLE_COMMON_EXPORT DocumentLayersItemModel : public QAbstractItemModel
{
    Q_OBJECT

//     typedef IDocumentPresenter::LayerList LayerList;
//     typedef IDocumentPresenter::LayerNode LayerNode;
//     typedef IDocumentPresenter::LayerNodeRemoved LayerNodeRemoved;
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

public slots:
//     void onPresenterLayersAdded(QList<IDocumentPresenter::LayerNode*> added);
//     void onPresenterLayersRemoved(QList<IDocumentPresenter::LayerNodeRemoved> removed);
//     void onPresenterLayersMoved(QList<IDocumentPresenter::LayerNode*> added);

private:
//     QModelIndex createIndex(const LayerNode* node, int row) const;

//     NodeModelHelper<LayerNode> _nodeHelper;
    PresenterAssignment<IDocumentPresenter> _presenter;
};

} // namespace Addle

#endif // DOCUMENTLAYERSITEMMODEL_HPP

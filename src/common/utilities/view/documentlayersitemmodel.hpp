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

// #include "utilities/datatree/observer.hpp"

#include "./observedtreeitemmodelhelperbase.hpp"

class QItemSelection;
class QItemSelectionModel;

namespace Addle {

// template<typename NodeType>
// class NodeModelHelper;

class ADDLE_COMMON_EXPORT DocumentLayersItemModel 
    : public QAbstractItemModel,
    private ObservedTreeItemModelHelperBase<DocumentLayersItemModel, IDocumentPresenter::LayersTree>
{
    Q_OBJECT
    
    using LayerNode = IDocumentPresenter::LayersTree::Node;
public:
    DocumentLayersItemModel(QObject* parent = nullptr);
    virtual ~DocumentLayersItemModel() = default;

    void setPresenter(PresenterAssignment<IDocumentPresenter> presenter);
    void setSelectionModel(QItemSelectionModel* selectionModel);
    
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

private slots:
    void onPresenterLayerNodesChanged(DataTreeNodeEvent);
    void onSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    
private:
    PresenterAssignment<IDocumentPresenter> _presenter;
    QItemSelectionModel* _selectionModel = nullptr;
    
    friend class ObservedTreeItemModelHelperBase<DocumentLayersItemModel, IDocumentPresenter::LayersTree>;
};

} // namespace Addle

#endif // DOCUMENTLAYERSITEMMODEL_HPP

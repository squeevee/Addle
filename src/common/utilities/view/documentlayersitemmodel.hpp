#ifndef DOCUMENTLAYERSITEMMODEL_HPP
#define DOCUMENTLAYERSITEMMODEL_HPP

#include <QAbstractItemModel>
#include "compat.hpp"

#include "../presenter/presenterassignment.hpp"

#include "interfaces/presenters/idocumentpresenter.hpp"

class ADDLE_COMMON_EXPORT DocumentLayersItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    DocumentLayersItemModel(QObject* parent = nullptr);
    virtual ~DocumentLayersItemModel() = default;

    void setPresenter(PresenterAssignment<IDocumentPresenter> presenter);
    
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

private:
    PresenterAssignment<IDocumentPresenter> _presenter;
};

#endif // DOCUMENTLAYERSITEMMODEL_HPP
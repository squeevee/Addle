#include "documentlayersitemmodel.hpp"

DocumentLayersItemModel::DocumentLayersItemModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

void DocumentLayersItemModel::setPresenter(PresenterAssignment<IDocumentPresenter> presenter)
{
    _presenter = presenter;
}

QModelIndex DocumentLayersItemModel::index(int row, int column, const QModelIndex& parent) const
{
    return QModelIndex();
}

QModelIndex DocumentLayersItemModel::parent(const QModelIndex& index) const
{
    return QModelIndex();
}

int DocumentLayersItemModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    // if (!_presenter) return 0;

    // return _presenter->layers().count();
    return 0;
}

int DocumentLayersItemModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant DocumentLayersItemModel::data(const QModelIndex& index, int role) const
{
    return QVariant();
}
#include "assetselector.hpp"
#include <QGridLayout>

#include <QtDebug>

AssetSelector::AssetSelector(IAssetSelectionPresenter& presenter, QWidget* parent)
    : QWidget(parent), _presenter(presenter)
{
    QGridLayout* layout = new QGridLayout(this);
    QWidget::setLayout(layout);

    _list = new QListWidget(this);
    _list->setViewMode(QListView::IconMode);
    // _list->setFlow(QListView::LeftToRight);
    // _list->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    // _list->setMaximumHeight(45);
    // _list->setIconSize(QSize(16, 16));
    layout->addWidget(_list);

    connect(_list, &QListWidget::itemSelectionChanged, this, &AssetSelector::onSelectionChanged);

    for (auto asset : _presenter.assets())
    {
        QListWidgetItem* item = new QListWidgetItem(_list);
        item->setText(asset->name());
        item->setIcon(asset->icon());

        _items[asset->getId()] = item;
        _itemValues[item] = asset;

        _list->addItem(item);
    }

    for (auto selected : _presenter.selectedIds())
    {
        _items[selected]->setSelected(true);
    }
}

void AssetSelector::onSelectionChanged()
{
    QList<PersistentId> selected;
    for (auto item : _list->selectedItems())
    {
        selected.append(_itemValues[item]->getId());
    }

    _presenter.select(selected);

    emit changed();
}
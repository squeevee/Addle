#include "assetselector.hpp"
#include "utilities/qtextensions/qobject.hpp"
#include "interfaces/presenters/assets/iassetpresenter.hpp"
#include <QGridLayout>

#include <QtDebug>

using namespace Addle;

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
    _list->setIconSize(QSize(64, 64));
    layout->addWidget(_list);

    connect(_list, &QListWidget::itemSelectionChanged, this, &AssetSelector::onListSelectionChanged);

    for (auto asset : _presenter.assets())
    {
        QListWidgetItem* item = new QListWidgetItem(_list);
        item->setText(asset->name());
        item->setIcon(asset->icon());

        _items[asset->id()] = item;
        _itemValues[item] = asset;

        _list->addItem(item);
    }

    connect_interface(&_presenter, SIGNAL(selectionChanged(QList<PersistentId>)), this, SLOT(onPresenterSelectionChanged()));
    onPresenterSelectionChanged();
}

void AssetSelector::onListSelectionChanged()
{
    QList<PersistentId> selected;
    for (auto item : _list->selectedItems())
    {
        selected.append(_itemValues[item]->id());
    }

    _presenter.select(selected);

    emit changed();
}

void AssetSelector::onPresenterSelectionChanged()
{
    for (auto selected : _presenter.selectedIds())
    {
        _items.value(selected)->setSelected(true);
    }
}
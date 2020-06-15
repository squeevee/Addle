#ifndef ASSETSELECTOR_HPP
#define ASSETSELECTOR_HPP

#include <QWidget>
#include <QListWidget>

#include "interfaces/presenters/toolpresenters/iassetselectionpresenter.hpp"

class AssetSelector : public QWidget
{
    Q_OBJECT
public:
    AssetSelector(QWidget* parent = nullptr);
    virtual ~AssetSelector() = default;

private:
    QWeakPointer<IAssetSelectionPresenter> _presenter;

    QListWidget* _list;

    QHash<double, QListWidgetItem*> _items;
    QHash<QListWidgetItem*, double> _itemValues;
};

#endif // ASSETSELECTOR_HPP

#ifndef ASSETSELECTOR_HPP
#define ASSETSELECTOR_HPP

#include <QWidget>
#include <QListWidget>

#include "interfaces/presenters/toolpresenters/iassetselectionpresenter.hpp"

class AssetSelector : public QWidget
{
    Q_OBJECT
public:
    AssetSelector(IAssetSelectionPresenter& presenter, QWidget* parent = nullptr);
    virtual ~AssetSelector() = default;

signals: 
    void changed();

private slots:
    void onSelectionChanged();

private:
    IAssetSelectionPresenter& _presenter;

    QListWidget* _list;

    QHash<PersistentId, QListWidgetItem*> _items;
    QHash<QListWidgetItem*, QSharedPointer<IAssetPresenter>> _itemValues;
};

#endif // ASSETSELECTOR_HPP

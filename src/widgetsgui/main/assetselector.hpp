#ifndef ASSETSELECTOR_HPP
#define ASSETSELECTOR_HPP

#include <QWidget>
#include <QListWidget>

#include "interfaces/presenters/tools/iassetselectionpresenter.hpp"

namespace Addle {

class AssetSelector : public QWidget
{
    Q_OBJECT
public:
    AssetSelector(IAssetSelectionPresenter& presenter, QWidget* parent = nullptr);
    virtual ~AssetSelector() = default;

signals: 
    void changed();

private slots:
    void onListSelectionChanged();
    void onPresenterSelectionChanged();

private:
    IAssetSelectionPresenter& _presenter;

    QListWidget* _list;

    QHash<PersistentId, QListWidgetItem*> _items;
    QHash<QListWidgetItem*, QSharedPointer<IAssetPresenter>> _itemValues;
};

} // namespace Addle

#endif // ASSETSELECTOR_HPP

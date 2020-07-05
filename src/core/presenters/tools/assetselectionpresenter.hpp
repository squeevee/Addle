#ifndef ASSETSELECTIONPRESENTER_HPP
#define ASSETSELECTIONPRESENTER_HPP

#include "compat.hpp"

#include <list>
#include <QObject>
#include <QHash>
#include <QSet>
#include "utilities/initializehelper.hpp"

#include "interfaces/presenters/tools/iassetselectionpresenter.hpp"

class ADDLE_CORE_EXPORT AssetSelectionPresenter : public QObject, public IAssetSelectionPresenter
{
    Q_OBJECT
public:
    virtual ~AssetSelectionPresenter() = default;

    void initialize(QList<QSharedPointer<IAssetPresenter>> assets, bool canMultiSelect);

    PersistentId selectedId()
    {
        _initHelper.check(); 
        return _selectedIds.isEmpty() ? PersistentId() : _selectedIds.first();
    }
    QList<PersistentId> selectedIds() { _initHelper.check(); return _selectedIds; }

    QSharedPointer<IAssetPresenter> selectedPresenter()
    {
        _initHelper.check();
        return _selection.isEmpty() ? nullptr : _selection.first();
    }
    QList<QSharedPointer<IAssetPresenter>> selectedPresenters() { _initHelper.check(); return _selection; }

    bool canMultiSelect() { _initHelper.check(); return _canMultiSelect; }

    void select(PersistentId assetId);
    void select(QList<PersistentId> assetIds);

    void select(QSharedPointer<IAssetPresenter> asset);
    void select(QList<QSharedPointer<IAssetPresenter>> assets);

    QList<PersistentId> ids() { _initHelper.check(); return _ids; }

    QList<QSharedPointer<IAssetPresenter>> assets() { _initHelper.check(); return _assets; }
    void setAssets(QList<QSharedPointer<IAssetPresenter>> assets);

    QSharedPointer<IAssetPresenter> assetById(PersistentId id) { _initHelper.check(); return _assets_ById.value(id); }
    
    QList<PersistentId> favorites() { _initHelper.check(); return _favorites;}
    void setFavorites(QList<PersistentId> favorites);

    void addFavorite(PersistentId id);
    void removeFavorite(PersistentId id);

    //QList<PersistentId> recent();

signals:
    void selectionChanged(QList<PersistentId> selection);
    void assetsChanged(QList<PersistentId> ids);
    void favoritesChanged(QList<PersistentId> favorites);

    void refresh();
public:
    void pushRecent(PersistentId id);

    bool _canMultiSelect;

    QList<PersistentId> _ids;
    QList<QSharedPointer<IAssetPresenter>> _assets;
    QHash<PersistentId, QSharedPointer<IAssetPresenter>> _assets_ById;

    QList<PersistentId> _selectedIds;
    QList<QSharedPointer<IAssetPresenter>> _selection;

    QList<PersistentId> _favorites;

    std::list<PersistentId> _recent;
    QHash<PersistentId, std::list<PersistentId>::iterator> _recentIndex;
    QList<PersistentId> _recentCache;

    InitializeHelper<AssetSelectionPresenter> _initHelper;
};

#endif // ASSETSELECTIONPRESENTER_HPP
/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef ASSETSELECTIONPRESENTER_HPP
#define ASSETSELECTIONPRESENTER_HPP

#include "compat.hpp"

#include <list>
#include <QObject>
#include <QHash>
#include <QSet>
#include "utilities/initializehelper.hpp"

#include "interfaces/presenters/tools/iassetselectionpresenter.hpp"
namespace Addle {

class ADDLE_CORE_EXPORT AssetSelectionPresenter : public QObject, public IAssetSelectionPresenter
{
    Q_OBJECT
    Q_INTERFACES(Addle::IAssetSelectionPresenter)
    IAMQOBJECT_IMPL
public:
    virtual ~AssetSelectionPresenter() = default;

    void initialize(QList<QSharedPointer<IAssetPresenter>> assets, bool canMultiSelect);

    AddleId selectedId()
    {
        _initHelper.check(); 
        return _selectedIds.isEmpty() ? AddleId() : _selectedIds.first();
    }
    QList<AddleId> selectedIds() { _initHelper.check(); return _selectedIds; }

    QSharedPointer<IAssetPresenter> selectedPresenter()
    {
        _initHelper.check();
        return _selection.isEmpty() ? nullptr : _selection.first();
    }
    QList<QSharedPointer<IAssetPresenter>> selectedPresenters() { _initHelper.check(); return _selection; }

    bool canMultiSelect() { _initHelper.check(); return _canMultiSelect; }

    void select(AddleId assetId);
    void select(QList<AddleId> assetIds);

    void select(QSharedPointer<IAssetPresenter> asset);
    void select(QList<QSharedPointer<IAssetPresenter>> assets);

    QList<AddleId> ids() { _initHelper.check(); return _ids; }

    QList<QSharedPointer<IAssetPresenter>> assets() { _initHelper.check(); return _assets; }
    void setAssets(QList<QSharedPointer<IAssetPresenter>> assets);

    QSharedPointer<IAssetPresenter> assetById(AddleId id) { _initHelper.check(); return _assets_ById.value(id); }
    
    QList<AddleId> favorites() { _initHelper.check(); return _favorites;}
    void setFavorites(QList<AddleId> favorites);

    void addFavorite(AddleId id);
    void removeFavorite(AddleId id);

    //QList<AddleId> recent();

signals:
    void selectionChanged(QList<AddleId> selection);
    void assetsChanged(QList<AddleId> ids);
    void favoritesChanged(QList<AddleId> favorites);

    void refresh();
public:
    void pushRecent(AddleId id);

    bool _canMultiSelect;

    QList<AddleId> _ids;
    QList<QSharedPointer<IAssetPresenter>> _assets;
    QHash<AddleId, QSharedPointer<IAssetPresenter>> _assets_ById;

    QList<AddleId> _selectedIds;
    QList<QSharedPointer<IAssetPresenter>> _selection;

    QList<AddleId> _favorites;

    std::list<AddleId> _recent;
    QHash<AddleId, std::list<AddleId>::iterator> _recentIndex;
    QList<AddleId> _recentCache;

    InitializeHelper _initHelper;
};

} // namespace Addle
#endif // ASSETSELECTIONPRESENTER_HPP
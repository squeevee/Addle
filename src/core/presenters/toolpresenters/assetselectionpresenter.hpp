#ifndef ASSETSELECTIONPRESENTER_HPP
#define ASSETSELECTIONPRESENTER_HPP

#include "compat.hpp"

#include <QObject>
#include <QHash>
#include "utilities/initializehelper.hpp"

#include "interfaces/presenters/toolpresenters/iassetselectionpresenter.hpp"

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

signals:
    void selectionChanged(QList<PersistentId> selection);
    void assetsChanged(QList<PersistentId> ids);

public:

    bool _canMultiSelect;

    QList<PersistentId> _ids;
    QList<QSharedPointer<IAssetPresenter>> _assets;
    QHash<PersistentId, QSharedPointer<IAssetPresenter>> _assets_ById;

    QList<PersistentId> _selectedIds;
    QList<QSharedPointer<IAssetPresenter>> _selection;

    InitializeHelper<AssetSelectionPresenter> _initHelper;
};

#endif // ASSETSELECTIONPRESENTER_HPP
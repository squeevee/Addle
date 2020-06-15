#include "assetselectionpresenter.hpp"

void AssetSelectionPresenter::initialize(QList<QSharedPointer<IAssetPresenter>> assets, bool canMultiSelect)
{
    _initHelper.initializeBegin();

    _assets = assets;
    for (auto asset : _assets)
    {
        _ids.append(asset->getId());
        _assets_ById[asset->getId()] = asset;
    }

    _canMultiSelect = canMultiSelect;

    _initHelper.initializeEnd();
}

void AssetSelectionPresenter::select(PersistentId assetId)
{
    if (!_assets_ById.contains(assetId)) return; // error

    _selectedIds = { assetId };
    _selection = { _assets_ById.value(assetId) };

    emit selectionChanged(_selectedIds);
}

void AssetSelectionPresenter::select(QList<PersistentId> assetIds)
{
    for (auto id : assetIds)
    {
        if (!_assets_ById.contains(id)) return; // error
    }

    _selectedIds = assetIds;
    _selection.clear();
    for (auto id : assetIds)
    {
        _selection.append(_assets_ById.value(id));
    }

    emit selectionChanged(_selectedIds);
}

void AssetSelectionPresenter::select(QSharedPointer<IAssetPresenter> asset)
{
     if (!_assets_ById.contains(asset->getId())) return; // error

    _selection = { asset };
    _selectedIds = { asset->getId() };

    emit selectionChanged(_selectedIds);
}

void AssetSelectionPresenter::select(QList<QSharedPointer<IAssetPresenter>> assets)
{
    for (auto asset : assets)
    {
        if (!_assets_ById.contains(asset->getId())) return; // error
    }

    _selection = assets;
    _selectedIds.clear();
    for (auto asset : _selection)
    {
        _selectedIds.append(asset->getId());
    }

    emit selectionChanged(_selectedIds);
}

void AssetSelectionPresenter::setAssets(QList<QSharedPointer<IAssetPresenter>> assets)
{
    _initHelper.check();

    _assets = assets;

    _ids.clear();
    _assets_ById.clear();

    for (auto asset : _assets)
    {
        _ids.append(asset->getId());
        _assets_ById[asset->getId()] = asset;
    }

    emit assetsChanged(_ids);
}
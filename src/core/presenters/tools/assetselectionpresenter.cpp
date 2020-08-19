/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "assetselectionpresenter.hpp"
using namespace Addle;

void AssetSelectionPresenter::initialize(QList<QSharedPointer<IAssetPresenter>> assets, bool canMultiSelect)
{
    const Initializer init(_initHelper);

    _assets = assets;
    for (auto asset : _assets)
    {
        _ids.append(asset->id());
        _assets_ById[asset->id()] = asset;
    }

    _canMultiSelect = canMultiSelect;
}

void AssetSelectionPresenter::select(PersistentId assetId)
{
    if (!_assets_ById.contains(assetId)) return; // error

    _selectedIds = { assetId };
    _selection = { _assets_ById.value(assetId) };
    pushRecent(assetId);

    emit selectionChanged(_selectedIds);
}

void AssetSelectionPresenter::select(QList<PersistentId> assetIds)
{
    if (!_canMultiSelect)
    {
        select(assetIds.first());
        return; // error?
    }

    for (auto id : assetIds)
    {
        if (!_assets_ById.contains(id)) return; // error?
    }

    _selectedIds = assetIds;
    _selection.clear();
    for (auto id : assetIds)
    {
        _selection.append(_assets_ById.value(id));
        pushRecent(id);
    }

    emit selectionChanged(_selectedIds);
}

void AssetSelectionPresenter::select(QSharedPointer<IAssetPresenter> asset)
{
    if (!_assets_ById.contains(asset->id())) return; // error?

    if (_selectedIds.size() == 1 && _selectedIds.first() == asset->id()) return;

    _selection = { asset };
    _selectedIds = { asset->id() };
    pushRecent(asset->id());

    emit selectionChanged(_selectedIds);
}

void AssetSelectionPresenter::select(QList<QSharedPointer<IAssetPresenter>> assets)
{
    if (!_canMultiSelect)
    {
        select(assets.first());
        return; // error?
    }

    for (auto asset : assets)
    {
        if (!_assets_ById.contains(asset->id())) return; // error?
    }

    _selection = assets;
    _selectedIds.clear();
    for (auto asset : _selection)
    {
        _selectedIds.append(asset->id());
        pushRecent(asset->id());
    }

    emit selectionChanged(_selectedIds);
}

void AssetSelectionPresenter::setAssets(QList<QSharedPointer<IAssetPresenter>> assets)
{
    _initHelper.check();

    if (assets != _assets)
    {
        _assets = assets;

        _ids.clear();
        _assets_ById.clear();

        for (auto asset : _assets)
        {
            _ids.append(asset->id());
            _assets_ById[asset->id()] = asset;
        }

        emit assetsChanged(_ids);
    }
}    

void AssetSelectionPresenter::setFavorites(QList<PersistentId> favorites)
{
    _initHelper.check();
    if (favorites != _favorites)
    {
        _favorites = favorites;
        emit favoritesChanged(_favorites);
    }
}

void AssetSelectionPresenter::addFavorite(PersistentId id)
{
    _initHelper.check();
    if (!_favorites.contains(id)) 
    {
        _favorites.append(id);
        emit favoritesChanged(_favorites);
    }
}

void AssetSelectionPresenter::removeFavorite(PersistentId id)
{
    _initHelper.check();
    if (_favorites.contains(id)) 
    {
        _favorites.removeAll(id);
        emit favoritesChanged(_favorites);
    }
}

void AssetSelectionPresenter::pushRecent(PersistentId id)
{
    if (_recentIndex.contains(id))
    {
        const auto i = _recentIndex[id];
        _recent.erase(i);
    }
    
    const auto i = _recent.insert(_recent.begin(), id);
    _recentIndex[id] = i;

    //TODO: limit history length
}
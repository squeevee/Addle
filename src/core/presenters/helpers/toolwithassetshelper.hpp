#ifndef TOOLWITHASSETSHELPER_HPP
#define TOOLWITHASSETSHELPER_HPP

#include <type_traits>

#include "utilities/qtextensions/qlist.hpp"
#include "interfaces/presenters/toolpresenters/itoolwithassetspresenter.hpp"

template<class PresenterType, class IdType>
class ToolWithAssetsHelper
{
    static_assert(
        std::is_base_of<IAssetPresenter, PresenterType>::value,
        "PresenterType must derive from IAssetPresenter"
    );

    static_assert(
        std::is_base_of<PersistentId, IdType>::value,
        "IdType must derive from PersistentId"
    );

public:
    ToolWithAssetsHelper(
            IToolWithAssetsPresenter& owner,
            IdType defaultSelected = IdType(),
            QList<QSharedPointer<PresenterType>> assets = QList<QSharedPointer<PresenterType>>()
        )
        : _owner(owner),
        defaultSelected(defaultSelected),
        _selected(defaultSelected)
    {
        if (!assets.isEmpty())
            setAssetList(assets);
    }

    void setAssetList(QList<QSharedPointer<PresenterType>> assets)
    {
        for (QSharedPointer<PresenterType> asset : assets)
        {
            _assets.insert(asset->getId(), asset);
        }
        _assetIds = _assets.keys();
        _presenterList = _assets.values();
        _upcastPresenterList = qSharedPointerListCast<IAssetPresenter>(_presenterList);
    }

    QList<QSharedPointer<IAssetPresenter>> getAllAssets()
    {
        return _upcastPresenterList;
    }

    QList<IdType> getAssetIds()
    {
        return _assetIds;
    }

    QList<QSharedPointer<PresenterType>> getAllAssets_p()
    {
        return _presenterList;
    }

    QSharedPointer<PresenterType> getAssetPresenter(PersistentId id)
    {
        //assert has
        return _assets.value(static_cast<IdType>(id));
    }

    const IdType defaultSelected;

    IdType getSelectedAsset() { return _selected; }

    void selectAsset(IdType selection)
    {        
        if (selection != _selected && _assets.contains(selection))
        {
            _selected = selection;
            emit _owner.selectedAssetChanged(_selected);
        }
    }

    QSharedPointer<PresenterType> getSelectedAssetPresenter()
    {
        return _selected ? _assets.value(_selected) : QSharedPointer<PresenterType>();
    }

private:
    IdType _selected;

    QHash<IdType, QSharedPointer<PresenterType>> _assets;

    QList<IdType> _assetIds; 

    QList<QSharedPointer<IAssetPresenter>> _upcastPresenterList;
    QList<QSharedPointer<PresenterType>> _presenterList;

    IToolWithAssetsPresenter& _owner;
};

#endif // TOOLWITHASSETSHELPER_HPP
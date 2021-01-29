/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IASSETSELECTIONPRESENTER_HPP
#define IASSETSELECTIONPRESENTER_HPP

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"


#include "../assets/iassetpresenter.hpp"

#include <QList>
namespace Addle {

class IAssetSelectionPresenter : public virtual IAmQObject
{
public:
    virtual ~IAssetSelectionPresenter() = default;

    virtual void initialize(QList<QSharedPointer<IAssetPresenter>> assets, bool canMultiSelect = true) = 0;

    virtual AddleId selectedId() = 0;
    virtual QList<AddleId> selectedIds() = 0;

    virtual QSharedPointer<IAssetPresenter> selectedPresenter() = 0;
    virtual QList<QSharedPointer<IAssetPresenter>> selectedPresenters() = 0;

    virtual bool canMultiSelect() = 0;

    virtual void select(AddleId assetId) = 0;
    virtual void select(QList<AddleId> assetIds) = 0;

    virtual void select(QSharedPointer<IAssetPresenter> asset) = 0;
    virtual void select(QList<QSharedPointer<IAssetPresenter>> assets) = 0;

    virtual QList<AddleId> ids() = 0;

    virtual QList<QSharedPointer<IAssetPresenter>> assets() = 0;
    virtual void setAssets(QList<QSharedPointer<IAssetPresenter>> assets) = 0;

    virtual QSharedPointer<IAssetPresenter> assetById(AddleId id) = 0;

    virtual QList<AddleId> favorites() = 0;
    virtual void setFavorites(QList<AddleId> favorites) = 0;

    virtual void addFavorite(AddleId id) = 0;
    virtual void removeFavorite(AddleId id) = 0;

    //virtual const std::list<AddleId>& recent() = 0;
    
signals:
    virtual void selectionChanged(QList<AddleId> selection) = 0;
    virtual void assetsChanged(QList<AddleId> ids) = 0;
    virtual void favoritesChanged(QList<AddleId> favorites) = 0;
};

ADDLE_DECL_MAKEABLE(IAssetSelectionPresenter);

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::IAssetSelectionPresenter, "org.addle.IAssetSelectionPresenter")

#endif // IASSETSELECTIONPRESENTER_HPP
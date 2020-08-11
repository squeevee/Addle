#ifndef IASSETSELECTIONPRESENTER_HPP
#define IASSETSELECTIONPRESENTER_HPP

#include "interfaces/traits.hpp"


#include "../assets/iassetpresenter.hpp"

#include <QList>
namespace Addle {

class IAssetSelectionPresenter
{
public:
    virtual ~IAssetSelectionPresenter() = default;

    virtual void initialize(QList<QSharedPointer<IAssetPresenter>> assets, bool canMultiSelect = true) = 0;

    virtual PersistentId selectedId() = 0;
    virtual QList<PersistentId> selectedIds() = 0;

    virtual QSharedPointer<IAssetPresenter> selectedPresenter() = 0;
    virtual QList<QSharedPointer<IAssetPresenter>> selectedPresenters() = 0;

    virtual bool canMultiSelect() = 0;

    virtual void select(PersistentId assetId) = 0;
    virtual void select(QList<PersistentId> assetIds) = 0;

    virtual void select(QSharedPointer<IAssetPresenter> asset) = 0;
    virtual void select(QList<QSharedPointer<IAssetPresenter>> assets) = 0;

    virtual QList<PersistentId> ids() = 0;

    virtual QList<QSharedPointer<IAssetPresenter>> assets() = 0;
    virtual void setAssets(QList<QSharedPointer<IAssetPresenter>> assets) = 0;

    virtual QSharedPointer<IAssetPresenter> assetById(PersistentId id) = 0;

    virtual QList<PersistentId> favorites() = 0;
    virtual void setFavorites(QList<PersistentId> favorites) = 0;

    virtual void addFavorite(PersistentId id) = 0;
    virtual void removeFavorite(PersistentId id) = 0;

    //virtual const std::list<PersistentId>& recent() = 0;
    
signals:
    virtual void selectionChanged(QList<PersistentId> selection) = 0;
    virtual void assetsChanged(QList<PersistentId> ids) = 0;
    virtual void favoritesChanged(QList<PersistentId> favorites) = 0;
};

DECL_MAKEABLE(IAssetSelectionPresenter);
DECL_EXPECTS_INITIALIZE(IAssetSelectionPresenter);
DECL_IMPLEMENTED_AS_QOBJECT(IAssetSelectionPresenter);

} // namespace Addle
#endif // IASSETSELECTIONPRESENTER_HPP
#ifndef IASSETTOOLPRESENTER_HPP
#define IASSETTOOLPRESENTER_HPP

#include <QObject>
#include <QList>
#include <QSharedPointer>

#include "itoolpresenter.hpp"
#include "../assets/iassetpresenter.hpp"

#include "interfaces/traits/initialize_traits.hpp"
#include "interfaces/traits/qobject_trait.hpp"

class IToolWithAssetsPresenter : public virtual IToolPresenter
{
public:
    virtual ~IToolWithAssetsPresenter() = default;

    virtual QList<QSharedPointer<IAssetPresenter>> getAllAssets() = 0;
    virtual QSharedPointer<IAssetPresenter> getAssetPresenter(PersistentId id) = 0;

    virtual PersistentId getSelectedAsset() = 0;
    virtual void selectAsset(PersistentId id) = 0;
    virtual QSharedPointer<IAssetPresenter> getSelectedAssetPresenter() = 0;

signals:
    virtual void selectedAssetChanged(PersistentId id) = 0;
};

DECL_IMPLEMENTED_AS_QOBJECT(IToolWithAssetsPresenter)

#endif // IASSETTOOLPRESENTER_HPP
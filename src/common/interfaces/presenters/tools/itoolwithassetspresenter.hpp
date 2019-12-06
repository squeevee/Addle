#ifndef IASSETTOOLPRESENTER_HPP
#define IASSETTOOLPRESENTER_HPP

#include <QObject>
#include <QList>

#include "itoolpresenter.hpp"
#include "../assets/iassetsetpresenter.hpp"
#include "../assets/iassetpresenter.hpp"

#include "interfaces/traits/initialize_traits.hpp"
#include "interfaces/traits/qobject_trait.hpp"

class IToolWithAssetsPresenter : public virtual IToolPresenter
{
public:
    virtual ~IToolWithAssetsPresenter() = default;

    virtual IAssetSetPresenter* getAssetSet() = 0;

    virtual QList<IAssetPresenter*> getAllAssets() = 0;
    virtual QList<IAssetPresenter*> getSelectedAssets() = 0;
    virtual IAssetPresenter* getSelectedAsset() = 0;
    virtual void setSelectedAssets(QList<IAssetPresenter*> assets) = 0;
    virtual void setSelectedAsset(IAssetPresenter* asset) = 0;

    virtual bool canMultiSelect() = 0;

signals:
    virtual void selectedAssetsChanged(QList<IAssetPresenter*> assets) = 0;
};

DECL_IMPLEMENTED_AS_QOBJECT(IToolWithAssetsPresenter)

#endif // IASSETTOOLPRESENTER_HPP
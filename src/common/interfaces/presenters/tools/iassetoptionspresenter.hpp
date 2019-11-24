#ifndef IASSETOPTIONSPRESENTER_HPP
#define IASSETOPTIONSPRESENTER_HPP

#include <QObject>
#include <QList>

#include "itooloptionspresenter.hpp"
#include "../assets/iassetsetpresenter.hpp"
#include "../assets/iassetpresenter.hpp"

#include "common/interfaces/traits/initialize_traits.hpp"
#include "common/interfaces/traits/qobject_trait.hpp"
class IAssetOptionsPresenter : public virtual IToolOptionsPresenter
{
public:
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

DECL_IMPLEMENTED_AS_QOBJECT(IAssetOptionsPresenter)

#endif // IASSETOPTIONSPRESENTER_HPP
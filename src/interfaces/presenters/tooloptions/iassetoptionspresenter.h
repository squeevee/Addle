#ifndef IASSETOPTIONSPRESENTER_H
#define IASSETOPTIONSPRESENTER_H

#include <QObject>
#include <QList>

#include "itooloptionspresenter.h"
#include "../assets/iassetsetpresenter.h"
#include "../assets/iassetpresenter.h"

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

#endif //IASSETOPTIONSPRESENTER_H
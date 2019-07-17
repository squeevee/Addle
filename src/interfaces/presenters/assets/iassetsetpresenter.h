#ifndef IASSETSETPRESENTER_H
#define IASSETSETPRESENTER_H

#include <QList>

#include "iassetpresenter.h"

class IAssetSetPresenter
{
    virtual QList<IAssetPresenter*> getAssets() = 0;
    
};

#endif //IASSETSETPRESENTER_H
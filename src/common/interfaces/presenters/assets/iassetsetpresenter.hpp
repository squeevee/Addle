#ifndef IASSETSETPRESENTER_HPP
#define IASSETSETPRESENTER_HPP

#include <QList>

#include "iassetpresenter.hpp"

class IAssetSetPresenter
{
public:
    virtual QList<IAssetPresenter*> getAssets() = 0;
    
};

#endif // IASSETSETPRESENTER_HPP
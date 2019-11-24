#ifndef IASSETPRESENTER_HPP
#define IASSETPRESENTER_HPP

#include <QString>

class IAssetSetPresenter;
class IAssetPresenter
{
public:
    virtual IAssetSetPresenter* getAssetSet() = 0;

    virtual QString getName() = 0;
};

#endif // IASSETPRESENTER_HPP
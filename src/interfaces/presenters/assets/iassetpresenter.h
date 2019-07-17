#ifndef IASSETPRESENTER_H
#define IASSETPRESENTER_H

class IAssetSetPresenter;
class IAssetPresenter
{
public:
    virtual IAssetSetPresenter* getAssetSet() = 0;

    virtual QString getName() = 0;
};

#endif //IASSETPRESENTER_H
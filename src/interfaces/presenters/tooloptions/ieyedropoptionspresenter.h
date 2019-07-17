#ifndef IEYEDROPOPTIONSPRESENTER_H
#define IEYEDROPOPTIONSPRESENTER_H

#include "itooloptionspresenter.h"

class IEyedropOptionsPresenter : public virtual IToolOptionsPresenter
{
public:
    //NECESSARY?
    enum SampleOptions
    {
        allLayers,
        currentLayer
    };

    virtual SampleOptions getSample() = 0;
    virtual void setSample(SampleOptions sample) = 0;

signals:
    virtual void sampleChanged(SampleOptions sample) = 0;
};

Q_DECLARE_METATYPE(IEyedropOptionsPresenter::SampleOptions)

#endif //IEYEDROPOPTIONSPRESENTER_H
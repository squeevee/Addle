#ifndef IEYEDROPOPTIONSPRESENTER_HPP
#define IEYEDROPOPTIONSPRESENTER_HPP

#include "itooloptionspresenter.hpp"
#include "common/interfaces/traits/initialize_traits.hpp"
#include "common/interfaces/traits/qobject_trait.hpp"

#include "common/interfaces/servicelocator/imakeable.hpp"
class IEyedropOptionsPresenter : public virtual IToolOptionsPresenter, public virtual IMakeable
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

DECL_IMPLEMENTED_AS_QOBJECT(IEyedropOptionsPresenter)

#endif // IEYEDROPOPTIONSPRESENTER_HPP
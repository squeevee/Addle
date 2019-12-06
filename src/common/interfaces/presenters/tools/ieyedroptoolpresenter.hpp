#ifndef IEYEDROPTOOLPRESENTER_HPP
#define IEYEDROPTOOLPRESENTER_HPP

#include "itoolpresenter.hpp"
#include "interfaces/traits/initialize_traits.hpp"
#include "interfaces/traits/qobject_trait.hpp"

#include "interfaces/servicelocator/imakeable.hpp"
class IEyedropToolPresenter : public virtual IToolPresenter, public virtual IMakeable
{
public:
    //NECESSARY?
    enum SampleOptions
    {
        allLayers,
        currentLayer
    };

    static const ToolId EYEDROP_TOOL_ID;

    virtual ~IEyedropToolPresenter() = default;

    virtual SampleOptions getSample() = 0;
    virtual void setSample(SampleOptions sample) = 0;

signals:
    virtual void sampleChanged(SampleOptions sample) = 0;
};
Q_DECLARE_METATYPE(IEyedropToolPresenter::SampleOptions)

DECL_IMPLEMENTED_AS_QOBJECT(IEyedropToolPresenter)

#endif // IEYEDROPTOOLPRESENTER_HPP
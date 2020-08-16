#ifndef IEYEDROPTOOLPRESENTER_HPP
#define IEYEDROPTOOLPRESENTER_HPP

#include "itoolpresenter.hpp"
#include "interfaces/traits.hpp"

namespace Addle {


class IEyedropToolPresenter : public virtual IToolPresenter
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

    virtual SampleOptions sample() = 0;
    virtual void setSample(SampleOptions sample) = 0;

signals:
    virtual void sampleChanged(SampleOptions sample) = 0;
};
//Q_DECLARE_METATYPE(IEyedropToolPresenter::SampleOptions)

DECL_MAKEABLE(IEyedropToolPresenter)
//DECL_IMPLEMENTED_AS_QOBJECT(IEyedropToolPresenter)

} // namespace Addle
#endif // IEYEDROPTOOLPRESENTER_HPP
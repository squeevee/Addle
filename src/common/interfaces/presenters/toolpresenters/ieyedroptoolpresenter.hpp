#ifndef IEYEDROPTOOLPRESENTER_HPP
#define IEYEDROPTOOLPRESENTER_HPP

#include "interfaces/traits/compat.hpp"
#include "itoolpresenter.hpp"
#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"

#include "interfaces/traits/makeable_trait.hpp"
class ADDLE_COMMON_EXPORT IEyedropToolPresenter : public virtual IToolPresenter
{
public:
    //NECESSARY?
    enum SampleOptions
    {
        allLayers,
        currentLayer
    };

    static const ToolId ID;

    virtual ~IEyedropToolPresenter() = default;

    virtual SampleOptions getSample() = 0;
    virtual void setSample(SampleOptions sample) = 0;

signals:
    virtual void sampleChanged(SampleOptions sample) = 0;
};
Q_DECLARE_METATYPE(IEyedropToolPresenter::SampleOptions)

DECL_MAKEABLE(IEyedropToolPresenter)
DECL_IMPLEMENTED_AS_QOBJECT(IEyedropToolPresenter)

#endif // IEYEDROPTOOLPRESENTER_HPP
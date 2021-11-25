/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

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

//     static const ToolId EYEDROP_TOOL_ID;

    virtual ~IEyedropToolPresenter() = default;

    virtual SampleOptions sample() = 0;
    virtual void setSample(SampleOptions sample) = 0;

signals:
    virtual void sampleChanged(SampleOptions sample) = 0;
};


ADDLE_DECL_MAKEABLE(IEyedropToolPresenter)


} // namespace Addle
#endif // IEYEDROPTOOLPRESENTER_HPP

/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IMEASURETOOLPRESENTER_HPP
#define IMEASURETOOLPRESENTER_HPP

#include "itoolpresenter.hpp"
#include "interfaces/traits.hpp"

namespace Addle {


class IMeasureToolPresenter : public virtual IToolPresenter
{
public:
    enum MeasureOperationOptions {
        line,
        rectangle,
        path,
        angle
    };

//     static const ToolId MEASURE_TOOL_ID;

    virtual ~IMeasureToolPresenter() = default;

    virtual MeasureOperationOptions measureOperation() = 0;
    virtual void setMeasureOperation(MeasureOperationOptions operation) = 0;

signals:
    virtual void measureOperationChanged(MeasureOperationOptions operation) = 0;
};



ADDLE_DECL_MAKEABLE(IMeasureToolPresenter)


} // namespace Addle
#endif // IMEASURETOOLPRESENTER_HPP

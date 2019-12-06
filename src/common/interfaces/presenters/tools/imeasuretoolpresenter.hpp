#ifndef IMEASURETOOLPRESENTER_HPP
#define IMEASURETOOLPRESENTER_HPP

#include "itoolpresenter.hpp"
#include "interfaces/traits/initialize_traits.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/servicelocator/imakeable.hpp"

class IMeasureToolPresenter : public virtual IToolPresenter, public virtual IMakeable
{
public:
    enum MeasureOperationOptions {
        line,
        rectangle,
        path,
        angle
    };

    static const ToolId MEASURE_TOOL_ID;

    virtual ~IMeasureToolPresenter() = default;

    virtual MeasureOperationOptions getMeasureOperation() = 0;
    virtual void setMeasureOperation(MeasureOperationOptions operation) = 0;

signals:
    virtual void measureOperationChanged(MeasureOperationOptions operation) = 0;
};

Q_DECLARE_METATYPE(IMeasureToolPresenter::MeasureOperationOptions)

DECL_IMPLEMENTED_AS_QOBJECT(IMeasureToolPresenter)

#endif // IMEASURETOOLPRESENTER_HPP
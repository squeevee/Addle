#ifndef IMEASURETOOLPRESENTER_HPP
#define IMEASURETOOLPRESENTER_HPP

#include "interfaces/traits/compat.hpp"
#include "itoolpresenter.hpp"
#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"

class ADDLE_COMMON_EXPORT IMeasureToolPresenter : public virtual IToolPresenter
{
public:
    enum MeasureOperationOptions {
        line,
        rectangle,
        path,
        angle
    };

    static const ToolId ID;

    virtual ~IMeasureToolPresenter() = default;

    virtual MeasureOperationOptions getMeasureOperation() = 0;
    virtual void setMeasureOperation(MeasureOperationOptions operation) = 0;

signals:
    virtual void measureOperationChanged(MeasureOperationOptions operation) = 0;
};

Q_DECLARE_METATYPE(IMeasureToolPresenter::MeasureOperationOptions)

DECL_MAKEABLE(IMeasureToolPresenter)
DECL_IMPLEMENTED_AS_QOBJECT(IMeasureToolPresenter)

#endif // IMEASURETOOLPRESENTER_HPP
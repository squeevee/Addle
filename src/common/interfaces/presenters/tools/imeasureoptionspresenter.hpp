#ifndef IMEASUREOPTIONSPRESENTER_HPP
#define IMEASUREOPTIONSPRESENTER_HPP

#include "itooloptionspresenter.hpp"
#include "common/interfaces/traits/initialize_traits.hpp"
#include "common/interfaces/traits/qobject_trait.hpp"
#include "common/interfaces/servicelocator/imakeable.hpp"

class IMeasureOptionsPresenter : public virtual IToolOptionsPresenter, public virtual IMakeable
{
public:
    enum MeasureOperationOptions {
        line,
        rectangle,
        path,
        angle
    };

    virtual MeasureOperationOptions getMeasureOperation() = 0;
    virtual void setMeasureOperation(MeasureOperationOptions operation) = 0;

signals:
    virtual void measureOperationChanged(MeasureOperationOptions operation) = 0;
};

Q_DECLARE_METATYPE(IMeasureOptionsPresenter::MeasureOperationOptions)

DECL_IMPLEMENTED_AS_QOBJECT(IMeasureOptionsPresenter)

#endif // IMEASUREOPTIONSPRESENTER_HPP
#ifndef IMEASUREOPTIONSPRESENTER_H
#define IMEASUREOPTIONSPRESENTER_H

#include "itooloptionspresenter.h"

class IMeasureOptionsPresenter : public virtual IToolOptionsPresenter
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

#endif //IMEASUREOPTIONSPRESENTER_H
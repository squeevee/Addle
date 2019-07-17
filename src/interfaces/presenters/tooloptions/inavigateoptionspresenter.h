#ifndef INAVIGATEOPTIONSPRESENTER_H
#define INAVIGATEOPTIONSPRESENTER_H

#include "itooloptionspresenter.h"

class INavigateOptionsPresenter : public virtual IToolOptionsPresenter
{
public:
    enum NavigateOperationOptions {
        gripPan,
        gripZoom,
        gripRotate,
        rectangleZoomTo
    };

    virtual NavigateOperationOptions getNavigateOperation() = 0;
    virtual void setNavigateOperation(NavigateOperationOptions operation) = 0;

signals:
    virtual void navigateOperationChanged(NavigateOperationOptions operation) = 0;
};

Q_DECLARE_METATYPE(INavigateOptionsPresenter::NavigateOperationOptions)

#endif //INAVIGATEOPTIONSPRESENTER_H
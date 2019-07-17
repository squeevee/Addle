#ifndef ISELECTOPTIONSPRESENTER_H
#define ISELECTOPTIONSPRESENTER_H

#include "isimilarregionoptionspresenter.h"

class ISelectOptionsPresenter : public virtual ISimilarRegionOptionsPresenter
{
public:
    enum SelectStyleOptions {
        //User draws a rectangle on the canvas, the interior region of which
        //forms the selection.
        rectangle, 

        //User draws an ellipse on the canvas, the interior region of which
        //forms the selection.
        ellipse, 

        //User draws an arbitrary path on the canvas, the interior region of
        //which forms the selection.
        lasso, 

        //User sets a series of points on the canvas, which are connected by
        //straight lines, the interior region of which forms the selection.
        //NECESSARY?
        polygon, 

        //User picks a point on the canvas, and a region is selected by similar
        magic
    };

    enum SelectionOperationOptions {
        scale,
        rotate,
        distort
    };

    virtual SelectStyleOptions getSelectStyle() = 0;
    virtual void setSelectStyle(SelectStyleOptions selectStyle) = 0;

    virtual SelectionOperationOptions getSelectionOperation() = 0;
    virtual void setSelectionOperation(SelectionOperationOptions operation) = 0;

public slots:
    virtual void crop() = 0;
    virtual void preciseTransform() = 0;

signals:
    virtual void selectStyleChanged(SelectStyleOptions selectStyle) = 0;
    virtual void selectionOperationChanged(SelectionOperationOptions operation) = 0;
};

Q_DECLARE_METATYPE(ISelectOptionsPresenter::SelectStyleOptions)
Q_DECLARE_METATYPE(ISelectOptionsPresenter::SelectionOperationOptions)

#endif //ISELECTOPTIONSPRESENTER_H
#ifndef ISELECTTOOLPRESENTER_HPP
#define ISELECTTOOLPRESENTER_HPP

#include "interfaces/traits.hpp"


namespace Addle {

class ISelectToolPresenter
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

        //User picks a point on the canvas, and a region is selected by similar
        magic
    };

    enum SelectionOperationOptions {
        scale,
        rotate,
        distort
    };

    static const ToolId SELECT_TOOL_ID;

    virtual ~ISelectToolPresenter() = default;

    virtual SelectStyleOptions selectStyle() = 0;
    virtual void setSelectStyle(SelectStyleOptions selectStyle) = 0;

    virtual SelectionOperationOptions selectionOperation() = 0;
    virtual void setSelectionOperation(SelectionOperationOptions operation) = 0;

public slots:
    virtual void crop() = 0;
    virtual void preciseTransform() = 0;

signals:
    virtual void selectStyleChanged(SelectStyleOptions selectStyle) = 0;
    virtual void selectionOperationChanged(SelectionOperationOptions operation) = 0;
};

//Q_DECLARE_METATYPE(ISelectToolPresenter::SelectStyleOptions)
//Q_DECLARE_METATYPE(ISelectToolPresenter::SelectionOperationOptions)

DECL_MAKEABLE(ISelectToolPresenter)
DECL_IMPLEMENTED_AS_QOBJECT(ISelectToolPresenter)

} // namespace Addle
#endif // ISELECTTOOLPRESENTER_HPP
#ifndef ISELECTTOOLPRESENTER_HPP
#define ISELECTTOOLPRESENTER_HPP

#include "isimilarregiontoolpresenter.hpp"
#include "interfaces/traits/initialize_traits.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/servicelocator/imakeable.hpp"

class ISelectToolPresenter : public virtual ISimilarRegionToolPresenter, public virtual IMakeable
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

Q_DECLARE_METATYPE(ISelectToolPresenter::SelectStyleOptions)
Q_DECLARE_METATYPE(ISelectToolPresenter::SelectionOperationOptions)

DECL_IMPLEMENTED_AS_QOBJECT(ISelectToolPresenter)

#endif // ISELECTTOOLPRESENTER_HPP
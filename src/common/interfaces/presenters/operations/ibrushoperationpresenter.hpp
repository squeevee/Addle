#ifndef IBRUSHOPERATIONPRESENTER_HPP
#define IBRUSHOPERATIONPRESENTER_HPP

#include "iundooperationpresenter.hpp"

#include "interfaces/traits.hpp"


class BrushStroke;
class ILayerPresenter;
class IBrushOperationPresenter : public IUndoOperationPresenter
{
public:
    virtual ~IBrushOperationPresenter() = default;

    virtual void initialize(
        QWeakPointer<BrushStroke> painter,
        QWeakPointer<ILayerPresenter> layer
    ) = 0;

    //
};

DECL_EXPECTS_INITIALIZE(IBrushOperationPresenter);
DECL_MAKEABLE(IBrushOperationPresenter);

#endif // IBRUSHOPERATIONPRESENTER_HPP
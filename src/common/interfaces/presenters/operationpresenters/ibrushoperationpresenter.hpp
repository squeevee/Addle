#ifndef IBRUSHOPERATIONPRESENTER_HPP
#define IBRUSHOPERATIONPRESENTER_HPP

#include "iundooperationpresenter.hpp"

#include "utilities/canvas/brushpainterdata.hpp"

#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"

class IRenderStep;
class ILayerPresenter;
class IBrushPresenter;
class IBrushOperationPresenter : public IUndoOperationPresenter
{
public:
    virtual ~IBrushOperationPresenter() = default;

    virtual void initialize(
        QWeakPointer<ILayerPresenter> layer,
        QSharedPointer<IBrushPresenter> brush
    ) = 0;

    virtual void addPainterData(BrushPainterData data) = 0;

    virtual QSharedPointer<IRenderStep> getPreview() = 0;
};

DECL_EXPECTS_INITIALIZE(IBrushOperationPresenter);
DECL_MAKEABLE(IBrushOperationPresenter);

#endif // IBRUSHOPERATIONPRESENTER_HPP
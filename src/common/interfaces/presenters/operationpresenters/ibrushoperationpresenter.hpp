#ifndef IBRUSHOPERATIONPRESENTER_HPP
#define IBRUSHOPERATIONPRESENTER_HPP

#include "iundooperationpresenter.hpp"

#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"

class IRenderStep;
class ILayerPresenter;
class IBrushPresenter;
class IBrushPainter;
class IBrushOperationPresenter : public IUndoOperationPresenter
{
public:
    virtual ~IBrushOperationPresenter() = default;

    virtual void initialize(
        QWeakPointer<ILayerPresenter> layer,
        QSharedPointer<IBrushPresenter> brush
    ) = 0;

    virtual QWeakPointer<IBrushPainter> getBrushPainter() = 0;
    virtual QSharedPointer<IRenderStep> getPreview() = 0;
};

DECL_EXPECTS_INITIALIZE(IBrushOperationPresenter);
DECL_MAKEABLE(IBrushOperationPresenter);

#endif // IBRUSHOPERATIONPRESENTER_HPP
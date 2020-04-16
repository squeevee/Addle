#ifndef IBRUSHOPERATIONPRESENTER_HPP
#define IBRUSHOPERATIONPRESENTER_HPP

#include "iundooperationpresenter.hpp"

#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/traits/compat.hpp"

class IBrushPainter;
class ILayerPresenter;
class ADDLE_COMMON_EXPORT IBrushOperationPresenter : public IUndoOperationPresenter
{
public:
    virtual ~IBrushOperationPresenter() = default;

    virtual void initialize(
        QWeakPointer<IBrushPainter> painter,
        QWeakPointer<ILayerPresenter> layer
    ) = 0;

    //
};

DECL_EXPECTS_INITIALIZE(IBrushOperationPresenter);
DECL_MAKEABLE(IBrushOperationPresenter);

#endif // IBRUSHOPERATIONPRESENTER_HPP
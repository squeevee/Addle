/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IBRUSHOPERATIONPRESENTER_HPP
#define IBRUSHOPERATIONPRESENTER_HPP

#include "iundooperationpresenter.hpp"

#include "interfaces/traits.hpp"

namespace Addle {

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

ADDLE_DECL_MAKEABLE(IBrushOperationPresenter);

} // namespace Addle
#endif // IBRUSHOPERATIONPRESENTER_HPP
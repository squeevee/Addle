/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IBRUSHTOOLPRESENTER_HPP
#define IBRUSHTOOLPRESENTER_HPP

#include "compat.hpp"

#include "globals.hpp"
#include "itoolpresenter.hpp"

#include "idtypes/brushid.hpp"
#include "interfaces/iamqobject.hpp"

#include "interfaces/traits.hpp"


#include "interfaces/metaobjectinfo.hpp"
namespace Addle {

class IMainEditorPresenter;
class IAssetSelectionPresenter;
class IBrushPresenter;
class ICanvasPresenter;
class IViewPortPresenter;
class IColorSelectionPresenter;

class IBrushToolPresenter : public virtual IToolPresenter, public virtual IAmQObject
{
public:
    INTERFACE_META(IBrushToolPresenter)

    enum Mode
    {
        Brush,
        Eraser
    };

    virtual ~IBrushToolPresenter() = default;

    virtual void initialize(IMainEditorPresenter* owner, Mode mode) = 0;

    virtual IAssetSelectionPresenter& brushSelection() = 0;

    virtual void selectBrush(BrushId id) = 0;
    virtual BrushId selectedBrush() = 0;

    virtual QSharedPointer<IBrushPresenter> selectedBrushPresenter() = 0;
signals:
    virtual void brushChanged(BrushId brush) = 0;
    virtual void refreshPreviews() = 0;
};

DECL_INTERFACE_META_PROPERTIES(IBrushToolPresenter,
    DECL_INTERFACE_PROPERTY(selectedBrush)
)
ADDLE_DECL_MAKEABLE(IBrushToolPresenter)


} // namespace Addle

Q_DECLARE_INTERFACE(Addle::IBrushToolPresenter, "org.addle.IBrushToolPresenter");

#endif // IBRUSHTOOLPRESENTER_HPP
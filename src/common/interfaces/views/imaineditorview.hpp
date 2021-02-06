/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IMAINEDITORVIEW_HPP
#define IMAINEDITORVIEW_HPP

#include "iviewbase.hpp"
#include "itoplevelview.hpp"

#include "interfaces/traits.hpp"

namespace Addle {

class IMainEditorPresenter;
class IMainEditorView : public IViewBase<IMainEditorPresenter>, public ITopLevelView
{
public:
    using PresenterType = IMainEditorPresenter;
    
    virtual ~IMainEditorView() = default;
};

ADDLE_DECL_MAKEABLE(IMainEditorView)

namespace aux_IMainEditorView {
    ADDLE_FACTORY_PARAMETER_ALIAS(aux_view, presenter)
}

ADDLE_VIEW_FACTORY_PARAMETERS(IMainEditorView)

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::IMainEditorView, "org.addle.IMainEditorView")

#endif // IMAINEDITORVIEW_HPP

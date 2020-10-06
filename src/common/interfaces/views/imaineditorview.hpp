/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IMAINEDITORVIEW_HPP
#define IMAINEDITORVIEW_HPP

#include "iviewfor.hpp"
#include "itoplevelview.hpp"

#include "interfaces/traits.hpp"

namespace Addle {

class IMainEditorPresenter;
class IMainEditorView : public IViewFor<IMainEditorPresenter>, public ITopLevelView
{
public:
    typedef IMainEditorPresenter PresenterType;
    
    virtual ~IMainEditorView() = default;
};

DECL_MAKEABLE(IMainEditorView)

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::IMainEditorView, "org.addle.IMainEditorView")

#endif // IMAINEDITORVIEW_HPP

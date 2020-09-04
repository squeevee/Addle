/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IMAINEDITORVIEW_HPP
#define IMAINEDITORVIEW_HPP

#include "itoplevelview.hpp"

#include "interfaces/traits.hpp"

namespace Addle {

class IMainEditorPresenter;
class IMainEditorView : public ITopLevelView
{
public:
    virtual ~IMainEditorView() = default;

    virtual void initialize(IMainEditorPresenter& presenter) = 0;

    virtual IMainEditorPresenter& presenter() const = 0;
};

DECL_MAKEABLE(IMainEditorView)


} // namespace Addle

Q_DECLARE_INTERFACE(Addle::IMainEditorView, "org.addle.IMainEditorView")

#endif // IMAINEDITORVIEW_HPP
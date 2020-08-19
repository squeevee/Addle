/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef ITEXTTOOLPRESENTER_HPP
#define ITEXTTOOLPRESENTER_HPP

#include "itoolpresenter.hpp"
#include "interfaces/traits.hpp"


namespace Addle {

class ITextToolPresenter : public virtual IToolPresenter
{
public:
    static const ToolId TEXT_TOOL_ID;

    virtual ~ITextToolPresenter() = default;
};

DECL_MAKEABLE(ITextToolPresenter)


} // namespace Addle

#endif // ITEXTTOOLPRESENTER_HPP
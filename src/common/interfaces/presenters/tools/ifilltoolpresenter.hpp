/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IFILLTOOLPRESENTER_HPP
#define IFILLTOOLPRESENTER_HPP

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"


namespace Addle {


class IFillToolPresenter
{
public:
//     static const ToolId FILL_TOOL_ID;

    virtual ~IFillToolPresenter() = default;
};

ADDLE_DECL_MAKEABLE(IFillToolPresenter)


} // namespace Addle
#endif // IFILLTOOLPRESENTER_HPP

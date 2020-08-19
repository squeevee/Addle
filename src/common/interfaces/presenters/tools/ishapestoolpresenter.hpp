/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef ISHAPESTOOLPRESENTER_HPP
#define ISHAPESTOOLPRESENTER_HPP

#include <QList>

#include "../assets/ishapepresenter.hpp"
#include "itoolpresenter.hpp"
#include "interfaces/traits.hpp"


namespace Addle {

class IShapesToolPresenter : public virtual IToolPresenter
{
public:
    static const ToolId SHAPES_TOOL_ID;

    virtual ~IShapesToolPresenter() = default;
};

DECL_MAKEABLE(IShapesToolPresenter)


} // namespace Addle
#endif // ISHAPESTOOLPRESENTER_HPP
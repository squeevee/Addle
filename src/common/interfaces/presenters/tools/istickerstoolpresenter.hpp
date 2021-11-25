/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef ISTICKERSTOOLPRESENTER_HPP
#define ISTICKERSTOOLPRESENTER_HPP

#include <QList>

#include "../assets/istickerpresenter.hpp"
#include "interfaces/traits.hpp"


namespace Addle {

class IStickersToolPresenter
{
public:
//     static const ToolId STICKERS_TOOL_ID;

    virtual ~IStickersToolPresenter() = default;
};

ADDLE_DECL_MAKEABLE(IStickersToolPresenter)


} // namespace Addle
#endif // ISTICKERSTOOLPRESENTER_HPP

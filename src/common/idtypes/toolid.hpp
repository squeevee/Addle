/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef TOOLID_HPP
#define TOOLID_HPP

#include "addleid.hpp"
namespace Addle {

class ADDLE_COMMON_EXPORT ToolId : public AddleId
{
    ID_TYPE_BOILERPLATE(ToolId)
};

} // namespace Addle

Q_DECLARE_METATYPE(Addle::ToolId)
Q_DECLARE_TYPEINFO(Addle::ToolId, Q_PRIMITIVE_TYPE);

ID_TYPE_BOILERPLATE2(ToolId)

#endif // TOOLID_HPP
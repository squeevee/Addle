/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License.
 * See "LICENSE" for full details.
 */

#ifndef MODULEID_HPP
#define MODULEID_HPP

#include "addleid.hpp"
namespace Addle {

class ModuleId final : public AddleId
{
    ID_TYPE_BOILERPLATE(ModuleId)
};

} // namespace Addle

Q_DECLARE_METATYPE(Addle::ModuleId);
Q_DECLARE_TYPEINFO(Addle::ModuleId, Q_PRIMITIVE_TYPE);

ID_TYPE_BOILERPLATE2(ModuleId)

#endif // MODULEID_HPP

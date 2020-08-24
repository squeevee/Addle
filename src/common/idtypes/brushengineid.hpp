/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef BRUSHENGINEID_HPP
#define BRUSHENGINEID_HPP

#include "addleid.hpp"
namespace Addle {

class ADDLE_COMMON_EXPORT BrushEngineId : public AddleId
{
    ID_TYPE_BOILERPLATE(BrushEngineId)
};

} // namespace Addle

Q_DECLARE_METATYPE(Addle::BrushEngineId);
Q_DECLARE_TYPEINFO(Addle::BrushEngineId, Q_PRIMITIVE_TYPE);
ID_TYPE_BOILERPLATE2(BrushEngineId);

#endif // BRUSHENGINEID_HPP
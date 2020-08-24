/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef BRUSHID_HPP
#define BRUSHID_HPP

#include "addleid.hpp"
#include "brushengineid.hpp"

namespace Addle {

class ADDLE_COMMON_EXPORT BrushId : public AddleId
{
    ID_TYPE_BOILERPLATE(BrushId)
};

} // namespace Addle

Q_DECLARE_METATYPE(Addle::BrushId)
Q_DECLARE_TYPEINFO(Addle::BrushId, Q_PRIMITIVE_TYPE);
ID_TYPE_BOILERPLATE2(BrushId);

#endif // BRUSHID_HPP
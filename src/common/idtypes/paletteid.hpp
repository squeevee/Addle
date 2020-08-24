/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef PALETTEID_HPP
#define PALETTEID_HPP

#include "addleid.hpp"
namespace Addle {

class PaletteId : public AddleId
{
    ID_TYPE_BOILERPLATE(PaletteId)
};

} // namespace Addle

Q_DECLARE_METATYPE(Addle::PaletteId);
Q_DECLARE_TYPEINFO(Addle::PaletteId, Q_PRIMITIVE_TYPE);

ID_TYPE_BOILERPLATE2(PaletteId)

#endif // PALETTEID_HPP
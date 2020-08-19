/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef PALETTEID_HPP
#define PALETTEID_HPP

#include "persistentid.hpp"
namespace Addle {

class PaletteId : public PersistentId
{
    PERSISTENT_ID_BOILERPLATE(PaletteId)
public:
    PaletteId(const char* key)
        : PaletteId(key, {})
    {
    }
};

} // namespace Addle

Q_DECLARE_METATYPE(Addle::PaletteId);

#endif // PALETTEID_HPP
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
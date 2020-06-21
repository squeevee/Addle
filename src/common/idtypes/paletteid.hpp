#ifndef PALETTEID_HPP
#define PALETTEID_HPP

#include "persistentid.hpp"

class PaletteId : public PersistentId
{
    PERSISTENT_ID_BOILERPLATE(PaletteId)
public:
    PaletteId(const char* key)
        : PaletteId(key, {})
    {
    }
};

Q_DECLARE_METATYPE(PaletteId);

#endif // PALETTEID_HPP
#ifndef BRUSHENGINEID_HPP
#define BRUSHENGINEID_HPP

#include "persistentid.hpp"

class ADDLE_COMMON_EXPORT BrushEngineId : public PersistentId
{
    PERSISTENT_ID_BOILERPLATE(BrushEngineId)
public:
    BrushEngineId(const char* key)
        : BrushEngineId(key, QHash<QString, QVariant>())
    {
    }
};
Q_DECLARE_METATYPE(BrushEngineId)

#endif // BRUSHENGINEID_HPP
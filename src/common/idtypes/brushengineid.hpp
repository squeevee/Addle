#ifndef BRUSHENGINEID_HPP
#define BRUSHENGINEID_HPP

#include "persistentid.hpp"
namespace Addle {

class ADDLE_COMMON_EXPORT BrushEngineId : public PersistentId
{
    PERSISTENT_ID_BOILERPLATE(BrushEngineId)
public:
    BrushEngineId(const char* key)
        : BrushEngineId(key, QHash<QString, QVariant>())
    {
    }
};

} // namespace Addle

Q_DECLARE_METATYPE(Addle::BrushEngineId);

#endif // BRUSHENGINEID_HPP
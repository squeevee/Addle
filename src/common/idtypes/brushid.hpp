#ifndef BRUSHID_HPP
#define BRUSHID_HPP

#include "persistentid.hpp"
#include "brushengineid.hpp"

class ADDLE_COMMON_EXPORT BrushId : public PersistentId
{
    PERSISTENT_ID_BOILERPLATE(BrushId)
public:
    BrushId(const char* key)
        : BrushId(key, {})
    {
    }
};
Q_DECLARE_METATYPE(BrushId)

#endif // BRUSHID_HPP
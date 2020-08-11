#ifndef BRUSHID_HPP
#define BRUSHID_HPP

#include "persistentid.hpp"
#include "brushengineid.hpp"

namespace Addle {

class ADDLE_COMMON_EXPORT BrushId : public PersistentId
{
    PERSISTENT_ID_BOILERPLATE(BrushId)
public:
    BrushId(const char* key)
        : BrushId(key, {})
    {
    }
};

} // namespace Addle

Q_DECLARE_METATYPE(Addle::BrushId)

#endif // BRUSHID_HPP
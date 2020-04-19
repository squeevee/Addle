#ifndef BRUSHID_HPP
#define BRUSHID_HPP

#include "persistentid.hpp"

class BrushId : public PersistentId
{
    PERSISTENT_ID_BOILERPLATE(BrushId)
public:
    BrushId(const char* key)
        : BrushId(key, QHash<QString, QVariant>())
    {
    }
};
Q_DECLARE_METATYPE(BrushId)

#endif // BRUSHID_HPP
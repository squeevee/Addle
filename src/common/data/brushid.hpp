#ifndef BRUSHID_HPP
#define BRUSHID_HPP

#include "persistentid.hpp"

class BrushId : public PersistentId
{
public: 
    BrushId() = default;
    BrushId(const char* key)
        : PersistentId(key)
    {
    }
    BrushId(const PersistentId& other)
        : PersistentId(other)
    {
    }
};
Q_DECLARE_METATYPE(BrushId)

#endif // BRUSHID_HPP
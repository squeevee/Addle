#ifndef TOOLID_HPP
#define TOOLID_HPP

#include "persistentid.hpp"

class ToolId : public PersistentId
{
public:
    ToolId() = default;
    ToolId(const char* key)
        : PersistentId(key)
    {
    }
};
Q_DECLARE_METATYPE(ToolId)

#endif // TOOLID_HPP
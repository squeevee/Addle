#ifndef TOOLID_HPP
#define TOOLID_HPP

#include "persistentid.hpp"

class ADDLE_COMMON_EXPORT ToolId : public PersistentId
{
    PERSISTENT_ID_BOILERPLATE(ToolId)
public:
    ToolId(const char* key)
        : ToolId(key, QHash<QString, QVariant>())
    {
    }
};
Q_DECLARE_METATYPE(ToolId)

#endif // TOOLID_HPP
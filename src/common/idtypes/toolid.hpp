#ifndef TOOLID_HPP
#define TOOLID_HPP

#include "persistentid.hpp"
namespace Addle {

class ADDLE_COMMON_EXPORT ToolId : public PersistentId
{
    PERSISTENT_ID_BOILERPLATE(ToolId)
public:
    ToolId(const char* key)
        : ToolId(key, QHash<QString, QVariant>())
    {
    }
};

} // namespace Addle

Q_DECLARE_METATYPE(Addle::ToolId)

#endif // TOOLID_HPP
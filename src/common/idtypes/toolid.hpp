/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

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
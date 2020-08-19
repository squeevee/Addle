/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

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
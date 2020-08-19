/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

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
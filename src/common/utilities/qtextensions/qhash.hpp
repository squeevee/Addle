/**
 * Addle source code
 * @file qhash.h
 * @copyright Copyright 2019 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 * 
 * Declares overloads of uint qHash(...), to extend the types usable as keys in
 * QHash and QSet
 */

#ifndef qtextensions__QHASH_HPP
#define qtextensions__QHASH_HPP

#include "compat.hpp"
#include <typeindex>
#include <QHash>

#include "../typeinforef.hpp"
#include "../indexvariant.hpp"
#include "idtypes/persistentid.hpp"

namespace std
{
    inline uint qHash(const std::type_index& key, uint seed = 0) { return key.hash_code() ^ seed; }
}
#endif //qtextensions__QHASH_HPP
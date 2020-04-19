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

#include <typeindex>
#include <QHash>

#include "../typeinforef.hpp"
#include "../indexvariant.hpp"
#include "idtypes/persistentid.hpp"

namespace std
{
    uint qHash(const std::type_index& key, uint seed = 0);
}
uint qHash(const PersistentId& id, uint seed = 0);

#endif //qtextensions__QHASH_HPP
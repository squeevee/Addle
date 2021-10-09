/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#pragma once 

#include <QHash> // qHash(...)
#include <QUuid>

#include <functional> // std::hash

#include <typeindex>

// Although the hash-based containers provided by Qt and STL are ostensibly 
// quite similar, the fact that Qt's are implicitly shared proves to be both
// an asset and a liability depending on the situation -- e.g., as a parameter 
// in a signal or other exposed data with ambiguous ownership and/or thread 
// affinity, a QHash or QSet is almost certainly preferable; but as a private
// mutable cache, fast copying is unlikely to be desired, and the muddier
// constness semantics make accidental detaching much more likely so 
// std::unordered_set or std::unordered_map are probably better choices.
//
// Ultimately it is (not very difficult) worthwhile to support both hash API's

namespace std {

// `std::hash`es for Qt types
    
template<> struct hash<QUuid>
{
    std::size_t operator()(const QUuid id) const { return qHash(id); }
};

// `qHash`es for STL types
    
inline uint qHash(const std::type_index& key, uint seed = 0) { return ::qHash(key.hash_code(), seed); }

}

/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef HASHFUNCTIONS_HPP
#define HASHFUNCTIONS_HPP

#include <QHash>

#include <typeindex>

#include <boost/functional/hash.hpp>
#include <boost/variant.hpp>

namespace std
{
    inline uint qHash(const std::type_index& key, uint seed = 0)
	{ 
		return ::qHash(key.hash_code(), seed); 
	}
}

#endif // HASHFUNCTIONS_HPP
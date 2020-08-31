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

// namespace boost //needed?
// {
//     class QHashVisitor
//     {
//     public:
//         QHashVisitor(uint seed = 0)
//             : _seed(seed)
//         {
//         }

//         template<typename T>
//         inline uint operator()(const T& x) { return qHash(x, _seed); }
//     private:
//         uint _seed;
//     };

//     template<typename T1, typename T2>
//     inline uint qHash(const variant<T1, T2>& v, uint seed = 0) { return apply_visitor(QHashVisitor(seed), v); }

//     // Could be extended for more parameters

//     template<typename T>
//     inline uint qHash(const recursive_wrapper<T>& w, uint seed = 0) { return qHash(w.get(), seed); }
// }

#endif // HASHFUNCTIONS_HPP
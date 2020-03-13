#include "qhash.hpp"

uint std::qHash(const std::type_index& key, uint seed ) { return key.hash_code() ^ seed; }
uint qHash(const PersistentId& id, uint seed) { return qHash(id._data, seed); }
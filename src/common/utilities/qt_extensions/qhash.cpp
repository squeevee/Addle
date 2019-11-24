#include "qhash.hpp"

uint std::qHash(const std::type_index& key) { return key.hash_code(); }

uint qHash(const FormatId& format) { return qHash(format._data); }
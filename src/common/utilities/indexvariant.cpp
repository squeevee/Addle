/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "indexvariant.hpp"
#include "errors.hpp"

using namespace Addle;

//Initialize with hashers for some basic types
QHash<int, uint(*)(QVariant, uint)> IndexVariant::_hashers = {
    { QMetaType::Int, &hasher<int> },
    { QMetaType::UInt, &hasher<uint> },
    { QMetaType::QString, &hasher<QString> },
    { QMetaType::QByteArray, &hasher<QByteArray> }
};

uint IndexVariant::hash(uint seed) const
{
    if (_var.isNull())
        return 0; //?
    
    int type = _var.userType();

    // If a hasher is explicitly registered for a type, that is preferred.
    if (_hashers.contains(type))
    {
        uint(*hasher)(QVariant, uint) = _hashers.value(type);
        uint result = hasher(_var, seed);
        return result;
    }

    if (_var.canConvert<int>()) return hasher<int>(_var, seed);
    if (_var.canConvert<uint>()) return hasher<uint>(_var, seed);

    if (_var.canConvert<qint64>()) return hasher<qint64>(_var, seed);
    if (_var.canConvert<quint64>()) return hasher<quint64>(_var, seed);

    if (_var.canConvert<double>()) return hasher<double>(_var, seed);
    if (_var.canConvert<QString>()) return hasher<QString>(_var, seed);

    ADDLE_LOGICALLY_UNREACHABLE();
}

bool IndexVariant::variantCanHash(const QVariant& var)
{
    return _hashers.contains(var.userType())
        || var.canConvert<qint64>() // implies canConvert<int>()
        || var.canConvert<quint64>() // implies canConvert<uint>()
        || var.canConvert<double>()
        || var.canConvert<QString>();
}

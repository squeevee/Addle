#ifndef QMETA_HPP
#define QMETA_HPP

#include <QByteArray>
#include <QMetaEnum>
namespace Addle {

template <typename EnumType>
inline bool enumKeyIsValid(const QByteArray& key)
{
    //todo static assert
    QMetaEnum meta = QMetaEnum::fromType<EnumType>();
    
    const int count = meta.keyCount();
    for (int i = 0; i < count; ++i)
    {
        if (key == meta.key(i)) return true;
    }
    return false;
}

template <typename EnumType>
inline const char* enumKeyFromValue(const EnumType& value)
{
    //todo static assert
    QMetaEnum meta = QMetaEnum::fromType<EnumType>();

    const int count = meta.keyCount();
    for (int i = 0; i < count; ++i)
    {
        if (static_cast<int>(value) == meta.value(i)) return meta.key(i);
    }
    return nullptr;
}

template<typename EnumType>
inline EnumType enumValueFromKey(const QByteArray& key)
{
    //todo static assert
    QMetaEnum meta = QMetaEnum::fromType<EnumType>();
    
    const int count = meta.keyCount();
    for (int i = 0; i < count; ++i)
    {
        if (key == meta.key(i)) return static_cast<EnumType>(meta.value(i));
    }
    return static_cast<EnumType>(NULL);
}
} // namespace Addle

#endif // QMETA_HPP
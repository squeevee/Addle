#ifndef QMETA_HPP
#define QMETA_HPP

#include <QByteArray>
#include <QMetaEnum>
namespace Addle {

template <typename EnumType>
    // TODO: std::enable_if
inline bool isValidKey(const QByteArray& key)
{
    QMetaEnum meta = QMetaEnum::fromType<EnumType>();
    
    for (int i = 0; i < meta.keyCount(); i++)
    {
        if (key == meta.key(i)) return true;
    }
    return false;
}

template<typename EnumType>
    // TODO: std::enable_if
inline EnumType valueFromKey(const QByteArray& key)
{
    QMetaEnum meta = QMetaEnum::fromType<EnumType>();
    
    for (int i = 0; i < meta.keyCount(); i++)
    {
        if (key == meta.key(i)) return static_cast<EnumType>(meta.value(i));
    }
    return static_cast<EnumType>(NULL);
}
} // namespace Addle

#endif // QMETA_HPP
#ifndef PROPERTYCACHE_HPP
#define PROPERTYCACHE_HPP

#include <functional>
#include <type_traits>
#include <QVector>

/**
 * @class PropertyCache
 * @brief Automatically caches a dynamically calculated property, calling a
 * notify signal when its value changes.
 * 
 * 
 */
template<typename OwnerType, typename PropertyType>
class PropertyCache
{
public:
    PropertyCache(
            OwnerType& owner,
            PropertyType (OwnerType::*calculate)(),
            void (OwnerType::*notify)(PropertyType) = nullptr,
            QVector<std::function<void()>> propagate = QVector<std::function<void()>>()
        )
        : _owner(owner),
        _calculate(calculate),
        _notify(notify),
        _propagate(propagate)
    {
    }

    inline void onChange(QVector<std::function<void()>> propagate)
    {
        _propagate.append(propagate);
    }
    
    inline void initialize(PropertyType value)
    {
        _value = value;
        _initialized = true;
    }

    inline const PropertyType& getValue()
    {
        if (!_initialized) initialize((_owner.*_calculate)());
        return _value;
    }
    
    inline void recalculate()
    {
        PropertyType newValue = (_owner.*_calculate)();
        if (_value != newValue)
        {
            _value = newValue;
            if (_notify)
                (_owner.*_notify)(_value);
                
            for (std::function<void()> func : _propagate)
                func();
        }
        _initialized = true;
    }
    
private:
    bool _initialized = false;
    PropertyType _value;

    OwnerType& _owner;

    PropertyType (OwnerType::*_calculate)();
    void (OwnerType::*_notify)(PropertyType);
    QVector<std::function<void()>> _propagate;
};

#define _BIND(obj, member) \
    std::bind( &std::remove_reference<decltype(obj)>::type::member, obj )

#endif // PROPERTYCACHE_HPP
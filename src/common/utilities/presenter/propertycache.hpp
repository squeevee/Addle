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
template<typename PropertyType>
class PropertyCache
{
public:
    PropertyCache(std::function<PropertyType()> calculate = nullptr)
        : _calculate(calculate)
    {
    }

    PropertyCache(const PropertyCache<PropertyType>&) = delete;

    inline void calculateBy(std::function<PropertyType()> calculate)
    {
        _calculate = calculate;
    }

    template<typename OwnerType>
    inline void calculateBy(PropertyType (OwnerType::*calculate)(), OwnerType* owner)
    {
        _calculate = std::bind(calculate, owner);
    }

    inline void onChange(std::function<void()> onChange)
    {
        _onChange_0.append(onChange);
    }

    inline void onChange(std::function<void(PropertyType)> onChange)
    {
        _onChange_1.append(onChange);
    }

    template<class OwnerType>
    inline void onChange(void (OwnerType::*onChange)(), OwnerType* owner)
    {
        _onChange_0.append(std::bind(onChange, owner));
    }

    template<class OwnerType>
    inline void onChange(void (OwnerType::*onChange)(PropertyType), OwnerType* owner)
    {
        _onChange_1.append(std::bind(onChange, owner, std::placeholders::_1));
    }

    template<typename T>
    inline void onChange_Recalculate(PropertyCache<T>& other)
    {
        _onChange_0.append(std::bind(&PropertyCache<T>::recalculate, &other));
    }
    
    inline void initialize(PropertyType value)
    {
        _value = value;
        _initialized = true;
    }

    inline const PropertyType& getValue()
    {
        if (!_initialized && _calculate) initialize(_calculate());
        return _value;
    }
    
    inline void recalculate()
    {
        if (!_calculate) return;

        PropertyType newValue = _calculate();
        if (_value != newValue)
        {
            _value = newValue;
            for (std::function<void(PropertyType)> func : _onChange_1)
                func(_value);
                
            for (std::function<void()> func : _onChange_0)
                func();
        }
        _initialized = true;
    }

private:
    bool _initialized = false;
    PropertyType _value;

    std::function<PropertyType()> _calculate;

    QVector<std::function<void()>> _onChange_0;
    QVector<std::function<void(PropertyType)>> _onChange_1;
};

#endif // PROPERTYCACHE_HPP
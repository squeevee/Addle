/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef PROPERTYCACHE_HPP
#define PROPERTYCACHE_HPP

#include <functional>
#include <type_traits>
#include <QVector>

#include "../helpercallback.hpp"
namespace Addle {

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

    template<typename OwnerType>
    PropertyCache(PropertyType (OwnerType::*member)(), OwnerType* owner)
        : _calculate(std::bind(member, owner))
    {
    }

    template<typename OwnerType>
    PropertyCache(PropertyType (OwnerType::*member)() const, const OwnerType* owner)
        : _calculate(std::bind(member, owner))
    {
    }

    PropertyCache(const PropertyCache<PropertyType>&) = delete;

    inline void calculateBy(std::function<PropertyType()> calculate)
    {
        _calculate = calculate;
    }

    template<typename OwnerType>
    inline void calculateBy(PropertyType (OwnerType::*member)(), OwnerType* owner)
    {
        _calculate = std::bind(member, owner);
    }

    template<typename OwnerType>
    inline void calculateBy(PropertyType (OwnerType::*member)() const, const OwnerType* owner)
    {
        _calculate = std::bind(member, owner);
    }

    HelperArgCallback<PropertyType> onChange;

    template<typename T>
    inline void onChange_Recalculate(PropertyCache<T>& other)
    {
        onChange.bind(&PropertyCache<T>::recalculate, &other);
    }
    
    inline void initialize(PropertyType value)
    {
        if (_initialized) return;

        _value = value;
        _initialized = true;
    }

    inline const PropertyType& value() const
    {
        if (!_initialized && _calculate)
        {
            _value = _calculate();
            _initialized = true;
        }
        return _value;
    }
    
    inline void recalculate()
    {
        if (!_calculate) return;

        PropertyType newValue = _calculate();
        if (_value != newValue)
        {
            _value = newValue;
            onChange(_value);
        }
        _initialized = true;
    }

private:
    mutable bool _initialized = false;
    mutable PropertyType _value;

    std::function<PropertyType()> _calculate;
};
} // namespace Addle

#endif // PROPERTYCACHE_HPP
/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef LAZYVALUE_HPP
#define LAZYVALUE_HPP

#include <functional>
#include <type_traits>
#include <QVector>

#include "helpercallback.hpp"
namespace Addle {

/**
 * @class LazyValue
 * @brief Lazily calculated value
 */
template<typename T>
class LazyValue
{
public:
    LazyValue(std::function<T()> calculate = nullptr)
        : _calculate(calculate)
    {
    }
    
    inline void calculateBy(std::function<T()> calculate)
    {
        _calculate = calculate;
    }

    inline void initialize(T value)
    {
        if (_initialized) return;

        _value = value;
        _initialized = true;
    }

    inline const T& value() const
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

        _value = _calculate();
        _initialized = true;
    }

private:
    mutable bool _initialized = false;
    mutable T _value;

    std::function<T()> _calculate;
};

template<typename T>
class LazyProperty
{
public:
    LazyProperty(std::function<T()> calculate = nullptr)
        : _calculate(calculate)
    {
    }

    template<typename OwnerType>
    LazyProperty(T (OwnerType::*member)(), OwnerType* owner)
        : _calculate(std::bind(member, owner))
    {
    }

    template<typename OwnerType>
    LazyProperty(T (OwnerType::*member)() const, const OwnerType* owner)
        : _calculate(std::bind(member, owner))
    {
    }

    LazyProperty(const LazyProperty<T>&) = delete;

    inline void calculateBy(std::function<T()> calculate)
    {
        _calculate = calculate;
    }

    template<typename OwnerType>
    inline void calculateBy(T (OwnerType::*member)(), OwnerType* owner)
    {
        _calculate = std::bind(member, owner);
    }

    template<typename OwnerType>
    inline void calculateBy(T (OwnerType::*member)() const, const OwnerType* owner)
    {
        _calculate = std::bind(member, owner);
    }

    HelperArgCallback<T> onChange;

    template<typename T_>
    inline void onChange_Recalculate(LazyProperty<T_>& other)
    {
        onChange.bind(&LazyProperty<T_>::recalculate, &other);
    }
    
    inline void initialize(T value)
    {
        if (_initialized) return;

        _value = value;
        _initialized = true;
    }

    inline const T& value() const
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

        T newValue = _calculate();
        if (_value != newValue)
        {
            _value = newValue;
            onChange(_value);
        }
        _initialized = true;
    }

private:
    mutable bool _initialized = false;
    mutable T _value;

    std::function<T()> _calculate;
};

} // namespace Addle

#endif // LAZYVALUE_HPP

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

#include <optional>

#include "helpercallback.hpp"
namespace Addle {

// TODO: these utilities are used for two largely unrelated cases. LazyValue is
// used fairly sparingly in situations that actively necessitate type erasure or 
// implementation hiding, and is essentially fine as is.
    
// LazyProperty is used to simplify the pattern of an object caching the values
// of its own properties when changes may trigger expensive updates. The 
// implementation leans on runtime abstraction and type erasure, but this is a 
// convenience: it is not necessary for the functionality of the utility. A more
// appropriate design would not lean on type erasure and would avoid
// persistently storing a `this` pointer -- this would require a certain amount 
// of boiler-plating that may ultimately undermine the purpose of the utility 
// altogether.
//
// HOWEVER, a more complete answer to the problem that LazyProperty is meant to 
// address would actually be a state machine, likely the one provided by Qt.
// This would provide the same advantages as caching, with a lot of additional
// ones, and would likely be operated in a similar way to the current
// LazyProperty implementation. So, fixing LazyProperty is not a priority over
// that.
    
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
    
    bool isInitialized() const { return _initialized; }

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
    
    inline void clear()
    {
        _initialized = false;
    }

private:
    mutable bool _initialized = false;
    mutable T _value;

    std::function<T()> _calculate;
};

} // namespace Addle

#endif // LAZYVALUE_HPP

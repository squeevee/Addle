/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef HELPERCALLBACK_HPP
#define HELPERCALLBACK_HPP

#include <QList>
#include <functional>
namespace Addle {

/**
 * Minimalist observer-style callback handles to be exposed by helper and
 * utility classes and subscribed-to by their owners. This is a convenience to
 * allow both helpers and owners to be more agnostic and flexible.
 * 
 * In typical usage, HelperCallback is owned by value in a ...Helper class, 
 * which in turn is owned by value in a (often ...Presenter) interface-
 * implementing class. Any instance of the owning class has a longer lifespan
 * than the callback object, and thus for a helper object `_spiffHelper` with
 * a HelperCallback member `onSpiff`, it is safe for the owner to bind its own
 * member functions:
 * 
 * ```c++
 * _spiffHelper.onSpiff += std::bind(&SpiffPresenter::onSpiff, this);
 * ```
 * 
 * No means of unsubscribing is given, so avoid in cases of uncertain relative
 * lifetimes. This also introduces a degree of overhead that may be inappropriate
 * for some cases.
 * 
 * Note: this is by means an alternative to Qt signals/slots, which should be
 * used for any kind of trans-interface communication or indeed any case where
 * a more robust observer is desired.
 */

class HelperCallback
{
public:
    HelperCallback() = default;
    HelperCallback(const HelperCallback&) = delete;

    HelperCallback& operator=(const HelperCallback&) = delete;

    inline HelperCallback& operator+=(std::function<void()> callback)
    {
        _callbacks.append(callback);
        return *this;
    }

    inline HelperCallback& operator+=(HelperCallback& other)
    {
        _callbacks.append([&](){ other(); });
        return *this;
    }

    template<typename OwnerType>
    inline HelperCallback& bind(void (OwnerType::*member)(), OwnerType* owner)
    {
        _callbacks.append(std::bind(member, owner));
        return *this;
    }

    inline void operator()() const 
    {
        for (auto c : _callbacks)
        {
            c();
        }
    }

private:
    QList<std::function<void()>> _callbacks;
};

template<typename ArgType>
class HelperArgCallback
{
public:
    HelperArgCallback() = default;
    HelperArgCallback(const HelperArgCallback&) = delete;

    HelperArgCallback& operator=(const HelperArgCallback&) = delete;

    inline HelperArgCallback& operator+=(std::function<void()> callback)
    {
        _callbacks.append(callback);
        return *this;
    }
	
    inline HelperArgCallback& operator+=(std::function<void(ArgType)> callback)
    {
        _callbacksArg.append(callback);
        return *this;
    }

    inline HelperArgCallback& operator+=(HelperCallback& other)
    {
        _callbacks.append([&](){ other(); });
        return *this;
    }

    inline HelperArgCallback& operator+=(HelperArgCallback& other)
    {
#ifndef Q_CC_MSVC
        _callbacksArg.append(std::bind(&HelperArgCallback::operator(), &other, std::placeholders::_1));
#else
        _callbacksArg.append([&](ArgType arg){ other(arg); });
#endif
        return *this;
    }

    template<typename OwnerType>
    inline HelperArgCallback& bind(void (OwnerType::*member)(), OwnerType* owner)
    {
        _callbacks.append(std::bind(member, owner));
        return *this;
    }

    template<typename OwnerType>
    inline HelperArgCallback& bind(void (OwnerType::*member)() const, const OwnerType* owner)
    {
        _callbacks.append(std::bind(member, owner));
        return *this;
    }

    template<typename OwnerType>
    inline HelperArgCallback& bind(void (OwnerType::*member)(ArgType), OwnerType* owner)
    {
#ifndef Q_CC_MSVC
        _callbacksArg.append(std::bind(member, owner, std::placeholders::_1));
#else
    // Workaround for https://developercommunity.visualstudio.com/content/problem/317991/stdis-convertible-is-broken-for-stdbind-functors.html
        _callbacksArg.append([member, owner](ArgType arg){ emit (owner->*member)(arg); });
#endif
        return *this;
    }

    template<typename OwnerType>
    inline HelperArgCallback& bind(void (OwnerType::*member)(ArgType) const, const OwnerType* owner)
    {
#ifndef Q_CC_MSVC
        _callbacksArg.append(std::bind(member, owner, std::placeholders::_1));
#else
        _callbacksArg.append([member, owner](ArgType arg){ emit (owner->*member)(arg); });
#endif
        return *this;
    }
    
    inline void operator()(ArgType arg) const 
    {
        for (auto c : _callbacks)
        {
            c();
        }
        for (auto c : _callbacksArg)
        {
            c(arg);
        }
    }

private:
    QList<std::function<void()>> _callbacks;
    QList<std::function<void(ArgType)>> _callbacksArg;
};
} // namespace Addle

#endif // HELPERCALLBACK_HPP
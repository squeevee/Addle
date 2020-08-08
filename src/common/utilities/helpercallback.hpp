#ifndef HELPERCALLBACK_HPP
#define HELPERCALLBACK_HPP

#include <QList>
#include <functional>

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

template<typename... ArgTypes>
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
	
    inline HelperArgCallback& operator+=(std::function<void(ArgTypes... args)> callback)
    {
        _callbacksArgs.append(callback);
        return *this;
    }

    inline void operator()(ArgTypes... args) const 
    {
        for (auto c : _callbacks)
        {
            c();
        }
        for (auto c : _callbacksArgs)
        {
            c(args...);
        }
    }

private:
    QList<std::function<void()>> _callbacks;
    QList<std::function<void(ArgTypes... args)>> _callbacksArgs;
};

#endif // HELPERCALLBACK_HPP
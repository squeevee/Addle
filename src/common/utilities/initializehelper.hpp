#ifndef INITIALIZEHELPER_HPP
#define INITIALIZEHELPER_HPP

#include <QObject>
#include <typeinfo>
#include <type_traits>

#include <cstdint> // for INT_MAX

#include "../exceptions/initializeexceptions.hpp"

// TODO: this class needs a lot of work.
// 1) There's no reason for it to be a template, that just kind of
// vaguely inconveniences the process of writing implementation classes without
// adding any genuinely useful diagnostic info: it's easy enough to get a stack
// trace when a problem actually occurs.
// 2) It might be better for this class to do *nothing* in release builds.

// In general I think I anticipated initialization to be more of an issue than
// it seems like it's gonna be. Frankly, if utility classes like this are as
// simplistic as possible, that will mean more for the maintenance of Addle so
// the behavior can be exhaustively validated by unit tests *once* and the issue
// be put to rest for the rest of the project's lifetime.

/**
 * A helper class for objects that expect initialization. Tracks whether the
 * owner class is initialized and performs assertions as called upon.
 */

#ifdef ADDLE_DEBUG
template<class OwnerType>
const char* _get_owner_typename(std::true_type)
{
    return OwnerType::staticMetaObject.className();
}

template<class OwnerType>
const char* _get_owner_typename(std::false_type)
{
    return typeid(OwnerType).name();
}
#endif

template<class OwnerType>
class InitializeHelper
{
public:
    InitializeHelper(OwnerType* owner = nullptr)
        : _isInitialized(false),
          _depth(0)
#ifdef ADDLE_DEBUG
         , _ownerTypeName(_get_owner_typename<OwnerType>(std::is_base_of<QObject, OwnerType>()))
#endif
    {
    }
    inline bool isInitialized() { return _isInitialized; }

    // The `checkpoint` argument permits classes to selectively make some
    // resources accessible before initialization is complete. This can be useful
    // for example when a class creates an owned object during initialization
    // that itself needs information from its owner (saving the trouble of
    // `initialize` functions with long and esoteric argument lists)
    //
    // This could lead to fiddly behavioral gotchas not explicit to an interface
    // but when you pick at the edge cases, that seems hard to avoid, and this
    // is probably one of the more graceful solutions.
    inline void check(int checkpoint = INT_MAX) const
    {
        //potential threading hazard // meh
        if (_depth > 0 && checkpoint > _checkpoint)
        {
#ifdef ADDLE_DEBUG
            InvalidInitializeException ex(
                InvalidInitializeException::Why::improper_order,
                _ownerTypeName
            );
#else
            InvalidInitializeException ex(
                InvalidInitializeException::Why::improper_order
            );
#endif
            ADDLE_THROW(ex);
        }

        if (!_isInitialized && checkpoint > _checkpoint)
        {
#ifdef ADDLE_DEBUG
            NotInitializedException ex(
                _ownerTypeName
            );
#else
            NotInitializedException ex;
#endif
            ADDLE_THROW(ex);
        }
    }

    // Checkpoint values are arbitrary and internal to a class (and its
    // descendants if any) 
    inline void setCheckpoint(int checkpoint)
    {
        _checkpoint = checkpoint;
    }

    // will anyone ever use this function for any reason?
    inline void assertNotInitialized()
    {
        if (_isInitialized)
        {
#ifdef ADDLE_DEBUG
            AlreadyInitializedException ex(
                _ownerTypeName
            );
#else
            AlreadyInitializedException ex;
#endif
            ADDLE_THROW(ex);
        }
    }

    //Call at the beginning of the initialize function block, before calling
    //any parent class's initialize function.
    inline void initializeBegin()
    {
        assertNotInitialized();
        _depth++;
    }

    //Call at the end of the initialize function block, after calling any
    //parent class's initialize function.
    inline void initializeEnd()
    {
        _depth--;

        if (_depth == 0)
            _isInitialized = true;
        else if (_depth < 0)
        {
#ifdef ADDLE_DEBUG
            InvalidInitializeException ex(
                InvalidInitializeException::Why::improper_order
                    , _ownerTypeName
            );
#else
            InvalidInitializeException ex(
                InvalidInitializeException::Why::improper_order
            );
#endif
            ADDLE_THROW(ex);
        }
    }

private:
    bool _isInitialized;
    int _depth;
    int _checkpoint = -1;
#ifdef ADDLE_DEBUG
    const char* _ownerTypeName;
#endif
};

#endif // INITIALIZEHELPER_HPP
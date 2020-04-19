#ifndef INITIALIZEHELPER_HPP
#define INITIALIZEHELPER_HPP

#include <QObject>
#include <typeinfo>
#include <type_traits>

#include "../exceptions/initializeexceptions.hpp"

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
          _initializingLevel(0)
#ifdef ADDLE_DEBUG
         , _ownerTypeName(_get_owner_typename<OwnerType>(std::is_base_of<QObject, OwnerType>()))
#endif
    {
    }
    inline bool isInitialized() { return _isInitialized; }

    inline void check() const
    {
        //potential threading hazard
        if (_initializingLevel > 0)
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

        if (!_isInitialized)
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
        _initializingLevel++;
    }

    //Call at the end of the initialize function block, after calling any
    //parent class's initialize function.
    inline void initializeEnd()
    {
        _initializingLevel--;

        if (_initializingLevel == 0)
            _isInitialized = true;
        else if (_initializingLevel < 0)
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
    int _initializingLevel;
#ifdef ADDLE_DEBUG
    const char* _ownerTypeName;
#endif
};

#endif // INITIALIZEHELPER_HPP
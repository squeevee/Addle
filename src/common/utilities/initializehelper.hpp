#ifndef INITIALIZEHELPER_HPP
#define INITIALIZEHELPER_HPP

#include <QObject>
#include <typeinfo>
#include <type_traits>

#include <cstdint> // for INT_MAX

#include "utilities/errors.hpp"

#include "../exceptions/initializeexceptions.hpp"

// TODO: It might be better for this class to do *nothing* in release builds.
// depends on whether we want objects to be uninitialized on purpose and use
// exceptions to test for this state.

// If uninitialized objects aren't supported then we will want to switch these
// from named runtime errors to generic logic errors

/**
 * A helper class for objects that expect initialization. Tracks whether the
 * owner class is initialized and performs assertions as called upon.
 */
namespace Addle {

class InitializeHelper
{
public:
    InitializeHelper()
        : _isInitialized(false),
          _depth(0)
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
            ADDLE_THROW(InvalidInitializeException(
                InvalidInitializeException::Why::improper_order
            ));

        if (!_isInitialized && checkpoint > _checkpoint)
            ADDLE_THROW(NotInitializedException());
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
            ADDLE_THROW(AlreadyInitializedException());
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
            ADDLE_THROW(InvalidInitializeException(InvalidInitializeException::Why::improper_order));
    }

private:
    bool _isInitialized;
    int _depth;
    int _checkpoint = -1;
};

class Initializer
{
public:
    Initializer(InitializeHelper& helper)
        : _helper(helper)
    {
        _helper.initializeBegin();
    }

    Initializer(const Initializer&) = delete;

    ~Initializer()
    {
        _helper.initializeEnd();
    }

private:
    InitializeHelper& _helper;
};

} // namespace Addle
#endif // INITIALIZEHELPER_HPP
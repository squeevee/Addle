#ifndef INITIALIZEHELPER_H
#define INITIALIZEHELPER_H

#include <QObject>

#include "./exceptions/initializeexceptions.h"

/**
 * A helper class for objects that expect initialization. Tracks whether the
 * owner class is initialized and performs assertions as called upon.
 */
class InitializeHelper
{
public:
    InitializeHelper(QObject* owner = nullptr)
        : _isInitialized(false),
          _initializingLevel(0),
         _owner(owner) {}
    inline bool isInitialized() { return _isInitialized; }

    inline void assertInitialized()
    {
        if (!_isInitialized)
            throw NotInitializedException(_owner);
    }

    inline void assertNotInitialized()
    {
        if (_isInitialized)
            throw AlreadyInitializedException(_owner);
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
            throw InvalidInitializeException(
                    "initializeEnd() called without matching initializeBegin()"
                );
    }

private:
    bool _isInitialized;
    int _initializingLevel;
    QObject* _owner;
};

#endif //INITIALIZEHELPER_H
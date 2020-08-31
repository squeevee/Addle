# Error handling in Addle

`AddleException` is the base class used to model and handle errors that occur
during runtime in Addle. It is derived from `std::exception`, with some
additional functionality to make it more useful in a multi-threaded application.
Namely:

- `raise()` - Throws the exception as its most-derived polymorphic type.
- `clone()` - Creates a copy of the exception as its most-derived polymorphic
type.

These allow an exception to be caught on one thread, thrown on another, and 
caught again as its original type.

The macro `ADDLE_THROW` should be used to throw AddleException rather than 
calling raise() directly -- this populates the exception with debugging
information on debug builds.

Derivatives of `AddleException` are broadly grouped into Runtime Errors and
Logic Errors (though these don't necessarily have to be exclusive). Exceptions
modeling Runtime Errors should have properties that verbosely describe and
could possibly be used by the software to correct the problem or inform further 
action. Logic Errors are assumed to be unrecoverable (they indicate unsound
code), and at most need to contain human-readable information.

As a convenience, the macros `ADDLE_ASSERT`, `ADDLE_ASSERT_M`, and 
`ADDLE_LOGIC_ERROR_M` are given to enforce integrity checks at runtime without 
needing to create bespoke exception classes. These should be favored over other
asserts, particularly ones that `abort()` on failure so Addle can handle errors
more gracefully.

`ASSERT_INIT` is another convenience macro that specifically runs
`_initHelper.check()`, (but with more debug info than would be available by the
plain call).

## Qt events

For various technical reasons, Qt does not support any exceptions entering 
QEventLoop. Qt slots, invokables, and other event handlers must catch all 
exceptions that happen within them. 

Addle provides the `ADDLE_SLOT_CATCH` and `ADDLE_EVENT_CATCH` macros as a
general answer to this. For example:

```c++
#include "utilities/errors.hpp"

void Spiff::freemSlot()
{
    try
    {
        ASSERT_INIT;

        // ...
    }
    ADDLE_SLOT_CATCH;
}
```

If while `freemSlot()` is handling a signal, an exception occurs -- for example 
`ASSERT_INIT` fails -- then that exception is reported to `IErrorService` which
notifies the user of an unexpected error. (In the future, it should also take 
precautionary or even corrective action if possible.) 

If the exception occurs when `freemSlot()` has been called as a function, the
exception will propagate to the caller, where it could potentially be caught
and handled synchronously (which would always be preferable to asynchronously
reporting it out of context).

The macro `ADDLE_EVENT_CATCH` provides similar functionality but will never 
propagate the error to the caller.

## Debugging features (debug builds only)

An `AddleException` keeps a record of all the locations from which it was 
thrown via `ADDLE_THROW`. Additionally, if Addle was built with glibc, the 
exception keeps a stack trace of the first location from which it was thrown. 
`AddleException` overrides `std::exception::what()` and displays all this 
information for compatibility with debugging tools.

The environment variable `ADDLE_DEBUG_BEHAVIOR` can be set to control various
global debug behavior flags related to error handling. The flags are defined
in "src/common/utilities/debugging/debugbehavior_p.hpp".

As an example, if Addle is run with `ADDLE_DEBUG_BEHAVIOR=0x104`, this sets the
flags `AbortOnUnhandledLogicError` and `InterceptQObjectConnectFailure`. If
a call to `QObject::connect` doesn't work, it will be intercepted and reported
to `IErrorService` as an unhandled logic error, which will then call `abort()`
(useful when running in a debugger).
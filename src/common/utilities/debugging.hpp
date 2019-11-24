#ifndef DEBUGGING_HPP
#define DEBUGGING_HPP

#ifndef ADDLE_DEBUG
#error "You should only include this file for debug builds."
#endif

#include <QFlags>
#include "common/globalconstants.hpp"
#include "common/interfaces/traits/qobject_trait.hpp"

void debugMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message);

// By default, Addle should contain and handle exceptions quietly, recovering
// where possible, using the UI to notify the user of errors, and not allowing
// exceptions to propagate to Qt or the system.
//
// This can, however, make debugging more of a challenge, especially when
// threading and asynchronous events are involved. The environment variable
// ADDLE_DEBUG_BEHAVIOR can be set to an (base 10) int representing a
// combination of these flags, which will cause an abort() signal at critical
// moments, presumably to be caught by the debugger with the stack intact. 
//
// It should go without saying, this does nothing to production builds.
class DebugBehavior
{
public:
    enum DebugBehaviorOptions
    {
        normal = 0x0,

        //Abort if an error occurs during startup
        abort_on_startup_error = 0x1,

        //reserved

        //Abort when a task results in an error.
        abort_on_task_error = 0x4,

        //Abort whenever an "error-ish" Qt message is logged.
        //not advisable due to QXcbConnection errors
        //maybe see some other way to break on invalid QObject::connect calls
        abort_on_errorish_qtlog = 0x40,

        //Abort any time a logic error is thrown
        abort_on_logic_error = 0x80

    };
    Q_DECLARE_FLAGS(DebugBehaviorFlags, DebugBehaviorOptions)

    static DebugBehaviorFlags& get();

    //Programmatically override the value from the environment
    static void set(DebugBehaviorFlags flags) { _instantiated = true; _instance = flags; }

    inline static bool test(DebugBehaviorOptions option) { return get().testFlag(option); }

private:
    static bool _instantiated;
    static DebugBehaviorFlags _instance;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DebugBehavior::DebugBehaviorFlags)

template<class Interface>
const char* getQObjectClassName(Interface* object)
{
    static_assert(
        implemented_as_QObject<Interface>::value,
        "This function can only be used on interfaces implementing QObject"
    );

    QObject* q_object = dynamic_cast<QObject*>(object);

    if (q_object)
    {
        return q_object->metaObject()->className();
    }
    else
    {
        return GlobalConstants::DEBUG__NONE_TYPE_NAME;
    }
}

#endif // DEBUGGING_HPP
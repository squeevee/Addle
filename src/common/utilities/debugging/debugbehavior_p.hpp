/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef DEBUGBEHAVIOR_P_HPP
#define DEBUGBEHAVIOR_P_HPP

#ifndef ADDLE_DEBUG
// Because of MOC issues, we can't just wrap the whole file in #ifdef ADDLE_DEBUG
#error "This file should only be included in debug builds"
#endif

#include "compat.hpp"

#include <QtDebug>
#include <QFlags>

namespace Addle {

/**
 * Programmatic accessor for special error handling behavior flags (set in the 
 * ADDLE_DEBUG_BEHAVIOR environment variable).
 */
class ADDLE_COMMON_EXPORT DebugBehavior
{
    Q_GADGET
public:
    static constexpr const char* DEBUG_BEHAVIOR_ENV_VARIABLE_NAME = "ADDLE_DEBUG_BEHAVIOR";

    enum DebugBehaviorOption
    {
        Normal = 0x0,

        AbortOnStartupError = 0x1,
        AbortOnLogicErrorThrown = 0x2,
        AbortOnUnhandledLogicError = 0x4,


        InterceptQObjectConnectFailure = 0x100,
        InterceptQStringArgFailure = 0x200
    };
    Q_DECLARE_FLAGS(DebugBehaviorFlags, DebugBehaviorOption)
    Q_FLAG(DebugBehaviorOption)

    static DebugBehaviorFlags& get();

    inline static bool test(DebugBehaviorOption option) { return get().testFlag(option); }

private:
    static bool _instantiated;
    static DebugBehaviorFlags _instance;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DebugBehavior::DebugBehaviorFlags)

} // namespace Addle

#endif // DEBUGBEHAVIOR_P_HPP
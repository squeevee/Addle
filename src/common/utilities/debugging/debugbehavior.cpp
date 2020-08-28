/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef ADDLE_DEBUG
#error "This file should only compiled in debug builds"
#endif

#include <QtGlobal>
#include <iostream>
#include <QStringBuilder>
#include <QCoreApplication>
#include <QMetaEnum>

#include "debugbehavior_p.hpp"

#include "globals.hpp"

using namespace Addle;

bool DebugBehavior::_instantiated = false;
DebugBehavior::DebugBehaviorFlags DebugBehavior::_instance = DebugBehaviorFlags();

DebugBehavior::DebugBehaviorFlags& DebugBehavior::get()
{
    if (!_instantiated)
    {
        if (qEnvironmentVariableIsSet(DEBUG_BEHAVIOR_ENV_VARIABLE_NAME))
        {
            bool ok;
            int init = qEnvironmentVariableIntValue(DEBUG_BEHAVIOR_ENV_VARIABLE_NAME, &ok);
            if (ok)
            {
                _instance = DebugBehaviorFlags(init);
            }
        }
        _instantiated = true;

        if ((uint)_instance)
        {
            //% "DebugBehavior initialized with these flags set:"
            qDebug() << qUtf8Printable(qtTrId("debug-messages.debug-behavior-initialized"))
                << QMetaEnum::fromType<DebugBehaviorOption>().valueToKeys(_instance).constData();
        }
    }
    return _instance;
}
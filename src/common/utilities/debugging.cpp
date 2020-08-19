#ifdef ADDLE_DEBUG //TODO this is bad

#include <QtGlobal>
#include <iostream>
#include <QStringBuilder>
#include <QCoreApplication>

#include "debugging.hpp"

#include "globals.hpp"

#include "qtextensions/qmeta.hpp"

using namespace Addle;

// void Addle::debugMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message)
// {
//     QString messageTypeString = QMetaEnum::fromType<QtMsgType>().valueToKey(type);
//     bool errorish = false;

//     // Place a breakpoint in this switch block to break on a particular message
//     // type.
//     // TODO: do this with a meta enum instead of a switch block
//     // switch(type)
//     // {
//     //     case QtMsgType::QtCriticalMsg:
//     //         errorish = true;
//     //         messageTypeString = (
//     //             "debugMessageHandler",
//     //             "critical"
//     //         );
//     //         break;
//     //     case QtMsgType::QtDebugMsg:
//     //         messageTypeString = (
//     //             "debugMessageHandler",
//     //             "debug"
//     //         );
//     //         break;
//     //     case QtMsgType::QtFatalMsg:
//     //         errorish = true;
//     //         messageTypeString = (
//     //             "debugMessageHandler",
//     //             "fatal"
//     //         );
//     //         break;
//     //     case QtMsgType::QtInfoMsg:
//     //         messageTypeString = (
//     //             "debugMessageHandler",
//     //             "info"
//     //         );
//     //         break;
//     //     case QtMsgType::QtWarningMsg:
//     //         errorish = true;
//     //         messageTypeString = (
//     //             "debugMessageHandler",
//     //             "warning"
//     //         );
//     //         break;
//     //     default:
//     //         std::cerr << qPrintable((
//     //             "debugMessageHandler",
//     //             "Unknown QtMsgType"
//     //         )) << std::endl;
//     //         std::cout << qPrintable(message) << std::endl;
//     //         return;
//     // }

//     if (errorish)
//     {
//         if(context.function || context.file)
//         {
//             std::cerr << qPrintable(QStringLiteral(
//                 "[%1] %2 (%3:%4): %5")
//                 .arg(messageTypeString)
//                 .arg(context.function)
//                 .arg(context.file)
//                 .arg(context.line)
//                 .arg(message))
//                 << std::endl;
//         }
//         else
//         {
//             std::cerr << qPrintable(
//                     //% "[%1] <unknown location>: %2"
//                     qtTrId("debug-messages.log-message.unknown-location")
//                         .arg(messageTypeString)
//                         .arg(message)
//                     )
//                 << std::endl;
//         }
//         if (DebugBehavior::test(DebugBehavior::abort_on_errorish_qtlog))
//             std::abort();
//     }
//     else
//     {
//         if(context.function)
//         {
//             std::cout << qPrintable(QStringLiteral(
//                 "[%1] %2: %3")
//                 .arg(messageTypeString)
//                 .arg(context.function)
//                 .arg(message))
//                 << std::endl;
//         }
//         else
//         {
//             std::cout << qPrintable(QStringLiteral(
//                 "[%1] %2")
//                 .arg(messageTypeString)
//                 .arg(message))
//                 << std::endl;
//         }
//     }
// }

bool DebugBehavior::_instantiated = false;
DebugBehavior::DebugBehaviorFlags DebugBehavior::_instance = DebugBehaviorFlags();

DebugBehavior::DebugBehaviorFlags& DebugBehavior::get()
{
    if (!_instantiated)
    {
#ifdef ADDLE_DEBUG_BEHAVIOR
        _instance = DebugBehaviorFlags(ADDLE_DEBUG_BEHAVIOR);
#else
        if (qEnvironmentVariableIsSet(DEBUG_BEHAVIOR_ENV_VARIABLE_NAME))
        {
            bool ok;
            int init = qEnvironmentVariableIntValue(DEBUG_BEHAVIOR_ENV_VARIABLE_NAME, &ok);
            if (ok)
            {
                _instance = DebugBehaviorFlags(init);
            }
        }
#endif
        _instantiated = true;
    }
    return _instance;
}

#endif
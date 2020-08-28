/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef MESSAGEHANDLER_HPP
#define MESSAGEHANDLER_HPP

#ifndef ADDLE_DEBUG
#error "This file should only be included in debug builds"
#endif

#include <QtGlobal>
#include <QtDebug>
#include <QRegularExpression>
#include <QMetaEnum>

#include "interfaces/services/ierrorservice.hpp"
#include "exceptions/addleexception.hpp"
#include "debugbehavior.hpp"

#include "servicelocator.hpp"

namespace Addle {

// Certain logic errors in using Qt result in mostly-quiet failures with only a
// warning message given. The purpose of this function is to intercept those
// errors and make more noise about them.

// Warning messages are not all equally problematic, but also do not contain any
// identifiable metadata (at least not in release builds of Qt). However, their
// text is not internationalized, so we can filter by English strings.

// This should only be used by `main()`

DECL_LOGIC_ERROR(QtWarningAsException)
class QtWarningAsException : public AddleException
{
    ADDLE_EXCEPTION_BOILERPLATE(QtWarningAsException)
public:
    QtWarningAsException(const QString& message, DebugBehavior::DebugBehaviorOption reason)
        //% "A Qt warning was intercepted (%1):\n"
        //% "%2"
        : AddleException(qtTrId("debug-messages.qt-warning-as-exception")
            .arg(QMetaEnum::fromType<DebugBehavior::DebugBehaviorOption>().valueToKey(reason))
            .arg(message)
        )
    {
    }
};

static void addleMessageHandler(QtMsgType type,
    const QMessageLogContext& context,
    const QString& message)
{
    static QtMessageHandler defaultHandler = nullptr;
    if (!defaultHandler)
    {
        qInstallMessageHandler(0);
        defaultHandler = qInstallMessageHandler(&addleMessageHandler);
    }

    try 
    {
        static DebugBehavior::DebugBehaviorFlags behavior = DebugBehavior::get();
        static IErrorService& errorService = ServiceLocator::get<IErrorService>();

        if (behavior.testFlag(DebugBehavior::FilterQObjectConnectFailure))
        {
            static const QRegularExpression matchQObjectConnectFailure
                = QRegularExpression(QStringLiteral("QObject::connect"));
            
            if (matchQObjectConnectFailure.match(message).hasMatch())
            {
                errorService.reportUnhandledError(QtWarningAsException(message, DebugBehavior::FilterQObjectConnectFailure));
                return;
            }
        }

        if (behavior.testFlag(DebugBehavior::FilterQStringArgFailure))
        {
            static const QRegularExpression matchQStringArgFailure
                = QRegularExpression(QStringLiteral("QString::arg"));

            if (matchQStringArgFailure.match(message).hasMatch())
            {
                errorService.reportUnhandledError(QtWarningAsException(message, DebugBehavior::FilterQStringArgFailure));
                return;
            }
        }

        defaultHandler(type, context, message);
    }
    catch(...)
    {
        qInstallMessageHandler(0);

        //% "An error occurred in addleMessageHandler. The default message handler will be used from here on."
        qWarning() << qUtf8Printable("debug-messages.message-handler.internal-error");
    }
}

} // namespace Addle

#endif // MESSAGEHANDLER_HPP
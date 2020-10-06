/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include <stdexcept>

#include "errorservice.hpp"

#include "servicelocator.hpp"

#include "interfaces/presenters/messages/inotificationpresenter.hpp"
#include "interfaces/views/itoplevelview.hpp"
#include "interfaces/views/imessageview.hpp"

#include "utilities/debugging/debugbehavior.hpp"
#include "utilities/presenter/messagebuilders.hpp"

using namespace Addle;

ErrorService::ErrorService()
{
#ifdef ADDLE_DEBUG
    DebugBehavior::get();
#endif

    connect(this, &ErrorService::unhandledErrorReported, this, &ErrorService::displayError);
}

void ErrorService::reportUnhandledError(const AddleException& ex, Severity severity)
{
#ifdef ADDLE_DEBUG
    if (DebugBehavior::test(DebugBehavior::AbortOnUnhandledLogicError) && ex.isLogicError())
    {
        std::abort();
    }
#endif

    auto unhandled = QSharedPointer<UnhandledException>(new UnhandledException(ex, severity));
    emit unhandledErrorReported(unhandled);
}

void ErrorService::reportUnhandledError(const std::exception& ex, Severity severity)
{
#ifdef ADDLE_DEBUG
    if (DebugBehavior::test(DebugBehavior::AbortOnUnhandledLogicError) && dynamic_cast<const std::logic_error*>(&ex))
    {
        std::abort();
    }
#endif

    auto unhandled = QSharedPointer<UnhandledException>(new UnhandledException(ex, severity));
    emit unhandledErrorReported(unhandled);
}

void ErrorService::displayError(QSharedPointer<UnhandledException> ex)
{
#ifdef ADDLE_DEBUG
    qWarning() << qUtf8Printable(ex->what());
    if (ex->addleException())
        qWarning() << qUtf8Printable(ex->addleException()->what());
    if (ex->stdException())
        qWarning() << qUtf8Printable(ex->stdException()->what());
#endif

    auto message = ServiceLocator::makeShared<INotificationPresenter>(
        NotificationPresenterBuilder(
#ifdef ADDLE_DEBUG
            QString(ex->what()), 
#else 
            //: Displayed in "release" builds of Addle if an error occurs in
            //: some part of the application and was not handled there. If this 
            //: message is displayed, it indicates a potentially serious bug of 
            //: unknown nature.
            //
            //: As Addle's error handling matures, this message will likely 
            //: require retooling.
            //
            //% "An unknown error occurred within Addle. The application may "
            //% "be in an unstable state. Proceed with caution."
            qtTrId("ui.release-unhandled-error"),
#endif // ADDLE_DEBUG
            IMessagePresenter::Problem
        )
            .setIsUrgentHint(true)
            .setException(ex)
    );
    
    //TODO: cleaner
    auto view = ServiceLocator::make<IMessageView>(message);
    auto tlv = qobject_interface_cast<ITopLevelView*>(view);
    ADDLE_ASSERT(tlv);
    tlv->open();
}

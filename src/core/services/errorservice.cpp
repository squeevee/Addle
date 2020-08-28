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

#include "interfaces/presenters/errors/iapplicationerrorpresenter.hpp"
#include "interfaces/views/iapplicationerrorview.hpp"

#include "utilities/debugging/debugbehavior.hpp"

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
    ServiceLocator::make<IApplicationErrorPresenter>(ex)->view().show();
}
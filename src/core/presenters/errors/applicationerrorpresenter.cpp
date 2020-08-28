/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "applicationerrorpresenter.hpp"

#include "utils.hpp"

using namespace Addle;

void ApplicationErrorPresenter::initialize(QSharedPointer<AddleException> error)
{
    const Initializer init(_initHelper);

    _error = error;
    _initHelper.setCheckpoint(InitCheck_ErrorSet);

    _view = ServiceLocator::make<IApplicationErrorView>(std::ref(*this));
    connect_interface(_view, SIGNAL(destroyed()), this, SLOT(deleteLater()));
}


QString ApplicationErrorPresenter::message() const
{
    ASSERT_INIT_CHECKPOINT(InitCheck_ErrorSet);
#ifdef ADDLE_DEBUG
    return _error->what();
#else 
    //  Should perhaps be out of source?
    //% "A logic error occurred. (Debug information is not available.)"
    return qtTrId("error-messages.logic-error-release");
#endif // ADDLE_DEBUG
}


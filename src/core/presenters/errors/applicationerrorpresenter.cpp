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
    //: Displayed in "release" builds of Addle if an error occurs in some part
    //: of the application and was not handled there. If this message is
    //: displayed, it indicates a potentially serious bug of unknown nature.
    //
    //: As Addle's error handling matures, this message will likely require
    //: retooling.
    //
    //% "An unknown error occurred within Addle. The application may be in an"
    //% "unstable state. Proceed with caution."
    return qtTrId("ui.release-unhandled-error");
#endif // ADDLE_DEBUG
}


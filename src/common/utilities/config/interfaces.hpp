/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License. 
 * See "LICENSE" for full details.
 */

#pragma once

#include "interfaces/services/iapplicationservice.hpp"
#include "interfaces/presenters/imaineditorpresenter.hpp"
#include "interfaces/presenters/icanvaspresenter.hpp"
#include "interfaces/presenters/icolorselectionpresenter.hpp"
#include "interfaces/presenters/iviewportpresenter.hpp"
#include "interfaces/presenters/imessagecontext.hpp"
#include "interfaces/presenters/ipalettepresenter.hpp"
#include "interfaces/presenters/tools/itoolpresenter.hpp"
#include "interfaces/views/imaineditorview.hpp"

#ifndef ADDLE_CONFIG_INTERFACES

#define ADDLE_CONFIG_INTERFACES \
    Addle::IApplicationService, \
    Addle::IMainEditorPresenter, \
    Addle::ICanvasPresenter, \
    Addle::IColorSelectionPresenter, \
    Addle::IViewPortPresenter, \
    Addle::IMessageContext, \
    Addle::IPalettePresenter, \
    Addle::IToolPresenter, \
    Addle::IMainEditorView

// ADDLE_CONFIG_INTERFACES contains a comma-separated list of the 
// unqualified names of interfaces exposed to the dependency injector.
// The interfaces and all their traits must be defined (`#include`d) here
// as well.

#endif // ADDLE_CONFIG_INTERFACES

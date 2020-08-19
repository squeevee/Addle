/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef ADDLE_ICON

#include "interfaces/services/iappearanceservice.hpp"
#include "servicelocator.hpp"

#define ADDLE_ICON(identifier) ::Addle::ServiceLocator::get<IAppearanceService>().icon(identifier)

#endif
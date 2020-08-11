#ifndef ADDLE_ICON

#include "interfaces/services/iappearanceservice.hpp"
#include "servicelocator.hpp"

#define ADDLE_ICON(identifier) ::Addle::ServiceLocator::get<IAppearanceService>().icon(identifier)

#endif
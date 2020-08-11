#ifndef ADDLE_TEXT

#include "servicelocator.hpp"
#include "interfaces/services/ii18nservice.hpp"

#define ADDLE_TEXT(identifier) ::Addle::ServiceLocator::get<II18nService>().text(identifier)

#endif
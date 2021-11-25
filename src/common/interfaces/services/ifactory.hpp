#pragma once

#include "utilities/config3/factory.hpp"

namespace Addle {

template<typename Interface>
class IFactory : public Factory<Interface> {}; // HACK

}

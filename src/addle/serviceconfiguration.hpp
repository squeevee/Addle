/**
 * Addle source code
 * Copyright 2020 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License.
 * See "LICENSE" for full details.
 */


#include "utilities/configuration/baseserviceconfiguration.hpp"

namespace Addle {

class ServiceConfiguration : public BaseServiceConfiguration
{
protected:
    void configure();
};

} // namespace Addle
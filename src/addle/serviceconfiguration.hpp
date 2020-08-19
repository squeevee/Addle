/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "utilities/configuration/serviceconfigurationbase.hpp"

namespace Addle {

class ServiceConfiguration : public ServiceConfigurationBase
{
protected:
    void configure();
};

} // namespace Addle
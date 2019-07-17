/**
 * Addle source code
 * Copyright 2019 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License.
 * See "LICENSE" for full details.
 */

#include "servicelocator.h"
#include "utilities/tfactory.h"

namespace configure
{
    class ServiceConfiguration : public ServiceLocator::AbstractConfiguration
    {
    protected:
        void configure();
    };
}

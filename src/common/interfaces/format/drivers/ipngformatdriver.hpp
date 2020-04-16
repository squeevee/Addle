#ifndef IPNGFORMATDRIVER_HPP
#define IPNGFORMATDRIVER_HPP

#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/traits/compat.hpp"

#include "../iformatdriver.hpp"

#include "idtypes/formatid.hpp"

class ADDLE_COMMON_EXPORT IPNGFormatDriver : public virtual IFormatDriver
{
public:
    virtual ~IPNGFormatDriver() = default;
};

DECL_MAKEABLE(IPNGFormatDriver)

#endif // IPNGFORMATDRIVER_HPP
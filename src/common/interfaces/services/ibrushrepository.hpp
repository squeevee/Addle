#ifndef IBRUSHREPOSITORY_HPP
#define IBRUSHREPOSITORY_HPP

#include "ipersistentobjectrepository.hpp"
#include "iservice.hpp"

#include "idtypes/brushid.hpp"
#include "interfaces/models/ibrush.hpp"

#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"

struct IBrushRepository : IPersistentObjectRepository<BrushId, IBrush>, IService {};
DECL_IMPLEMENTED_AS_QOBJECT(IBrushRepository)
DECL_MAKEABLE(IBrushRepository)

#endif // IBRUSHREPOSITORY_HPP
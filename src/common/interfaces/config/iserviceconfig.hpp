#ifndef ISERVICECONFIG_HPP
#define ISERVICECONFIG_HPP

#include <typeindex>
#include <functional>

#include <QHash>

#include "idtypes/addleid.hpp"
#include "idtypes/moduleid.hpp"

namespace Addle {
    
class FactorySelector;
class IFactory;
class IServiceConfig
{
public:
    virtual ~IServiceConfig() = default;
    
    virtual ModuleId id() const = 0;
    
    virtual IFactory* factory(const FactorySelector& index, bool* isIndiscriminate = nullptr) const = 0;
    
    virtual QHash<std::type_index, QSet<AddleId>> persistentObjects() const = 0;
    virtual void initializeObject(AddleId id, void* object) = 0;
};
    
} // namespace Addle

#endif // ISERVICECONFIG_HPP

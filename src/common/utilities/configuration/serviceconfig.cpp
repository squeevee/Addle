#include "servicelocatorcontrol.hpp"

#include "serviceconfig.hpp"

using namespace Addle;

bool ServiceConfig::Filter::test(const FactorySelector& index) const
{
    if (index.id() && _idTest)
    {
        if (!_idTest(index.id()))
            return false;
    }
    
    if (_argTests.count() > index.argCount())
        return false;
    
    for (int i = 0; i < qMin(index.argCount(), _argTests.count()); ++i)
    {
        if (_argTests.at(i) && !_argTests.at(i)(index.argAt(i)))
            return false;
    }
    
    return true;
}

ServiceConfig::ServiceConfig(ModuleId id)
    : _id(id)
{
}

ServiceConfig::~ServiceConfig()
{
    for (auto entry : noDetach(_factories))
    {
        delete entry.second; // factory
    }
}

IFactory* ServiceConfig::factory(const FactorySelector& index, bool* isIndiscriminate_p) const
{
    auto indIndex = qMakePair(index.interfaceType(), index.id());
    
    if (_indiscriminateFactories.contains(indIndex))
    {
        if (isIndiscriminate_p)
            *isIndiscriminate_p = true;
        return _indiscriminateFactories.value(indIndex);
    }
    else
    {
        auto i = _factories.constFind(index.interfaceType());
        auto&& end = _factories.constEnd();
        
        while (i != end && i.key() == index.interfaceType())
        {
            auto& filter = i.value().first;
            auto& factory = i.value().second;
            
            if (filter.test(index))
            {
                if (isIndiscriminate_p)
                    *isIndiscriminate_p = filter.isIndiscriminate();
                return factory;
            }
            // test against filter
            ++i;
        }
        
        if (isIndiscriminate_p)
            *isIndiscriminate_p = false;
        return nullptr;
    }
}

void ServiceConfig::initializeObject(AddleId id, void* object)
{
    if(_persistentObjectInitializers.contains(id))
    {
        _persistentObjectInitializers.value(id)(object);
    }
}
    
void ServiceConfig::commit()
{
    ServiceLocatorControl::configure(this);
}

void ServiceConfig::addFactory_p(std::type_index interfaceIndex, const Filter& filter, IFactory* factory)
{
    try
    {
        auto indIndex = qMakePair(interfaceIndex, filter.id());
        
        ADDLE_ASSERT(!_indiscriminateFactories.contains(indIndex));
        if (filter.isIndiscriminate() && !_factories.contains(interfaceIndex))
        {
            _indiscriminateFactories.insert(indIndex, factory);
        }
        
        _factories.insert(interfaceIndex, qMakePair(filter, factory));
    }
    catch(...)
    {
        delete factory;
        throw;
    }
}

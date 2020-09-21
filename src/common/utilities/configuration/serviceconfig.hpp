#ifndef SERVICECONFIG_HPP
#define SERVICECONFIG_HPP

#include <typeinfo>
#include <functional>

#include <QHash>
#include <QMultiHash>

#include "servicelocator.hpp"
#include "interfaces/config/ifactory.hpp"
#include "interfaces/config/iserviceconfig.hpp"

#include "factoryselector.hpp"
#include "autofactory.hpp"

#include "idtypes/moduleid.hpp"
#include "utilities/errors.hpp"

namespace Addle {

class ServiceConfig : public IServiceConfig
{
public:
    class Filter
    {
    public:
        Filter() = default;
        Filter(AddleId id)
            : _id(id)
        {
        }
        
        inline AddleId id() const { return _id; }
        inline bool isIndiscriminate() const { return !_idTest && _argTests.isEmpty(); }
        
        inline Filter& byId(AddleId id)
        {
            ADDLE_ASSERT(id);
            ADDLE_ASSERT(!_id);
            _id = id;
            return *this;
        }

        template<class IdType>
        inline Filter& byId(std::function<bool(IdType)> test)
        {
            ADDLE_ASSERT(test);
            ADDLE_ASSERT(!_idTest);
            _idTest = [=](AddleId id) -> bool {
                auto typedId = static_cast<IdType>(id);
                return typedId && test(typedId);
            };
            
            return *this;
        }
        
        template<class Interface>
        inline typename std::enable_if<
            std::is_base_of<QObject, Interface>::value ||
            Traits::implemented_as_QObject<Interface>::value,
            Filter&
        >::type byArg(std::function<bool(const Interface&)> test = nullptr)
        {
            _argTests.append(
                [=](QVariant var) -> bool {
                    QObject* qobj = var.value<QObject*>();
                    if (!qobj)
                        return false;
                    
                    Interface* iobj = qobject_cast<Interface*>(qobj);
                    if (!iobj)
                        return false;
                    
                    if (test)
                        return test(*iobj);
                    else
                        return true;
                }
            );
            return *this;
        }
        
        template<typename T>
        inline typename std::enable_if<
            !std::is_base_of<QObject, T>::value &&
            !Traits::implemented_as_QObject<T>::value,
            Filter&
        >::type byArg(std::function<bool(const T&)> test = nullptr)
        {
            _argTests.append(
                [=](QVariant var) -> bool {
                    if (!var.canConvert<T>())
                        return false;
                    
                    if (test)
                        return test(var.value<T>());
                    else
                        return true;
                }
            );
            return *this;
        }
        
        inline Filter& skipArg()
        {
            _argTests.append(nullptr);
            return *this;
        }
        
        bool test(const FactorySelector& index) const;
        
    private:
        AddleId _id;
        std::function<bool(AddleId)> _idTest;
        QList<std::function<bool(QVariant)>> _argTests;
    };
        
    ServiceConfig(ModuleId id);
    ~ServiceConfig();
    
    ModuleId id() const override { return _id; }
    IFactory* factory(const FactorySelector& index, bool* isIndiscriminate = nullptr) const override;
    QHash<std::type_index, QSet<AddleId>> persistentObjects() const { return _persistentObjects; }
    void initializeObject(AddleId id, void* object);
    
    template<class Interface, class Impl>
    typename std::enable_if<
        !Traits::is_private_makeable_by_id<Interface>::value
        || std::is_default_constructible<Impl>::value
    >::type addAutoFactory(const Filter& filter = Filter())
    {
        addFactory_p(
            std::type_index(typeid(Interface)),
            filter,
            new AutoFactory<Interface, Impl>
        );
    }
    
    template<class Interface, class Impl>
    typename std::enable_if<
        Traits::is_private_makeable_by_id<Interface>::value
        && !std::is_default_constructible<Impl>::value
    >::type addAutoFactory(const Filter& filter = Filter())
    {
        auto index = std::type_index(typeid(Interface));
        addFactory_p(
            index,
            filter,
            new AutoFactoryById<Interface, Impl>
        );
        _persistentObjects[index].insert(filter.id());
    }
    
    template<class Interface>
    void setupPersistentObjects(QList<typename Traits::id_type<Interface>::type> ids)
    {
        auto& set = _persistentObjects[std::type_index(typeid(Interface))];
        for (auto id : noDetach(ids))
        {        
            set.insert(id);
        }
    }
    
    template<class Interface, typename... ArgTypes>
    void setupPersistentObject(typename Traits::id_type<Interface>::type id, ArgTypes... args)
    {
        auto index = std::type_index(typeid(Interface));
        _persistentObjects[index].insert(id);
        _persistentObjectInitializers.insert(id, [=](void* obj) {
            reinterpret_cast<Interface*>(obj)->initialize(args...);
        });
    }
    
    void commit();
    
private:
    const ModuleId _id;
        
    QMultiHash<std::type_index, QPair<Filter, IFactory*>> _factories;
    QHash<QPair<std::type_index, AddleId>, IFactory*> _indiscriminateFactories;
    
    QHash<std::type_index, QSet<AddleId>> _persistentObjects;
    QHash<AddleId, std::function<void(void*)>> _persistentObjectInitializers;
        
    void addFactory_p(std::type_index interfaceIndex, const Filter& filter, IFactory* factory);
};

} // namespace Addle

#endif // SERVICECONFIG_HPP

#include "utilities/debugging/debugbehavior.hpp"

#include "servicelocatorcontrol.hpp"
#include "serviceconfig.hpp"

using namespace Addle;

bool ServiceConfig::Filter::test(const FactorySelector& index) const
{
    if (index.id() && _idTest)
    {
        if (!_idTest(index.id()))
        {
#ifdef ADDLE_DEBUG    
            if (DebugBehavior::test(DebugBehavior::PrintFactorySelectionInfo))
            {
                //% "The filter rejected the selector's id."
                qDebug() << qUtf8Printable(qtTrId("debug-messages.factory-select.filter-rejected-id"));
            }
#endif
            return false;
        }
    }
    
    if (_argTests.count() > index.argCount())
    {
#ifdef ADDLE_DEBUG    
        if (DebugBehavior::test(DebugBehavior::PrintFactorySelectionInfo))
        {
            //% "The filter expected more arguments than were provided by the selector.\n"
            //% " expected: %1\n"
            //% "   actual: %2"
            qDebug() << qUtf8Printable(qtTrId("debug-messages.factory-select.too-few-arguments")
                .arg(_argTests.count())
                .arg(index.argCount()));
        }
#endif
        return false;
    }
    
    for (int i = 0; i < qMin(index.argCount(), _argTests.count()); ++i)
    {
        if (_argTests.at(i) && !_argTests.at(i)(index.argAt(i)))
        {
#ifdef ADDLE_DEBUG    
            if (DebugBehavior::test(DebugBehavior::PrintFactorySelectionInfo))
            {
                QVariant arg = index.argAt(i);
                QString typeName;
                QString repr;
                if (arg.canConvert<QObject*>())
                {
                    const QObject* argObj = arg.value<QObject*>();
                    typeName = argObj && argObj->metaObject() ?
                        argObj->metaObject()->className()
                        : QObject::staticMetaObject.className();
                    repr = "0x" + QString::number(reinterpret_cast<quintptr>(argObj), 16);
                }
                else
                {
                    typeName = arg.typeName();
                    repr = arg.toString();
                }
                
                //% "The filter rejected the selector's argument at index %1.\n"
                //% "  type: %2\n"
                //% " value: %3"
                qDebug() << qUtf8Printable(qtTrId("debug-messages.factory-select.filter-rejected-arg")
                    .arg(i)
                    .arg(typeName)
                    .arg(repr));
            }
#endif
            return false;
        }
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
#ifdef ADDLE_DEBUG
    if (DebugBehavior::test(DebugBehavior::PrintFactorySelectionInfo))
    {
        //% "Beginning factory selection\n"
        //% " type_index: `%1`\n"
        //% "         id: \"%2\"\n"
        //% "     module: \"%3\""
        qDebug() << qUtf8Printable(qtTrId("debug-messsages.factory-select.begin")
            .arg(index.interfaceType().name())
            .arg(index.id().key())
            .arg(_id.key()));
    }
#endif
    auto indIndex = qMakePair(index.interfaceType(), index.id());
    
    if (_indiscriminateFactories.contains(indIndex))
    {
        if (isIndiscriminate_p)
            *isIndiscriminate_p = true;
        
        auto factory = _indiscriminateFactories.value(indIndex);
#ifdef ADDLE_DEBUG
        if (DebugBehavior::test(DebugBehavior::PrintFactorySelectionInfo))
        {
            //% "An indiscriminate factory was found:\n"
            //% "        factory: %1\n"
            //% " implementation: %2"
            qDebug() << qUtf8Printable(qtTrId("debug-messsages.factory-select.indiscriminate-found")
                .arg(factory->factoryName())
                .arg(factory->implementationName()));
        }
#endif
        return factory;
    }
    else
    {
        auto i = _factories.constFind(index.interfaceType());
        auto&& end = _factories.constEnd();
        
#ifdef ADDLE_DEBUG
        if (DebugBehavior::test(DebugBehavior::PrintFactorySelectionInfo))
        {
            if (i == end || i.key() != index.interfaceType())
            {
                //% "No eligible factories were found. (%1 factories available)"
                qDebug() << qUtf8Printable(qtTrId("debug-messsages.factory-select.no-factories")
                    .arg(_factories.count()));
            }
        }
#endif 
        
        while (i != end && i.key() == index.interfaceType())
        {
            auto& filter = i.value().first;
            auto& factory = i.value().second;
            
#ifdef ADDLE_DEBUG
            if (DebugBehavior::test(DebugBehavior::PrintFactorySelectionInfo))
            {
                //% "Considering factory %1\n"
                //% " implementation: %2"
                qDebug() << qUtf8Printable(qtTrId("debug-messages.factory-select.considering-factory")
                    .arg(factory->factoryName())
                    .arg(factory->implementationName()));
            }
#endif 
            
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

#include "interfacebindingconfig.hpp"
#include "moduleconfig.hpp"

#ifdef ADDLE_DEBUG
#include "utilities/debugging/stacktrace_config.hpp"
#include "utilities/debugging/qdebug_extensions.hpp"
#endif

using namespace Addle;

bool Addle::aux_config3::_typename_check_impl(const QObject* obj, const QByteArray& name, bool exact)
{
    if (!obj) return false;
    return exact ? (obj->metaObject()->className() == name) : (obj->inherits(name.data()));
}

bool Addle::aux_config3::_typename_check_impl(const QVariant& var, const QByteArray& name, bool exact, QVariant* overwrite)
{
    auto type = QMetaType::type(name.data());
    
    if (!type) return false;
    
    if (exact) 
        return var.type() == type;
    else if (!var.canConvert(type)) 
        return false;
    
    QVariant converted(var);
    bool result = converted.convert(type);
    
    if (overwrite && result) *overwrite = std::move(converted);
    return result;
}

#if defined(ADDLE_DEBUG) || defined(ADDLE_TEST)
QDebug Addle::aux_config3::operator<<(QDebug debug, const BindingCondition& condition)
{
    if (condition.isNull()) return debug << "BindingCondition()";
    
    QDebugStateSaver s1(debug);
    debug.nospace();
    
    debug << "BindingCondition(";
    
    if (condition.valueType != BindingCondition::None)
    {
        debug << "(param)";
#ifdef ADDLE_DEBUG
        if (!condition.paramName.isEmpty())
        {
            QDebugStateSaver s2(debug);
            debug.noquote();
            debug << ' ' << condition.paramName;
        }
        else
#endif
        {
            debug << '[' << condition.paramIndex << ']';
        }
        
        if (condition.valueType == BindingCondition::TypeName)
        {
            QDebugStateSaver s3(debug);
            debug.noquote();
            debug << " (has type) < " << condition.bytes << " >";
        }
        else
        {
            debug << " == ";
            
#ifdef ADDLE_DEBUG
            if (condition.valueToString)
            {
                QDebugStateSaver s4(debug);
                debug.noquote();
                debug << condition.valueToString(condition);
            }
            else
#endif
            {
                switch(condition.valueType)
                {
                    case BindingCondition::None:
                    case BindingCondition::TypeName:
                        Q_UNREACHABLE();
                        
                    case BindingCondition::Int:
                        debug << condition.number.asInt;
                        break;
                        
                    case BindingCondition::Unsigned:
                        debug << condition.number.asUnsigned;
                        break;
                        
                    case BindingCondition::Long:
                        debug << condition.number.asLong;
                        break;
                        
                    case BindingCondition::UnsignedLong:
                        debug << condition.number.asUnsignedLong;
                        break;
                        
                    case BindingCondition::Bytes:
                        debug << condition.bytes;
                        break;
                }
            }
        }
    }
    
    if (condition.valueType != BindingCondition::None && condition.function)
        debug << " && ";
    
    if (condition.function)
    {
        debug << "(result of) " << condition.function;
    }

    return debug << ')'; 
}
#endif

aux_config3::ModuleConfig aux_config3::InterfaceBindingConfig::addModule(QByteArray name)
{
    return ModuleConfig(*this, std::move(name));
}

void aux_config3::ModuleConfig::commit()
{
    assert(!_committed);
    _committed = true;
    
    const QWriteLocker lock(&_injectorConfig._lock);
    
    QSet<std::type_index> unconditionals;
    
    bool idAlreadyCommitted = _injectorConfig._index_byModuleId.contains(_id);
    
    QVarLengthArray<MakeBinding> shadowedUnconditionals;
    QVarLengthArray<MakeBinding> shadowedConditionals;
    
    for (const auto& binding : noDetach(_makeBindings))
    {
        if (binding.condition.isNull())
        {
            if (unconditionals.contains(*binding.interfaceType))
            {
                shadowedUnconditionals.append(binding);
                continue;
            }
            else
            {
                unconditionals.insert(*binding.interfaceType);
            }
        }
        
        auto find = noDetach(_injectorConfig._index_byInterface).find(*binding.interfaceType);
        if (find == _injectorConfig._index_byInterface.cend()) continue;
        
        const auto& interfaceEntry = **find;
        
        if (binding.condition.isNull() && interfaceEntry.unconditionalMake.function)
        {
            shadowedUnconditionals.append(binding);
            continue;
        }
        
        if (binding.condition.valueType != BindingCondition::None && !binding.condition.function)
        {
            for (const auto& otherMakeBinding : noDetach(interfaceEntry.conditionalMakes))
            {
                if (Q_LIKELY(binding.condition.valueType != otherMakeBinding.condition.valueType))
                    continue;
                
                switch(binding.condition.valueType)
                {
                    case BindingCondition::None:
                        Q_UNREACHABLE();
                        
                    case BindingCondition::Int:
                        if (binding.condition.number.asInt == otherMakeBinding.condition.number.asInt)
                            shadowedConditionals.append(binding);
                        break;
                    case BindingCondition::Unsigned:
                        if (binding.condition.number.asUnsigned == otherMakeBinding.condition.number.asUnsigned)
                            shadowedConditionals.append(binding);
                        break;
                    case BindingCondition::Long:
                        if (binding.condition.number.asLong == otherMakeBinding.condition.number.asLong)
                            shadowedConditionals.append(binding);
                        break;
                    case BindingCondition::UnsignedLong:
                        if (binding.condition.number.asUnsignedLong == otherMakeBinding.condition.number.asUnsignedLong)
                            shadowedConditionals.append(binding);
                        break;
                    case BindingCondition::Bytes:
                    case BindingCondition::TypeName:
                        if (binding.condition.bytes == otherMakeBinding.condition.bytes)
                            shadowedConditionals.append(binding);
                        break;
                }
            }
        }
    }
    
    // TODO verify get-bindings
    
    if (idAlreadyCommitted || !shadowedUnconditionals.isEmpty() || !shadowedConditionals.isEmpty())
    {
        QList<ModuleConfigCommitFailure::ShadowedBinding> shadowedBindings;
        shadowedBindings.reserve(shadowedUnconditionals.size() + shadowedConditionals.size());
        
        for (const auto& binding : shadowedUnconditionals)
        {
            shadowedBindings.append({
                      nullptr
                    , binding.interfaceType
#ifdef ADDLE_DEBUG
                    , binding.implType
#endif
                });
        }
        
        for (const auto& binding : shadowedConditionals)
        {
            shadowedBindings.append({
                      QSharedPointer<BindingCondition>::create(std::move(binding.condition))
                    , binding.interfaceType
#ifdef ADDLE_DEBUG
                    , binding.implType
#endif
                });
        }
        
        ADDLE_THROW(ModuleConfigCommitFailure(_id, idAlreadyCommitted, std::move(shadowedBindings)));
    }
    
    // === COMMIT ===
    
    auto moduleIt = _injectorConfig._modules.insert(
            _injectorConfig._modules.end(),
            { _id , std::move(_injectors) , std::move(_makeBindings), std::move(_getBindings) }
        );
    
    _injectorConfig._index_byModuleId[_id] = moduleIt;
    
    for (const auto& binding : noDetach((*moduleIt).makeBindings))
    {
        std::list<InterfaceBindingConfig::InterfaceEntry>::iterator interfaceIt;
        
        auto find = _injectorConfig._index_byInterface.find(*binding.interfaceType);
        if (find != _injectorConfig._index_byInterface.end())
        {
            interfaceIt = *find;
        }
        else
        {
            interfaceIt = _injectorConfig._interfaces.insert(
                    _injectorConfig._interfaces.end(), { *binding.interfaceType }
                );
            _injectorConfig._index_byInterface[*binding.interfaceType] = interfaceIt;
        }
        
        if (binding.condition.isNull())
        {
            (*interfaceIt).unconditionalMake = binding;
        }
        else 
        {
            if (binding.condition.valueType != BindingCondition::None && (*interfaceIt).conditionalMakes.isEmpty())
            {
                assert(std::holds_alternative<std::monostate>((*interfaceIt).makeIndex));
                switch(binding.condition.valueType)
                {
                    case BindingCondition::None:
                        Q_UNREACHABLE();
                        
                    case BindingCondition::Int:
                        (*interfaceIt).makeIndex = InterfaceBindingConfig::InterfaceEntry::make_index_t<int>(
                                {{ binding.condition.number.asInt, binding }}
                            );
                        break;
                        
                    case BindingCondition::Unsigned:
                        (*interfaceIt).makeIndex = InterfaceBindingConfig::InterfaceEntry::make_index_t<unsigned>(
                                {{ binding.condition.number.asUnsigned, binding }}
                            );
                        break;
                        
                    case BindingCondition::Long:
                        (*interfaceIt).makeIndex = InterfaceBindingConfig::InterfaceEntry::make_index_t<long>(
                                {{ binding.condition.number.asLong, binding }}
                            );
                        break;
                        
                    case BindingCondition::UnsignedLong:
                        (*interfaceIt).makeIndex = InterfaceBindingConfig::InterfaceEntry::make_index_t<unsigned long>(
                                {{ binding.condition.number.asUnsignedLong, binding }}
                            );
                        break;
                        
                    case BindingCondition::Bytes:
                    case BindingCondition::TypeName:
                        (*interfaceIt).makeIndex = InterfaceBindingConfig::InterfaceEntry::make_index_t<QByteArray>(
                                {{ binding.condition.bytes, binding }}
                            );
                        break;
                }
                
                assert(binding.condition.paramIndex >= 0);
                (*interfaceIt).conditionalMakeIndexedParameter = binding.condition.paramIndex;
            }
            else if (binding.condition.valueType != BindingCondition::None 
                && binding.condition.paramIndex == (*interfaceIt).conditionalMakeIndexedParameter)
            {
                assert(!std::holds_alternative<std::monostate>((*interfaceIt).makeIndex));
                switch(binding.condition.valueType)
                {
                    case BindingCondition::None:
                        Q_UNREACHABLE();
                        
                    case BindingCondition::Int:
                        assert(std::holds_alternative<InterfaceBindingConfig::InterfaceEntry::make_index_t<int>>((*interfaceIt).makeIndex));
                        std::get_if<InterfaceBindingConfig::InterfaceEntry::make_index_t<int>>(&(*interfaceIt).makeIndex)
                            ->insert(binding.condition.number.asInt, binding);
                        break;
                        
                    case BindingCondition::Unsigned:
                        assert(std::holds_alternative<InterfaceBindingConfig::InterfaceEntry::make_index_t<unsigned>>((*interfaceIt).makeIndex));
                        std::get_if<InterfaceBindingConfig::InterfaceEntry::make_index_t<unsigned>>(&(*interfaceIt).makeIndex)
                            ->insert(binding.condition.number.asUnsigned, binding);
                        break;
                        
                    case BindingCondition::Long:
                        assert(std::holds_alternative<InterfaceBindingConfig::InterfaceEntry::make_index_t<long>>((*interfaceIt).makeIndex));
                        std::get_if<InterfaceBindingConfig::InterfaceEntry::make_index_t<long>>(&(*interfaceIt).makeIndex)
                            ->insert(binding.condition.number.asLong, binding);
                        break;
                        
                    case BindingCondition::UnsignedLong:
                        assert(std::holds_alternative<InterfaceBindingConfig::InterfaceEntry::make_index_t<unsigned long>>((*interfaceIt).makeIndex));
                        std::get_if<InterfaceBindingConfig::InterfaceEntry::make_index_t<unsigned long>>(&(*interfaceIt).makeIndex)
                            ->insert(binding.condition.number.asUnsignedLong, binding);
                        break;
                        
                    case BindingCondition::Bytes:
                    case BindingCondition::TypeName:
                        assert(std::holds_alternative<InterfaceBindingConfig::InterfaceEntry::make_index_t<QByteArray>>((*interfaceIt).makeIndex));
                        std::get_if<InterfaceBindingConfig::InterfaceEntry::make_index_t<QByteArray>>(&(*interfaceIt).makeIndex)
                            ->insert(binding.condition.bytes, binding);
                        break;
                }
            }
            else
            {
                (*interfaceIt).makeIndex = std::monostate {};
                (*interfaceIt).conditionalMakeIndexedParameter = -1;
            }
            
            (*interfaceIt).conditionalMakes.append(binding);
        }
    }
    
    for (const auto& binding : noDetach((*moduleIt).getBindings))
    {
        std::list<InterfaceBindingConfig::InterfaceEntry>::iterator interfaceIt;
        
        auto find = _injectorConfig._index_byInterface.find(*binding.interfaceType);
        if (find != _injectorConfig._index_byInterface.end())
        {
            interfaceIt = *find;
        }
        else
        {
            interfaceIt = _injectorConfig._interfaces.insert(
                    _injectorConfig._interfaces.end(), { *binding.interfaceType }
                );
            _injectorConfig._index_byInterface[*binding.interfaceType] = interfaceIt;
        }
        
        (*interfaceIt).get = binding;
    }
}

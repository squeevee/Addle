#include "configexceptions.hpp"


#ifdef ADDLE_DEBUG

#include <boost/core/demangle.hpp>
#include <boost/range/adaptor/transformed.hpp>


#include <QDebug>
#include <QMetaType>
#include "utilities/debugging/exceptionproperties.hpp"

#include "./bindingcondition.hpp"
#include <utilities/ranges.hpp>

using namespace Addle;

aux_config3::InterfaceBindingNotFoundException::InterfaceBindingNotFoundException(
        Why why, const std::type_info& interfaceType, 
        QList<QSharedPointer<BindingCondition>> failedConditions,
        std::function<QString()> paramsToString
    )
    : AddleException(
        AddleExceptionBuilder(
            [why](){
                switch(why)
                {
                case NotBound:
                    //% "No bindings were found for the given interface."
                    return qtTrId("debug-messages.interface-not-bound");
                    
                case ConditionFailure:
                    //% "One or more conditional bindings for the given "
                    //% "interface were found, but all bindings found rejected "
                    //% "and/or did not match the given factory parameter(s)."
                    return qtTrId("debug-messages.interface-condition-failure");
                    
                default:
                    Q_UNREACHABLE();
                }
            }()
        )
    ),
    _why(why),
    _interfaceType(interfaceType),
    _failedConditions(std::move(failedConditions)),
    _paramsToString(std::move(paramsToString))
{
}

AddleException::debug_data_t 
aux_config3::InterfaceBindingNotFoundException::debugData() const
{
    using namespace boost::adaptors;
    return {
            { "why", aux_debug::debugString(_why) },
            { "interfaceType", QString::fromStdString(boost::core::demangle(_interfaceType.name())) },
            { 
                "failedConditions", 
                aux_debug::debugString(
                    qToList(noDetach(_failedConditions) | transformed([](const auto& c) { return *c; })) 
                ) 
            },
            { "parameters", (_paramsToString ? _paramsToString() : QString()) }
        };
}
    
static QDebug operator<<(QDebug debug, const aux_config3::ModuleConfigCommitFailure::ShadowedBinding& binding)
{
    debug << "ShadowedBinding(";
    bool prependComma = false;
    
    if (binding.condition && !binding.condition->isNull())
    {
        debug << "condition:" << (*binding.condition);
        prependComma = true;
    }
    
    if (binding.interfaceType)
    {
        if (prependComma)
            debug << ", ";
        
        auto name = boost::core::demangle(binding.interfaceType->name());
        debug << "interface: <" << name.data() << '>';
        prependComma = true;
    }
    
    if (binding.implType)
    {
        if (prependComma)
            debug << ", ";
        
        auto name = boost::core::demangle(binding.implType->name());
        debug << "impl: <" << name.data() << '>';
    }
    
    return debug << ')';
}

aux_config3::ModuleConfigCommitFailure::ModuleConfigCommitFailure(
        QByteArray id, bool idAlreadCommitted, 
        QList<ShadowedBinding> shadowedBindings)
    : AddleException(
        AddleExceptionBuilder(
            //% "The module configuration could not be committed."
            qtTrId("debug-messages.module-config-commit-failure")
        )
        .addProperty("id", aux_debug::exceptionProperty(&ModuleConfigCommitFailure::_id))
        .addProperty("idAlreadyCommitted", aux_debug::exceptionProperty(&ModuleConfigCommitFailure::_idAlreadyCommitted))
        .addProperty("shadowedBindings", aux_debug::exceptionProperty(&ModuleConfigCommitFailure::_shadowedBindings))
    ), 
    _id(std::move(id)), _idAlreadyCommitted(idAlreadCommitted), 
    _shadowedBindings(std::move(shadowedBindings))
{
}

QString aux_config3::_debugStringHelper<QByteArray>::get(QByteArray x) { return aux_debug::debugString(x); }
QString aux_config3::_debugStringHelper<QByteArray>::get(QList<QByteArray> x) { return aux_debug::debugString(x); }

QString aux_config3::_debugStringHelper<QString>::get(QString x) { return aux_debug::debugString(x); }
QString aux_config3::_debugStringHelper<QString>::get(QList<QString> x) { return aux_debug::debugString(x); }

QString aux_config3::_debugStringHelper<int>::get(int x) { return aux_debug::debugString(x); }
QString aux_config3::_debugStringHelper<int>::get(QList<int> x) { return aux_debug::debugString(x); }

QString aux_config3::_debugStringHelper<std::size_t>::get(std::size_t x) { return aux_debug::debugString(x); }
QString aux_config3::_debugStringHelper<std::size_t>::get(QList<std::size_t> x) { return aux_debug::debugString(x); }

QString aux_config3::_debugStringHelper<void*>::get(void* x) { return aux_debug::debugString(x); }
QString aux_config3::_debugStringHelper<void*>::get(QList<void*> x) { return aux_debug::debugString(x); }


aux_config3::GenericRepoKeysNotFoundException::GenericRepoKeysNotFoundException()
    : AddleException(
        AddleExceptionBuilder(
            //% "This repository did not contain one or more of the given key(s)."
            qtTrId("debug-messages.repo-keys-not-found-error")
        )
        .addProperty("keys (generic)", aux_debug::exceptionProperty(&GenericRepoKeysNotFoundException::keysGeneric))
    )
{
}

aux_config3::GenericRepoKeysAlreadyAddedException::GenericRepoKeysAlreadyAddedException()
    : AddleException(
        AddleExceptionBuilder(
            //% "This repository already contained one or more of the given key(s)."
            qtTrId("debug-messages.repo-keys-already-added-error")
        )
        .addProperty("keys (generic)", aux_debug::exceptionProperty(&GenericRepoKeysAlreadyAddedException::keysGeneric))
    )
{
}

aux_config3::RepoLazyInitNotSupportedException::RepoLazyInitNotSupportedException()
    : RepoLogicError(
        //% "This repository does not support lazy initialization."
        qtTrId("debug-messages.repo-lazy-init-not-supported")
    )
{
}

aux_config3::RepoKeyDeductionNotSupportedException::RepoKeyDeductionNotSupportedException()
    : RepoLogicError(
        //% "This repository does not support key deduction."
        qtTrId("debug-messages.repo-key-deduction-not-supported")
    )
{
}

aux_config3::GenericRepoVariantConversionFailure::GenericRepoVariantConversionFailure(
        Which which, QVariant original, int expectedType
    )
    : RepoLogicError(
        AddleExceptionBuilder(
            //% "Failed to convert a given variant to the type expected by this "
            //% "repository."
            qtTrId("debug-messages.repo-key-conversion-failure")
        )
        .addProperty("which",        aux_debug::exceptionProperty(&GenericRepoVariantConversionFailure::_which))
        .addProperty("original",     aux_debug::exceptionProperty(&GenericRepoVariantConversionFailure::_original))
        .addProperty("expectedType", &GenericRepoVariantConversionFailure::_expectedType_impl)
    )
    , _which(which)
    , _original(std::move(original))
    , _expectedType(expectedType)
{
}

QString aux_config3::GenericRepoVariantConversionFailure::_expectedType_impl(const AddleException& ex)
{
    const auto& ex_ = static_cast<const GenericRepoVariantConversionFailure&>(ex);
    const char* name = QMetaType::typeName(ex_._expectedType);
    
    if (!name)
    {
        //% "(Unnamed type with metatype ID `%1`)"
        return qtTrId("debug-messages.repo-key-conversion-failure.unnamed-type")
            .arg(ex_._expectedType);
    }
    
    return QString(name);
}

aux_config3::GenericRepoLazyInitError::GenericRepoLazyInitError(inner_exception_t inner, bool isStale)
    : AddleException(AddleExceptionBuilder(
            //% "An error occurred while attempting to lazily initialize a "
            //% "member of this repository."
            qtTrId("debug-messages.repo-member-lazy-init-error")
        )
        .setInnerExceptions({ std::move(inner) })
        .addProperty("key (generic)", aux_debug::exceptionProperty(&GenericRepoLazyInitError::keyGeneric))
        .addProperty("isStale",       aux_debug::exceptionProperty(&GenericRepoLazyInitError::_isStale))
    ), _isStale(isStale)
{
}

QString aux_config3::GenericRepoLazyInitError::_isStaleProperty_impl() const
{
    return aux_debug::debugString(_isStale);
}

#endif

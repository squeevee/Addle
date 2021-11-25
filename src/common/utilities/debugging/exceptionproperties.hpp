#pragma once

#ifndef ADDLE_DEBUG
#error "This file must only be included in debug builds, preferably only by .cpp files of exception classes"
#endif

#include <type_traits>

#include "exceptions/addleexception.hpp"

#include "./qdebug_extensions.hpp"
#include <QMetaEnum>

namespace Addle::aux_debug {
  
template<typename F, typename T, class C, bool = std::is_enum_v<T>>
struct exceptionProperty_impl
{
    QString operator()(const AddleException& ex) const
    {
        return debugString( std::invoke( f, static_cast<const C&>(ex)) );
    }
    F f;
};


template<typename F, typename T, class C>
struct exceptionProperty_impl<F, T, C, true>
{
    QString operator()(const AddleException& ex) const
    {
        auto m = QMetaEnum::fromType<T>();
        T v = std::invoke(f, static_cast<const C&>(ex));
        int i;
        
        if (m.isValid() && (i = m.value((int) v)) != -1)
            return m.key(i);
        else
            return QString::number((std::underlying_type_t<T>) v);
    }
    F f;
};
    
template<typename T, class C>
AddleException::AddleExceptionBuilder::property_t exceptionProperty(T C::*mem)
{
    return AddleException::AddleExceptionBuilder::property_t(exceptionProperty_impl<T C::*, T, C> { mem });
}

template<typename T, class C>
AddleException::AddleExceptionBuilder::property_t exceptionProperty(T (C::*mem)())
{
    return AddleException::AddleExceptionBuilder::property_t(exceptionProperty_impl<T (C::*)(), T, C> { mem });
}

template<typename F>
AddleException::AddleExceptionBuilder::property_t exceptionProperty(F&& f)
{
    return AddleException::AddleExceptionBuilder::property_t(
        exceptionProperty_impl< std::decay_t<F>, std::invoke_result_t<F, const AddleException&>, AddleException> { std::forward<F>(f) }
    );
}

}

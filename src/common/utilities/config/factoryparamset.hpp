/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License. 
 * See "LICENSE" for full details.
 */

#pragma once

#include <boost/mp11.hpp>
#include <boost/parameter.hpp>
#include <boost/type_traits.hpp>

#include "utilities/metaprogramming.hpp"

#if defined(ADDLE_DEBUG) || defined(ADDLE_TEST)
#include <QDebug>
#include "utilities/debugging/qdebug_extensions.hpp"
#endif

namespace Addle::config_detail {

template<typename T>
struct _is_factory_param_set : std::false_type {};

template<class TTag, typename TValue, class IsRequired>
struct FactoryParamInfo
{ 
    using tag_type = TTag;
    using value_type = TValue;
    static constexpr bool isRequired = IsRequired::value;
};

template<typename... ParamsInfo>
class FactoryParamSet
{   
    static_assert(boost::mp11::mp_is_map<FactoryParamSet>::value);
    
    template<class FactoryParamInfo_>
    using get_tag_type = typename FactoryParamInfo_::tag_type;
    
    template<class FactoryParamInfo_>
    using get_value_type = typename FactoryParamInfo_::value_type;
    
    template<class FactoryParamInfo_>
    using get_is_required = boost::mp11::mp_bool<FactoryParamInfo_::value>;
    
    template<class Tag>
    struct _match_tag
    {
        template<class FactoryParamInfo_>
        using fn = std::is_same<get_tag_type<FactoryParamInfo_>, Tag>;
    };
    
    // Internally r-value references are stored as l-values to allow addressof,
    // except for scalar types, which Boost.Parameter always passes by copy, and
    // so we store by copy.
    template<typename T>
    using storage_t = boost::mp11::mp_cond<
            std::is_scalar<std::remove_reference_t<T>>, boost::remove_cv_ref_t<T>,
            std::is_reference<T>,                       std::remove_reference_t<T>&,
            boost::mp11::mp_true,                       std::remove_cv_t<T>
        >;
        
    template<typename... Args>
    struct make_tuple_impl
    {
        template<typename Formal, typename Actual>
        using _forward_t = boost::mp11::mp_if<
                std::is_reference<Formal>,
                std::remove_reference_t<Actual>&,
                Actual&&
            >;
            
        template<std::size_t... Is>
        static auto make(std::index_sequence<Is...>, Args&&... args)
        {
            auto args_ = std::forward_as_tuple(args...);
            return std::tuple<storage_t<get_value_type<ParamsInfo>>...> {
                    static_cast<_forward_t<
                        get_value_type<boost::mp11::mp_at_c<FactoryParamSet, Is>>,
                        boost::mp11::mp_at_c<boost::mp11::mp_list<Args&&>, Is>
                    >>(std::get<Is>(args_))...
                };
        }
    };
    
    template<typename... Args>
    static auto make_tuple(Args&&... args)
    {
        return make_tuple_impl<Args...>::make(std::make_index_sequence<sizeof...(ParamsInfo)>(), std::forward<Args>(args)...);
    }
    
    // A const "reference" to a scalar type will actually be a prvalue, again
    // owing to the slightly idiomatic way Boost.Parameter treats scalar types
    template<class Tag>
    using _value_const_ref_t = boost::mp11::mp_if<
            std::is_scalar<std::remove_reference_t< get_value_type<boost::mp11::mp_map_find<FactoryParamSet, Tag>> >>,
            std::decay_t< get_value_type<boost::mp11::mp_map_find<FactoryParamSet, Tag>> >,
            const get_value_type<boost::mp11::mp_map_find<FactoryParamSet, Tag>>&
        >;
    
public:
    using tag_types     = boost::mp11::mp_list<get_tag_type<ParamsInfo>...>;
    using value_types   = boost::mp11::mp_list<get_value_type<ParamsInfo>...>;
        
    template<class Tag>
    using get_index_of = boost::mp11::mp_find_if_q<FactoryParamSet, _match_tag<Tag>>;
    
    template<class Tag>
    using get_value_type_of = get_value_type<boost::mp11::mp_map_find<FactoryParamSet, Tag>>;
        
    template<class Tag>
    using is_required = get_is_required<boost::mp11::mp_map_find<FactoryParamSet, Tag>>;
    
    FactoryParamSet(const FactoryParamSet&) = default; // Delete?
    FactoryParamSet(FactoryParamSet&&) = default;
    
    FactoryParamSet& operator=(const FactoryParamSet&) = delete;
    FactoryParamSet& operator=(FactoryParamSet&&) = delete;
    
    template<
        typename ArgumentPack,
        std::enable_if_t<
            boost::mp11::mp_or<
                boost::parameter::is_argument_pack<boost::remove_cv_ref_t<ArgumentPack>>,
                _is_factory_param_set<boost::remove_cv_ref_t<ArgumentPack>>
            >::value,
            void*
        > = nullptr
    >
    FactoryParamSet(ArgumentPack&& args)
        : _values(make_tuple( args[boost::parameter::keyword<get_tag_type<ParamsInfo>>::instance]... ))
    {
    }
    
    template<typename Tag>
    inline get_value_type_of<Tag>& get()
    { 
        return static_cast<get_value_type_of<Tag>&>(std::get<get_index_of<Tag>::value>(_values)); 
    }
    template<typename Tag>
    inline _value_const_ref_t<Tag> get() const 
    { 
        return static_cast<_value_const_ref_t<Tag>>(std::get<get_index_of<Tag>::value>(_values)); 
    }
    
    template<typename Tag>
    inline get_value_type_of<Tag>& operator[](const boost::parameter::keyword<Tag>&) { return this->get<Tag>(); }
    template<typename Tag>
    inline _value_const_ref_t<Tag> operator[](const boost::parameter::keyword<Tag>&) const { return this->get<Tag>(); }
    
    template<typename Tag>
    inline std::remove_reference_t<get_value_type_of<Tag>>* addressof(){ return std::addressof(std::get<get_index_of<Tag>::value>(_values)); }
    template<typename Tag>
    inline std::remove_reference_t<_value_const_ref_t<Tag>>* addressof() const { return std::addressof(std::get<get_index_of<Tag>::value>(_values)); }
    
private:
    std::tuple<storage_t<get_value_type<ParamsInfo>>...> _values;
};

template<typename... Specs>
struct _is_factory_param_set<FactoryParamSet<Specs...>> : std::true_type {};

#if defined(ADDLE_DEBUG) || defined(ADDLE_TEST)
template<typename ParamInfo, bool = aux_debug::has_qdebug_stream<typename ParamInfo::value_type>::value>
struct _factoryParamSetDebugHelper
{
    template<typename ParamSet>
    void operator()(QDebug debug, const ParamSet& params) const
    {
        debug << (const char*)(ParamInfo::tag_type::keyword_name())
              << ":" 
              << params[boost::parameter::keyword<typename ParamInfo::tag_type>::instance];
    }
};

template<typename ParamInfo>
struct _factoryParamSetDebugHelper<ParamInfo, false>
{
    template<typename _>
    void operator()(QDebug debug, const _&) const
    {
        debug << (const char*)(ParamInfo::tag_type::keyword_name())
              << ": <no string>";
    }
};

template<typename... ParamsInfo>
QDebug operator<< (QDebug debug, const FactoryParamSet<ParamsInfo...>& params)
{
    debug << "FactoryParamSet(";
    
    const int _[] = { (_factoryParamSetDebugHelper<ParamsInfo>{}(debug, params), 0)... };
    Q_UNUSED(_);
    
    return debug << ')';
}
#endif

} // namespace Addle::config_detail

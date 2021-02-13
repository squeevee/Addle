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

#include "utilities/metaprogramming.hpp"

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
    template<typename FactoryParamInfo_>
    using _map_entry = boost::mp11::mp_list<
            typename FactoryParamInfo_::tag_type,
            FactoryParamInfo_
        >;
    
    using type_map = boost::mp11::mp_list<_map_entry<ParamsInfo>...>;
    
    static_assert(boost::mp11::mp_is_map<type_map>::value);
    
    template<class FactoryParamInfo_>
    using get_tag_type = typename boost::mp11::mp_back<FactoryParamInfo_>::tag_type;
    
    template<class FactoryParamInfo_>
    using get_value_type = typename boost::mp11::mp_back<FactoryParamInfo_>::value_type;
    
    template<class FactoryParamInfo_>
    using get_is_required = boost::mp11::mp_bool<boost::mp11::mp_back<FactoryParamInfo_>::value>;
    
    template<class Tag>
    struct _front_matches 
    {
        template<class FactoryParamInfo_>
        using fn = std::is_same<
                boost::mp11::mp_front<FactoryParamInfo_>,
                Tag
            >;
    };
    
public:
    using tag_types = boost::mp11::mp_transform<
            get_tag_type,
            type_map
        >;
    
    using value_types = boost::mp11::mp_transform<
            get_value_type,
            type_map
        >;
        
    template<class Tag>
    using get_index_of = boost::mp11::mp_find_if_q<
            type_map,
            _front_matches<Tag>
        >;
        
    template<class Tag>
    using get_value_type_of = get_value_type<boost::mp11::mp_map_find<type_map, Tag>>;
    
    template<class Tag>
    using is_required = get_is_required<boost::mp11::mp_map_find<type_map, Tag>>;
    
    FactoryParamSet(const FactoryParamSet&) = default; // Delete?
    FactoryParamSet(FactoryParamSet&&) = default;
    
    FactoryParamSet& operator=(const FactoryParamSet&) = delete;
    FactoryParamSet& operator=(FactoryParamSet&&) = delete;
    
    template<
        typename ArgumentPack,
        std::enable_if_t<
            boost::mp11::mp_or<
                boost::parameter::is_argument_pack<ArgumentPack>,
                _is_factory_param_set<ArgumentPack>
            >::value,
            void*
        > = nullptr
    >
    FactoryParamSet(const ArgumentPack& args)
        : _values(generate_tuple_over_list<type_map>(
            [&args](auto t) -> auto&& {
                using Tag = typename boost::mp11::mp_back<typename decltype(t)::type>::tag_type;
                return args[boost::parameter::keyword<Tag>::instance];
            }
        ))
    {
    }
    
    template<typename Tag>
    inline get_value_type_of<Tag> get() const
    {
        return std::get<get_index_of<Tag>::value>(_values);
    }
    
    template<typename Tag>
    inline get_value_type_of<Tag> operator[](const boost::parameter::keyword<Tag>&) const { return this->get<Tag>(); }
    
private:
    boost::mp11::mp_apply<std::tuple, value_types> _values;
};

template<typename... Specs>
struct _is_factory_param_set<FactoryParamInfo<Specs...>> : std::true_type {};

} // namespace Addle::config_detail

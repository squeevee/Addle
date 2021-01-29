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

#include "utilities/generate_tuple_over.hpp"

namespace Addle {

template<class TTag, typename TValue>
struct TagValuePair { using tag_type = TTag; using value_type = TValue; };

/**
 * Maps tag types to heterogeneous values. Can be populated from a
 * Boost.Parameter argument pack.
 */
template<typename... TagValuePairs>
class TaggedValueSet
{
    using type_map = boost::mp11::mp_list<TagValuePairs...>;
    
    static_assert(boost::mp11::mp_is_map<type_map>::value);
    
    template<class TagValuePair_>
    using get_tag_type = typename TagValuePair_::tag_type;
    
    template<class TagValuePair_>
    using get_value_type = typename TagValuePair_::value_type;
    
    template<class Tag>
    struct _front_matches {
        template<class TagValuePair_>
        using fn = std::is_same<
                boost::mp11::mp_front<TagValuePair_>,
                Tag
            >;
    };
    
    template<class Tag>
    using get_index_of = boost::mp11::mp_find_if_q<
            type_map,
            _front_matches<Tag>
        >;
    
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
    using get_value_type_of = get_value_type<boost::mp11::mp_map_find<type_map, Tag>>;
    
    TaggedValueSet() = default;
    TaggedValueSet(const TaggedValueSet&) = default;
    TaggedValueSet(TaggedValueSet&&) = default;
    
    template<typename ArgumentPack>
    TaggedValueSet(const ArgumentPack& args)
        : _values(generate_tuple_over<type_map>(
            [&args](auto i) -> auto&& {
                using Tag = typename decltype(i)::type::tag_type;
                return args[boost::parameter::keyword<Tag>::instance];
            }
        ))
    {
        static_assert(boost::parameter::is_argument_pack<ArgumentPack>::value);
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

} // namespace Addle

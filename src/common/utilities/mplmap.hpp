/**
 * Utilities for accessing Boost.MPL functionality from runtime (e.g., based on
 * the underlying type of a boost::variant to a correlated sequence)
 */

#ifndef MPLRUNTIME_HPP
#define MPLRUNTIME_HPP

#include <boost/mpl/size.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/for_each.hpp>

#include "utilities/errors.hpp"

namespace Addle {

// Thanks to Stack Overflow user 101010
// https://stackoverflow.com/questions/36453874/is-it-possible-to-loop-over-template-parameters
    
template <typename T>
struct _mpl_map_wrapper {};

template <typename T>
struct _mpl_map_default_initializer
{
   template<typename Key>
   inline static T make()
   {
       return T();
   }
};

/**
 * @class 
 * @brief Maps values of type TValue to types in KeySequence (also accessible by
 * the int position of a given key type in KeySequence).
 * 
 * @tparam Initializer 
 * Must be a class with a static template function make() with return type TValue.
 */
template <typename KeySequence, typename TValue, typename Initializer = _mpl_map_default_initializer<TValue>>
class MPLMap
{
public:
    static constexpr std::size_t size = boost::mpl::size<KeySequence>::type::value;
    
    MPLMap()
    {
        boost::mpl::for_each<
            KeySequence, _mpl_map_wrapper<boost::mpl::placeholders::_1>
        >(visitor_init(_data));
    }
    
    /// @note KeySequence must be random-accessible.
    template<typename Key>
    inline const TValue& get() const
    {
        static_assert(
            boost::mpl::contains<KeySequence, Key>::type::value,
            "KeySequence does not contain the given type"
        );
        
        return _data[
            boost::mpl::find<KeySequence, Key>::type::pos::value
        ];
    }
    
    /// @note KeySequence must be random-accessible.
    template<typename Key>
    inline TValue& get()
    {
        static_assert(
            boost::mpl::contains<KeySequence, Key>::type::value,
            "KeySequence does not contain the given type"
        );
        
        return _data[
            boost::mpl::find<KeySequence, Key>::type::pos::value
        ];
    }
    
    inline const TValue& get(int index) const
    {
        ADDLE_ASSERT(index >= 0);
        ADDLE_ASSERT(index < size);
        
        return _data[index];
    }
    
    inline TValue& get(int index)
    {
        ADDLE_ASSERT(index >= 0);
        ADDLE_ASSERT(index < size);
        
        return _data[index];
    }
    
    template<typename Variant>
    inline const TValue& get(const Variant& var) const
    {
        static_assert(
            boost::mpl::size<typename Variant::types>::type::value == size,
            "The variant's type sequence is not the same size as KeySequence."
        );
        return _data[static_cast<std::size_t>(var.which())];
    }
    
    template<typename Variant>
    inline TValue& get(const Variant& var)
    {
        static_assert(
            boost::mpl::size<typename Variant::types>::type::value == size,
            "The variant's type sequence is not the same size as KeySequence."
        );
        return _data[static_cast<std::size_t>(var.which())];
    }
    
    template<typename Key>
    inline void set(const TValue& value) const
    {
        static_assert(
            boost::mpl::contains<KeySequence, Key>::type::value,
            "The sequence did not contain the given type"
        );
        
        _data[
            boost::mpl::find<KeySequence, Key>::type::pos::value
        ] = value;
    }
    
    inline void set(int index, const TValue& value) const
    {
        ADDLE_ASSERT(index >= 0);
        ADDLE_ASSERT(index < size);
        
        _data[index] = value;
    }
    
    inline const TValue& operator[](int index) const { return get(index); }
    inline TValue& operator[](int index)
    {
        ADDLE_ASSERT(index >= 0);
        ADDLE_ASSERT(index < size);
        
        return _data[index];
    }
private:
    struct visitor_init
    {
        visitor_init(TValue (&data_)[size])
            : data(data_)
        {
        }
        
        template<typename Key>
        void operator()(_mpl_map_wrapper<Key>) const
        {
            
            data[index++] = Initializer::template make<Key>();
        }
        
        mutable int index = 0;
        TValue (&data)[size];
    };
    
    TValue _data[size];
};

/**
 * @class
 * @brief MPLMap that infers its value type from the provided Initializer.
 */
template<typename KeySequence, typename Initializer>
class MPLAutoMap : public MPLMap<decltype(Initializer::template make<boost::mpl::placeholders::_1>()), KeySequence, Initializer>
{
    typedef decltype(Initializer::template make<boost::mpl::placeholders::_1>()) type;
};

} // namespace Addle;

#endif // MPLRUNTIME_HPP

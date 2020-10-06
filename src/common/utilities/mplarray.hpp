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
#include <boost/mpl/is_sequence.hpp>

#include "utilities/errors.hpp"

namespace Addle {

// Thanks to Stack Overflow user 101010
// https://stackoverflow.com/questions/36453874/is-it-possible-to-loop-over-template-parameters
    
template <typename T>
struct _mpl_mapped_array_wrapper {};

template <typename T>
struct _mpl_mapped_array_default_initializer
{
   template<typename Key> inline T operator()() const { return T(); }
};

/**
 * @class 
 * @brief An array of TValue mapped to member types of KeySequence by index
 * 
 * @tparam KeySequence 
 * MPL-compatible forward sequence of types
 * 
 * @tparam Initializer 
 * Optional. Must be a class with a operator() taking a type parameter Key that
 * is the current member of KeySequence and returning the TValue value
 * corresponding to that key.
 */
template<
    typename KeySequence,
    typename TValue,
    typename ValueInitializer = _mpl_mapped_array_default_initializer<TValue>
>
class MPLMappedArray
{
    static_assert(
        boost::mpl::is_sequence<KeySequence>::type::value,
        "KeySequence must be an MPL-compatible forward sequence"
    );
public:
    static constexpr std::size_t size = boost::mpl::size<KeySequence>::type::value;
    
    inline MPLMappedArray()
    {
        ValueInitializer valueInitializer;
        boost::mpl::for_each<
            KeySequence, _mpl_mapped_array_wrapper<boost::mpl::placeholders::_1>
        >(functor_init(_data, valueInitializer));
    }
    
    inline MPLMappedArray(ValueInitializer& valueInitializer)
    {
        boost::mpl::for_each<
            KeySequence, _mpl_mapped_array_wrapper<boost::mpl::placeholders::_1>
        >(functor_init(_data, valueInitializer));
    }
    
    inline MPLMappedArray(const ValueInitializer& valueInitializer)
    {
        boost::mpl::for_each<
            KeySequence, _mpl_mapped_array_wrapper<boost::mpl::placeholders::_1>
        >(functor_init(_data, valueInitializer));
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
    
    inline TValue* begin() { return _data; }
    inline const TValue* begin() const { return _data; }
    
    inline TValue* end() { return (TValue*)_data + size; }
    inline const TValue* end() const { return (const TValue*)_data + size; }
    
private:
    struct functor_init
    {
        functor_init(TValue (&data_)[size], ValueInitializer& init_)
            : data(data_), init(init_)
        {
        }
        
        template<typename Key>
        void operator()(_mpl_mapped_array_wrapper<Key>) const
        {
            
            data[index++] = init.template operator()<Key>();
        }
        
        mutable int index = 0;
        TValue (&data)[size];
        ValueInitializer& init;
    };
    
    TValue _data[size];
};

/**
 * @class
 * @brief MPLMappedArray that infers its value type from the provided Initializer.
 */
template<typename KeySequence, typename Initializer>
class MPLAutoMappedArray : public MPLMappedArray<
    decltype(
        std::declval<Initializer>()
            .template operator()<boost::mpl::placeholders::_1>()
    ), KeySequence, Initializer>
{
public:
    typedef decltype(
        std::declval<Initializer>()
            .template operator()<boost::mpl::placeholders::_1>()
    ) type;
    
    MPLAutoMappedArray() = default;
    MPLAutoMappedArray(Initializer& init)
        : MPLMappedArray<type, KeySequence, Initializer>(init)
    {
    }
    MPLAutoMappedArray(const Initializer& init)
        : MPLMappedArray<type, KeySequence, Initializer>(init)
    {
    }
};

} // namespace Addle;

#endif // MPLRUNTIME_HPP

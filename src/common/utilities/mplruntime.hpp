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
struct _mpl_runtime_nil {};

template <typename T>
struct _mpl_runtime_default_initializer
{
   template<typename Key>
   static T make()
   {
       return T();
   }
};

template <typename T, typename Sequence, typename Initializer = _mpl_runtime_default_initializer<T>>
class mpl_runtime_array
{
public:
    static constexpr int size = boost::mpl::size<Sequence>::type::value;
    
    mpl_runtime_array()
    {
        boost::mpl::for_each<
            Sequence, _mpl_runtime_nil<boost::mpl::placeholders::_1>
        >(visitor_init(_data));
    }
    
    template<typename Key>
    inline const T& get() const
    {
        static_assert(
            boost::mpl::contains<Sequence, Key>::type::value,
            "The sequence did not contain the given type"
        );
        
        return _data[
            boost::mpl::find<Sequence, Key>::type::pos::value
        ];
    }
    
    inline const T& get(int index) const
    {
        ADDLE_ASSERT(index >= 0);
        ADDLE_ASSERT(index < size);
        
        return _data[index];
    }
    
    template<typename Key>
    inline void set(const T& value) const
    {
        static_assert(
            boost::mpl::contains<Sequence, Key>::type::value,
            "The sequence did not contain the given type"
        );
        
        _data[
            boost::mpl::find<Sequence, Key>::type::pos::value
        ] = value;
    }
    
    inline void set(int index, const T& value) const
    {
        ADDLE_ASSERT(index >= 0);
        ADDLE_ASSERT(index < size);
        
        _data[index] = value;
    }
    
    inline const T& operator[](int index) const { return get(index); }
    inline T& operator[](int index)
    {
        ADDLE_ASSERT(index >= 0);
        ADDLE_ASSERT(index < size);
        
        return _data[index];
    }
private:
    struct visitor_init
    {
        visitor_init(T (&data_)[size])
            : data(data_)
        {
        }
        
        template<typename Key>
        void operator()(_mpl_runtime_nil<Key>) const
        {
            
            data[index++] = Initializer::template make<Key>();
        }
        
        mutable int index = 0;
        T (&data)[size];
    };
    
    T _data[size];
};

    
    
} // namespace Addle;

#endif // MPLRUNTIME_HPP

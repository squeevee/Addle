#pragma once

#include <boost/multi_array.hpp>
#include <boost/multi_array/concept_checks.hpp>
#include <boost/iterator.hpp>
#include <boost/type_traits.hpp>

#include <boost/range/iterator_range.hpp>
#include <boost/mp11.hpp>
#include <array>

#include <boost/range/iterator_range.hpp>

namespace Addle::aux_multi_array_util {
    
// I don't know how well this interacts with non-default storage ordering or 
// non-zero index bases (but especially negative ones).
    
// A traverser whose behavior is controlled at runtime instead of compile time
// could be useful.
    
template<typename T, std::size_t NDims>
class traverser_base
{
protected:
    using size_type     = boost::multi_array_types::size_type;
    using index_type    = boost::multi_array_types::index;
    
    traverser_base() = default;
    traverser_base(const traverser_base&) = default;
    
    traverser_base(T* origin, const size_type* shape, const index_type* strides,      
        const index_type* bases, size_type pitch, index_type cursor = 0)
        : _origin(origin),
        _pitch(pitch),
        _cursor(cursor)
    {
        std::copy(shape, shape + NDims, _shape.begin());
        std::copy(strides, strides + NDims, _strides.begin());
        std::copy(bases, bases + NDims, _bases.begin());
        
        update_indices();
    }
    
    template<typename IndexRange>
    traverser_base(T* origin, const size_type* shape, const index_type* strides,      
        const index_type* bases, size_type pitch, IndexRange&& indices)
        : _origin(origin),
        _pitch(pitch)
    {
        std::copy(shape, shape + NDims, _shape.begin());
        std::copy(strides, strides + NDims, _strides.begin());
        std::copy(bases, bases + NDims, _bases.begin());
        
        using std::begin;
        using std::end;
        
        auto inIt = begin(std::forward<IndexRange>(indices));
        auto inEnd = end(std::forward<IndexRange>(indices));
        
        auto base = _bases.begin();
        auto extent = _shape.begin();
        
        for (auto& index : _indices)
        {
            assert(inIt != inEnd);
            assert((*inIt) >= *base && (*inIt) < *extent);
            index = (*inIt);
            
            ++inIt;
            ++base;
            ++extent;
        }
        
        update_cursor();
    }
    
    void update_cursor()
    {
        _cursor = 0;
        auto stride = _strides.begin();
        auto base = _bases.begin();
        
        for (index_type index : _indices)
        {
            _cursor += (*stride) * (index - (*base));
            ++stride;
            ++base;
        }
    }
    
    void update_indices()
    {
        const index_type* stride = _strides;
        const size_type* extent = _shape;
        const index_type* base = _bases;
        
        auto indexIt = _indices.begin();
        auto indexEnd = _indices.end();
        
        for(; indexIt != indexEnd; ++indexIt)
        {
            (*indexIt) = (_cursor / (*stride)) % (*extent - *base) + *base;
            
            ++stride;
            ++extent;
            ++base;
        }
    }
    
    T*                  _origin;
    std::array<size_type, NDims> _shape;
    std::array<index_type, NDims> _strides;
    std::array<index_type, NDims> _bases;
    
    // pitch is the distance from _origin to the first memory location that is
    // not contained by the array.
    size_type           _pitch = 0;
    
    index_type          _cursor     = 0;
    std::array<index_type, NDims> _indices;
};

template<typename I1, typename I2>
using _integral_equal = boost::mp11::mp_bool<I1::value == I2::value>;

#ifdef ADDLE_DEBUG
template<typename DimPriority, typename NDims_>
using _is_valid_dim_priority_list_impl = 
    boost::mp11::mp_apply<
        boost::mp11::mp_all,
        boost::mp11::mp_transform<
            _integral_equal, 
            boost::mp11::mp_sort<DimPriority, boost::mp11::mp_less>, 
            boost::mp11::mp_iota<NDims_>
        >
    >;

template<typename DimPriority, std::size_t NDims>
using is_valid_dim_priority_list = 
    boost::mp11::mp_eval_if<
        std::is_void<DimPriority>,
        boost::mp11::mp_true,
        _is_valid_dim_priority_list_impl,
        DimPriority,
        boost::mp11::mp_size_t<NDims>
    >;
    
#endif // ADDLE_DEBUG

template<typename DimPriority, std::size_t IPriority>
using get_dim_priority = 
    boost::mp11::mp_eval_if<
        std::is_void<DimPriority>,
        boost::mp11::mp_size_t<IPriority>,
        boost::mp11::mp_at,
        DimPriority,
        boost::mp11::mp_size_t<IPriority>
    >;
    
template<typename T>
using _is_bool_t = std::is_convertible<decltype(T::value), bool>;

#ifdef ADDLE_DEBUG
template<typename DimReversals, typename NDims_>
using _is_valid_dim_reversal_list_impl =
    boost::mp11::mp_and<
        _integral_equal<boost::mp11::mp_size<DimReversals>, NDims_>,
        boost::mp11::mp_apply<
            boost::mp11::mp_all, 
            boost::mp11::mp_transform<
                _is_bool_t, 
                DimReversals
            >
        >
    >;

template<typename DimReversals, std::size_t NDims>
using is_valid_dim_reversal_list = 
    boost::mp11::mp_eval_if<
        std::is_void<DimReversals>,
        boost::mp11::mp_true,
        _is_valid_dim_reversal_list_impl,
        DimReversals,
        boost::mp11::mp_size_t<NDims>
    >;
#endif // ADDLE_DEBUG

template<typename DimReversals, std::size_t IDim>
using get_dim_reversal = 
    boost::mp11::mp_eval_if<
        std::is_void<DimReversals>,
        std::false_type,
        boost::mp11::mp_at,
        DimReversals,
        boost::mp11::mp_size_t<IDim>
    >;
    
template<
    typename T, 
    std::size_t NDims,
    typename DimPriority = void,
    typename DimReversals = void
>
class traverser : private traverser_base<T, NDims>
{
    static_assert(NDims > 0);
#ifdef ADDLE_DEBUG
    // disable this check for faster release compilation
    static_assert(is_valid_dim_priority_list<DimPriority, NDims>::value);
    static_assert(is_valid_dim_reversal_list<DimReversals, NDims>::value);
#endif
    
    using base = traverser_base<T, NDims>;
public:
    using size_type     = typename base::size_type;
    using index_type    = typename base::index_type;
    
    traverser() = default;
    traverser(const traverser&) = default;
    
    traverser(T* origin, const size_type* shape, const index_type* strides,      
        const index_type* bases, size_type pitch)
        : base(origin, shape, strides, bases, pitch)
    {
    }
    
    traverser(T* origin, const size_type* shape, const index_type* strides,      
        const index_type* bases, size_type pitch, T* pointer)
        : base(origin, shape, strides, bases, pitch, pointer - origin)
    {
    }
    
    template<typename IndexRange>
    traverser(T* origin, const size_type* shape, const index_type* strides,
        const index_type* bases, size_type pitch, IndexRange&& indices)
        : base(origin, shape, strides, bases, pitch, 
        std::forward<IndexRange>(indices))
    {
    }

    traverser(T* origin, const size_type* shape, const index_type* strides,
        const index_type* bases, size_type pitch, 
        const std::initializer_list<index_type>& indices)
        : base(origin, shape, strides, bases, pitch, indices)
    {
    }
    
    T* pointer() const { return this->_origin + this->_cursor; }
    
    void setPointer(T* pointer)
    {
        this->_cursor = pointer - this->_origin;
        this->update_indices();
    }
    
    const index_type* indices() const { return this->_indices.data(); }
    
    index_type x() const
    { 
        return this->_indices[0]; 
    }
    
    index_type y() const 
    { 
        
        if constexpr (NDims > 1)
        {
            return this->_indices[1];
        }
        else
        {
            // Most checks shouldn't be so forgiving, but for the y() and z()
            // accessors, we'll just give a warning in debug if there's no
            // dimension to access.
#ifdef ADDLE_DEBUG
            qWarning("%s", qUtf8Printable(
                //% "Accessed dimension %1 on traverser with only %2 "
                //% "dimension(s)."
                qtTrId("debug-messages.multi-array-util.index-access-too-few-dimensions")
                    .arg(1)
                    .arg(NDims)
            ));
#endif
            return 0;
        }
    }
    
    index_type z() const 
    { 
        if constexpr (NDims > 2)
        {
            return this->_indices[2]; 
        }
        else
        {
#ifdef ADDLE_DEBUG
            qWarning("%s", qUtf8Printable(
                qtTrId("debug-messages.multi-array-util.index-access-too-few-dimensions")
                    .arg(2)
                    .arg(NDims)
            ));
#endif
            return 0;
        }
    }
    
    template<std::size_t IDim>
    void moveIndex(index_type delta)
    {
        static_assert(NDims > IDim);

        const index_type base = this->_bases[IDim];
        const size_type width = this->_shape[IDim] - base;
        index_type& index = this->_indices[IDim];
        
        index = (index = ((index + delta - base) % width) < 0 ? index + width : index) + base;
        
        this->update_cursor();
    }
    
    void advance(index_type delta)
    {
        // thanks https://stackoverflow.com/a/23214219/2808947
        
        boost::mp11::mp_for_each<boost::mp11::mp_iota_c<NDims>>(
            [&] (auto priority) {
                constexpr std::size_t IDim = get_dim_priority<
                        DimPriority,
                        NDims - decltype(priority)::value - 1
                    >::value;
                
                constexpr bool IsReversed = get_dim_reversal<
                        DimReversals,
                        IDim
                    >::value;
                    
                index_type& index = this->_indices[IDim];
                const auto base = this->_bases[IDim];
                const index_type width = this->_shape[IDim] - base;
                
                if constexpr (IsReversed)
                {
                    index -= delta - base;
                    delta = (-index / width) + ((index < 0) ? 1 : 0);
                }
                else
                {
                    index += delta - base;
                    delta = (index / width) + ((index < 0) ? -1 : 0);
                }
                
                index = (((index %= width) < 0) ? index + width : index) + base;
            }
        );
        
        const index_type magnitude
            = ((this->_cursor / (index_type)this->_pitch) + ((this->_cursor < 0) ? -1 : 0));
        
        this->update_cursor();
        this->_cursor += (magnitude + delta) * this->_pitch;
    }
    
    index_type distance_to(const index_type* otherIndices, const T* otherPointer) const
    {
        index_type result = 0;
        boost::mp11::mp_for_each<boost::mp11::mp_iota_c<NDims>>(
            [&] (auto priority) {
                constexpr std::size_t IDim = get_dim_priority<
                        DimPriority,
                        NDims - decltype(priority)::value - 1
                    >::value;
                    
                constexpr bool IsReversed = get_dim_reversal<
                        DimReversals,
                        IDim
                    >::value;
                
                const index_type index = this->_indices[IDim];
                const index_type other_index = otherIndices[IDim];
                
                const index_type width = this->_shape[IDim] - this->_bases[IDim];
                
                result *= width;
                
                if constexpr (IsReversed)
                    result += index - other_index;
                else
                    result += other_index - index; 
            }
        );
                
        const index_type magnitude 
            = ((otherPointer - this->_origin) - this->_cursor) / (index_type)this->_pitch;
        
        return result + (magnitude * this->_pitch);
    }
    
};

template<typename T, typename Traverser>
class traverser_iterator : public boost::iterator_facade<
        traverser_iterator<T, Traverser>,
        T,
        std::random_access_iterator_tag,
        T&,
        boost::multi_array_types::index
    >
{
public:
    using index_type = boost::multi_array_types::index;
    traverser_iterator() = default;
    traverser_iterator(const traverser_iterator&) = default;
    
    traverser_iterator(const Traverser& traverser)
        : _traverser(traverser)
    {
    }
    
    Traverser& traverser() { return _traverser; }
    const Traverser& traverser() const { return _traverser; }
    
    const index_type* indices() const { return _traverser.indices(); }
    index_type x() const { return _traverser.x(); }
    index_type y() const { return _traverser.y(); }
    index_type z() const { return _traverser.z(); }
    
private:
    Traverser _traverser;
    
    T& dereference() const { return *(_traverser.pointer()); }
    
    template<typename U, typename OtherTraverser, std::enable_if_t<
            std::is_convertible_v<U*, T*>, void*> = nullptr>
    bool equal(const traverser_iterator<U, OtherTraverser>& other) const
    {
        return other.traverser().pointer() == _traverser.pointer();
    }
    
    void increment() { _traverser.advance(1); }
    void decrement() { _traverser.advance(-1); }
    void advance(index_type n) { _traverser.advance(n); }
    template<typename U, typename OtherTraverser, std::enable_if_t<
            std::is_convertible_v<U*, T*>, void*> = nullptr>
    index_type distance_to(const traverser_iterator<U, OtherTraverser>& other) const
    {
        return _traverser.distance_to(
                other.traverser().indices(),
                other.traverser().pointer()
            );
    }
    
    friend class boost::iterator_core_access;
};

template<typename MultiArray>
using _storage_order_t = decltype(std::declval<MultiArray>().storage_order());

template<
    typename DimPriority = void,
    typename DimReversals = void
>
struct traverse_builder
{
    template<typename MultiArray>
    inline auto operator()(MultiArray&& array) const
    {
        constexpr std::size_t NDims = std::decay_t<MultiArray>::dimensionality;
        boost::function_requires<
            boost::multi_array_concepts::ConstMultiArrayConcept<
                boost::remove_cv_ref_t<MultiArray>, NDims
            >>();
        
        using size_type = boost::multi_array_types::size_type;
        using index_type = boost::multi_array_types::index;
        
        using ptr_t = std::decay_t<decltype(array.origin())>;
        using value_t = typename std::decay_t<MultiArray>::template subarray<1>::type::value_type;
        
        using T = boost::mp11::mp_if<
                boost::mp11::mp_or<
                    std::is_const<std::remove_reference_t<MultiArray>>,
                    std::is_const<std::remove_pointer_t<ptr_t>>
                >,
                const value_t,
                value_t
            >;
            
        using traverser_t = traverser<T, NDims, DimPriority, DimReversals>;
        
        if (array.empty()) 
            return boost::iterator_range<traverser_iterator<T, traverser_t>>();
        
        T* origin = array.origin();
        const size_type* shape = array.shape();
        const index_type* strides = array.strides();
        const index_type* base = array.index_bases();
        
        std::array<index_type, NDims> beginIndices;
        
        boost::mp11::mp_for_each<boost::mp11::mp_iota_c<NDims>>(
            [&] (auto i) {
                constexpr std::size_t IDim = decltype(i)::value;
                
                constexpr bool IsReversed = get_dim_reversal<
                        DimReversals,
                        IDim
                    >::value;
                
                if constexpr (IsReversed)
                    beginIndices[IDim] = shape[IDim] - 1;
                else
                    beginIndices[IDim] = 0;
            }
        );
        
        size_type pitch;
        if constexpr (boost::mp11::mp_valid<_storage_order_t, const MultiArray>::value)
        {
            size_type i = array.storage_order().ordering(NDims - 1);
            pitch = strides[i] * shape[i];
            
#ifdef ADDLE_DEBUG
            assert(*std::max_element(strides, strides + NDims) == strides[i]);
#endif
        }
        else
        {
            auto maxStride = std::max_element(strides, strides + NDims);
            pitch = (*maxStride) * shape[maxStride - strides];
        }
        
        traverser_t beginTraverser(
                origin,
                shape,
                strides,
                base,
                pitch,
                std::move(beginIndices)
            );
        
        traverser_t endTraverser(beginTraverser);
        endTraverser.advance(array.num_elements());
        
        return boost::iterator_range<traverser_iterator<T, traverser_t>>(
                std::move(beginTraverser), 
                std::move(endTraverser)
            );
    }
};

// Creates a default traversal range over any multi array. For a two-dimensional
// array this traverses columns.
template<typename MultiArray>
inline auto traverse(MultiArray&& array)
{
    return traverse_builder<>{}(std::forward<MultiArray>(array));
}

// Creates a traversal range over a two-dimensional array such that the 
// traverser moves along rows, i.e., the first index, "x", increments with each
// iteration and the second index, "y", increments after the end of a row is 
// reached.
template<typename MultiArray>
inline auto traverse_rows(MultiArray&& array)
{
    static_assert(std::decay_t<MultiArray>::dimensionality == 2);
    return traverse_builder<
           boost::mp11::mp_list_c<std::size_t, 1, 0>
        >{}(std::forward<MultiArray>(array));
}

#ifdef ADDLE_DEBUG
template<typename MultiArray, 
    std::enable_if_t<
        boost::mp11::mp_valid<_storage_order_t, const MultiArray>::value, 
    void*> = nullptr>
inline void _validate_fast_traverse_array(const MultiArray& array)
{
    using size_type = boost::multi_array_types::size_type;
    
    const auto& o = array.storage_order();
    size_type expectedStride = 1;
    
    constexpr size_type NDims = std::decay_t<MultiArray>::dimensionality;
    static_assert(NDims > 0);
    for (std::size_t i = 0; i < NDims; ++i)
    {
        assert(!array.index_bases()[o.ordering(i)]);
        assert(array.strides()[o.ordering(i)] == expectedStride);
        expectedStride *= array.shape()[o.ordering(i)];
    }
}

template<typename MultiArray, 
    std::enable_if_t<
        !boost::mp11::mp_valid<_storage_order_t, const MultiArray>::value, 
    void*> = nullptr>
inline void _validate_fast_traverse_array(const MultiArray& array)
{
    using size_type = boost::multi_array_types::size_type;
    size_type expectedStride = 1;
    
    constexpr size_type NDims = std::decay_t<MultiArray>::dimensionality;
    static_assert(NDims > 0);
    for (std::size_t i = NDims; i > 0; --i)
    {
        assert(!array.index_bases()[i - 1]);
        assert(array.strides()[i - 1] == expectedStride);
        expectedStride *= array.shape()[i - 1];
    }
}
#endif

// If no special ordering or random access of indices during iteration is
// required, and the array has no index offsets or special striding, then a
// plain pointer can be used as a traversal iterator with the smallest possible
// overhead. Note that this can sometimes work for array views but only under 
// limited conditions that can't be verified at compile time.
template<typename MultiArray>
inline auto fast_traverse(MultiArray&& array)
{
    using size_type = boost::multi_array_types::size_type;
    constexpr size_type NDims = std::decay_t<MultiArray>::dimensionality;
    boost::function_requires<
        boost::multi_array_concepts::ConstMultiArrayConcept<
            boost::remove_cv_ref_t<MultiArray>, NDims
        >>();
    
#ifdef ADDLE_DEBUG
    _validate_fast_traverse_array(array);
#endif
    
    using ptr_t = std::decay_t<decltype(array.origin())>;
    return boost::iterator_range<ptr_t>(array.origin(), array.origin() + array.num_elements());
}

}

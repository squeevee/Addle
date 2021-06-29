#pragma once

#include <cstdint>
#include <variant>

#include <boost/mp11.hpp>
#include <boost/iterator.hpp>
#include <boost/container_hash/hash.hpp>

#include <QList>

#if defined(ADDLE_DEBUG) || defined(ADDLE_TEST)
#include <QtDebug>      // QDebug operator<< for NodeAddress
#endif

#ifdef ADDLE_TEST
#include <QTest>        // QTest::toString for NodeAddress
class DataTree_UTest;   // friend access for NodeAddress
#endif

#include "utilities/ranges.hpp"
#include "utilities/metaprogramming.hpp"

namespace Addle {
namespace aux_datatree {

// Generically represents the location of a node in a tree as a sequence of
// 0-based integer indices.
//
// TODO Using "small" and "fast" optimizations in parallel like this is 
// cumbersome from a maintenance standpoint. One should be eliminated, or they
// should be combined somehow.
// 
// Some preliminary benchmarking gives me the impresson that the "small" 
// optimization by itself is no slower at comparisons than the "fast" 
// optimization.
//
// Curiously, my benchmarking showed the "small" optimization taking longer to
// *construct* than no optimization (avoiding dynamic memory for speed is the 
// whole point of the small addresses). I suspect this could be aleviated with 
// some relatively small adjustments to NodeAddressBuilder.
//
// "Fast" optimizations give by far the best performance when hashing -- but 
// 1) hashing is not very important for us, 2) even unoptimized, it's still way
// faster than any other operation, 3) this was true even when addresses were
// hashed exactly once after construction, which could simply be an indication
// that the fast address scheme is better suited as a hashing algorithm.
//
// My takeaway is that more benchmarking and profiling is needed before anything
// definitive can be said about these optimizations.
//
// Note: we will almost surely be better off with no neither optimization on
// 32-bit platforms.

class NodeAddress
{
#ifndef ADDLE_NO_SMALL_NODEADDRESS
    using large_t = std::size_t;
    using large_data_t = QList<large_t>;
    
    using small_t = std::uint_least8_t;
    
    static constexpr std::size_t    SMALL_ADDRESS_CAPACITY = 
                boost::mp11::mp_max<
                    boost::mp11::mp_int<sizeof(large_data_t) / sizeof(small_t)>,
                    boost::mp11::mp_int<8>
                >::value;
    
    using small_data_t = small_t [SMALL_ADDRESS_CAPACITY];
    
    static constexpr std::size_t    SMALL_ADDRESS_MAX_VALUE = UINT_LEAST8_MAX;
    static constexpr unsigned short INVALID_SMALL_SIZE = USHRT_MAX;
#else    
    using index_t   = std::size_t;
    using list_t    = QList<index_t>;
#endif //ADDLE_NO_SMALL_NODEADDRESS
    
#ifndef ADDLE_NO_FAST_NODEADDRESS
    using fast_address_t = std::uint_fast64_t;
    
    static constexpr std::uint64_t  INVALID_FAST_ADDRESS = UINT_FAST64_MAX;
#endif // ADDLE_NO_FAST_NODEADDRESS
public:        
    using value_type        = std::size_t;
    using const_reference   = const std::size_t&;
    using reference         = const_reference;
    using difference_type   = std::ptrdiff_t;
    using size_type         = std::size_t;
    
#ifndef ADDLE_NO_SMALL_NODEADDRESS
    class Iterator : public boost::iterator_facade<
            Iterator,
            std::size_t,
            std::random_access_iterator_tag,
            std::size_t
        >
    {
    public:
        Iterator() = default;
        Iterator(const Iterator&) = default;
        
    private:
        Iterator(const small_t* base)
            : _base(base)
        {
        }
        
        Iterator(large_data_t::const_iterator base)
            : _base(base)
        {
        }
        
        
        std::size_t dereference() const
        {
            return std::visit(
                    [] (const auto& i) -> std::size_t { return *i; },
                    _base
                );
        }
        
        struct visitor_equal
        {
            template<typename T>
            bool operator()(const T& i, const T& j) const
            {
                return i == j;
            }
            
            template<typename T, typename U>
            bool operator()(const T&, const U&) const
            { 
                return false;
            }
        };
        
        bool equal(const Iterator& other) const
        {
            return std::visit(visitor_equal {}, _base, other._base);
        }
        
        void increment()
        {
            std::visit([] (auto& i) { ++i; }, _base);
        }
        
        void decrement()
        {
            std::visit([] (auto& i) { --i; }, _base);
        }
        
        void advance(std::ptrdiff_t n)
        {
            std::visit([n] (auto& i) { i += n; }, _base);
        }
        
        struct visitor_distance
        {
            template<typename T>
            std::ptrdiff_t operator()(const T& i, const T& j) const
            {
                return std::distance(i, j);
            }
            
            template<typename T, typename U>
            std::ptrdiff_t operator()(const T&, const U&) const
            {
#ifdef ADDLE_DEBUG
                qWarning("invalid nodeaddress iterator comparison");
#endif
                return 0;
            }
        };
        
        std::ptrdiff_t distance_to(const Iterator& other) const
        {
            return std::visit(visitor_distance {}, _base, other._base);
        }
        
        std::variant<const small_t*, large_data_t::const_iterator> _base;
        
        friend class boost::iterator_core_access;
        friend class NodeAddress;
    };
    using iterator          = Iterator;
    using const_iterator    = Iterator;
#else // defined ADDLE_NO_SMALL_NODEADDRESS
    using iterator          = list_t::const_iterator;
    using const_iterator    = list_t::const_iterator;
#endif // ADDLE_NO_SMALL_NODEADDRESS
    
    NodeAddress() = default;
    
#ifndef ADDLE_NO_SMALL_NODEADDRESS
    inline ~NodeAddress();
    inline NodeAddress(const NodeAddress&);
    inline NodeAddress(NodeAddress&&);
    
    inline NodeAddress& operator=(const NodeAddress&);
    inline NodeAddress& operator=(NodeAddress&&);
#else // ADDLE_NO_SMALL_NODEADDRESS
    inline NodeAddress(const NodeAddress&) = default;
    inline NodeAddress(NodeAddress&&) = default;
    
    inline NodeAddress& operator=(const NodeAddress&) = default;
    inline NodeAddress& operator=(NodeAddress&&) = default;
#endif // ADDLE_NO_SMALL_NODEADDRESS
    
    inline NodeAddress(const std::initializer_list<std::size_t>& init);
    
    inline bool operator==(const NodeAddress& other) const
    {
#ifndef ADDLE_NO_FAST_NODEADDRESS
        if (Q_LIKELY(
            _fastAddress != INVALID_FAST_ADDRESS 
            && other._fastAddress != INVALID_FAST_ADDRESS
        ))
        {
            return _fastAddress == other._fastAddress;
        }
        else
#endif // ADDLE_NO_FAST_NODEADDRESS
        {
#ifndef ADDLE_NO_SMALL_NODEADDRESS
            if (_smallSize != other._smallSize)
                return false;
            
            if (_smallSize != INVALID_SMALL_SIZE)
                return std::equal(
                        _smallData, 
                        _smallData + _smallSize, 
                        other._smallData);
            else
                return largeData() == other.largeData();
#else // ADDLE_NO_SMALL_NODEADDRESS
            return _data == other._data;
#endif // ADDLE_NO_SMALL_NODEADDRESS
        }
    }
    
    inline bool operator!=(const NodeAddress& other) const
    {
        return !(*this == other);
    }
    
    inline bool operator<(const NodeAddress& other) const
    {
#ifndef ADDLE_NO_FAST_NODEADDRESS
        if (Q_LIKELY(
            _fastAddress != INVALID_FAST_ADDRESS 
            && other._fastAddress != INVALID_FAST_ADDRESS
        ))
        {
            return _fastAddress < other._fastAddress;
        }
        else
#endif // ADDLE_NO_FAST_NODEADDRESS
        {            
#ifndef ADDLE_NO_SMALL_NODEADDRESS
            if (_smallSize != INVALID_SMALL_SIZE 
                    && other._smallSize != INVALID_SMALL_SIZE)
                return std::lexicographical_compare(
                            _smallData, _smallData + _smallSize,
                            other._smallData, other._smallData + other._smallSize
                        );
            else if (_smallSize == INVALID_SMALL_SIZE 
                    && other._smallSize != INVALID_SMALL_SIZE)
                return std::lexicographical_compare(
                            largeData().begin(), largeData().end(),
                            other._smallData, other._smallData + other._smallSize
                        );
            else if (_smallSize != INVALID_SMALL_SIZE
                    && other._smallSize == INVALID_SMALL_SIZE)
                return std::lexicographical_compare(
                            _smallData, _smallData + _smallSize,
                            other.largeData().begin(), other.largeData().end()
                        );
            else
                return std::lexicographical_compare(
                            largeData().begin(), largeData().end(),
                            other.largeData().begin(), other.largeData().end()
                        );
#else // ADDLE_NO_SMALL_NODEADDRESS
            return std::lexicographical_compare(
                    _data.begin(), _data.end(),
                    other._data.begin(), other._data.end()
                );
#endif // ADDLE_NO_SMALL_NODEADDRESS
        }
    }
    
    inline bool operator<=(const NodeAddress& other) const
    {
        return !(*this > other);
    }
    
    inline bool operator>(const NodeAddress& other) const
    {
#ifndef ADDLE_NO_FAST_NODEADDRESS
        if (Q_LIKELY(
            _fastAddress != INVALID_FAST_ADDRESS 
            && other._fastAddress != INVALID_FAST_ADDRESS
        ))
        {
            return _fastAddress > other._fastAddress;
        }
        else
#endif // ADDLE_NO_FAST_NODEADDRESS
        {
#ifndef ADDLE_NO_SMALL_NODEADDRESS
            if (_smallSize != INVALID_SMALL_SIZE 
                    && other._smallSize != INVALID_SMALL_SIZE)
                return std::lexicographical_compare(
                            other._smallData, other._smallData + other._smallSize,
                            _smallData, _smallData + _smallSize
                        );
            else if (_smallSize == INVALID_SMALL_SIZE 
                    && other._smallSize != INVALID_SMALL_SIZE)
                return std::lexicographical_compare(
                            other._smallData, other._smallData + other._smallSize,
                            largeData().begin(), largeData().end()
                        );
            else if (_smallSize != INVALID_SMALL_SIZE
                    && other._smallSize == INVALID_SMALL_SIZE)
                return std::lexicographical_compare(
                            other.largeData().begin(), other.largeData().end(),
                            _smallData, _smallData + _smallSize
                        );
            else
                return std::lexicographical_compare(
                            other.largeData().begin(), other.largeData().end(),
                            largeData().begin(), largeData().end()
                        );
#else // ADDLE_NO_SMALL_NODEADDRESS
            return std::lexicographical_compare(
                    other._data.begin(), other._data.end(),
                    _data.begin(), _data.end()
                );
#endif // ADDLE_NO_SMALL_NODEADDRESS
        }
    }
    
    inline bool operator>=(const NodeAddress& other) const
    {
        return !(*this < other);
    }
    
    inline std::size_t operator[](std::size_t depth) const
    { 
#ifndef ADDLE_NO_SMALL_NODEADDRESS
        if (_smallSize != INVALID_SMALL_SIZE)
        {
            assert(depth < _smallSize);
            return _smallData[depth];
        }
        else
        {
            return largeData()[depth];
        }
#else // ADDLE_NO_SMALL_NODEADDRESS
        return _data[depth];
#endif // ADDLE_NO_SMALL_NODEADDRESS
    }
    
    inline std::size_t lastIndex() const 
    {
        if (isRoot()) 
            return 0;
#ifndef ADDLE_NO_SMALL_NODEADDRESS
        else if (_smallSize != INVALID_SMALL_SIZE)
            return _smallData[_smallSize - 1];
        else
            return largeData().last();
#else // ADDLE_NO_SMALL_NODEADDRESS
        return _data.last();
#endif // ADDLE_NO_SMALL_NODEADDRESS
    }
    
    inline std::size_t size() const     
    { 
#ifndef ADDLE_NO_SMALL_NODEADDRESS
        return (_smallSize != INVALID_SMALL_SIZE) ? 
            _smallSize : 
            largeData().size(); 
#else // ADDLE_NO_SMALL_NODEADDRESS
        return _data.size();
#endif // ADDLE_NO_SMALL_NODEADDRESS
    }
    
    inline iterator begin() const
    { 
#ifndef ADDLE_NO_SMALL_NODEADDRESS
        if (_smallSize != INVALID_SMALL_SIZE)
            return iterator(_smallData);
        else
            return iterator(largeData().begin());
#else // ADDLE_NO_SMALL_NODEADDRESS
        return _data.begin();
#endif // ADDLE_NO_SMALL_NODEADDRESS
    }
    
    inline iterator end() const  
    {
#ifndef ADDLE_NO_SMALL_NODEADDRESS
        if (_smallSize != INVALID_SMALL_SIZE)
            return iterator(_smallData + _smallSize);
        else
            return iterator(largeData().end());
#else // ADDLE_NO_SMALL_NODEADDRESS
        return _data.end();
#endif // ADDLE_NO_SMALL_NODEADDRESS
    }
    
    inline bool isRoot() const 
    { 
#ifndef ADDLE_NO_SMALL_NODEADDRESS
        return !_smallSize; 
#else // ADDLE_NO_SMALL_NODEADDRESS
        return _data.isEmpty();
#endif // ADDLE_NO_SMALL_NODEADDRESS
    }
    
    inline iterator selfIndex() const
    {
        iterator result = end();
        if (!isRoot())
            --result;
        
        return result;
    }
    
    inline iterator parentIndex() const
    {
        iterator result = end();
        if (size() > 1)
            std::advance(result, -2);
        
        return result;
    }
    
    inline iterator commonAncestorIndex(const NodeAddress& other) const
    {
        if (isRoot() || other.isRoot())
            return this->end();
        
        auto i = this->begin();
        auto j = other.begin();
        
        if (*i != *j)
            return this->end();
        
        auto end = this->end();
        auto otherEnd = other.end();
        
        while (true)
        {
            ++i;
            ++j;
            
            if (i == end || j == otherEnd || *i != *j)
                return --i;
        }
    }

    inline bool isAncestorOf(const NodeAddress& other) const
    {
        return (commonAncestorIndex(other)) == this->selfIndex();
    }

    inline bool isDescendantOf(const NodeAddress& other) const
    {
        return other.isAncestorOf(*this);
    }
    
    inline NodeAddress parent() const;

    inline void swap(NodeAddress& other);
    
private:
// #ifndef ADDLE_NO_SMALL_NODEADDRESS
//     inline NodeAddress(const large_data_t& largeData);
//     inline NodeAddress(large_data_t&& largeData);
//     inline NodeAddress(const small_t (&smallData)[SMALL_ADDRESS_CAPACITY], unsigned short smallSize);
// #else // ADDLE_NO_SMALL_NODEADDRESS
//     
//     inline NodeAddress(const list_t& data)
//         : _data(data)
//     {
// #ifndef ADDLE_NO_FAST_NODEADDRESS
//         updateFastAddress();
// #endif // ADDLE_NO_FAST_NODEADDRESS
//     }
//     
//     inline NodeAddress(list_t&& data)
//         : _data(std::move(data))
//     {
// #ifndef ADDLE_NO_FAST_NODEADDRESS
//         updateFastAddress();
// #endif // ADDLE_NO_FAST_NODEADDRESS
//     }
// 
// #endif // ADDLE_NO_SMALL_NODEADDRESS
    
#ifndef ADDLE_NO_SMALL_NODEADDRESS
    const large_data_t& largeData() const
    {
        return *reinterpret_cast<const large_data_t*>(std::addressof(_largeDataStorage));
    }
    
    large_data_t& largeData()
    {
        return *reinterpret_cast<large_data_t*>(std::addressof(_largeDataStorage));
    }
#endif // ADDLE_NO_SMALL_NODEADDRESS
    
#ifndef ADDLE_NO_FAST_NODEADDRESS
    inline void updateFastAddress()
    {
        // First, portion a 64-bit uint into an "array" of 8 x 8-bit uints 
        // composed from the address indices with the first index filling
        // the most significant bits, and the 8th filling the least
        // significant bits. Add 1 to each index before inserting in order
        // to distinguish between a 0 and an empty cell.
        //
        // NOTE: This scheme has imperfect packing. Theoretically the
        // MAX_CELL_VALUE could be raised to UINT8_MAX by taking advantage 
        // of adder overflow. This is not a great concern.
        
        constexpr unsigned CELL_COUNT = 8;
        constexpr unsigned CELL_WIDTH = 8;
        constexpr unsigned MAX_CELL_VALUE = UINT8_MAX - 1;

        const std::size_t size = this->size();
        
        if (size > CELL_COUNT)
        {
            _fastAddress = INVALID_FAST_ADDRESS;
            return;
        }
        
        _fastAddress = 0;
        
        const auto calc = [&](auto&& range) -> void {
                for (std::size_t v : range)
                {
                    if (Q_UNLIKELY(v > MAX_CELL_VALUE))
                    {
                        _fastAddress = INVALID_FAST_ADDRESS;
                        return;
                    }
                    
                    _fastAddress = _fastAddress << CELL_WIDTH | v + 1;
                }
            };
            
#ifndef ADDLE_NO_SMALL_NODEADDRESS
        if (_smallSize != INVALID_SMALL_SIZE)
            calc(
                    boost::iterator_range<const small_t*>(
                        _smallData, 
                        _smallData + _smallSize
                    )
                );
        else
            calc(largeData());
#else // ADDLE_NO_SMALL_NODEADDRESS
        calc(_data);
#endif // ADDLE_NO_SMALL_NODEADDRESS
        if (_fastAddress == INVALID_FAST_ADDRESS)
            return;
        
        _fastAddress 
            <<= std::max(CELL_COUNT - size, (std::size_t) 0) * CELL_WIDTH;
    }
#endif // ADDLE_NO_FAST_NODEADDRESS

#ifndef ADDLE_NO_SMALL_NODEADDRESS
    unsigned short                  _smallSize = 0;
    
    union
    {
        small_data_t                _smallData;
        std::aligned_storage_t<
            sizeof(large_data_t), 
            alignof(large_data_t)>  _largeDataStorage;
    };
#else // ADDLE_NO_SMALL_NODEADDRESS
    list_t                          _data;
#endif // ADDLE_NO_SMALL_NODEADDRESS
    
#ifndef ADDLE_NO_FAST_NODEADDRESS
    // While the total address space of a data tree is arbitrarily large and 
    // requires dynamic memory and iteration to represent fully, we can 
    // encode a reasonable portion of the address space (covering a majority 
    // of typical cases) into a 64-bit integer such that arithmetic
    // comparison of _fastAddress is equivalent to lexicographical 
    // comparison of _address (provided that both _fastAddress are valid)
    //
    // That "reasonable portion" is currently tuned to approximately max
    // index of 255 and max depth of 8. This can be adjusted if desired.
    fast_address_t _fastAddress = 0;
#endif // ADDLE_NO_FAST_NODEADDRESS
    
    std::size_t hash() const
    {
#ifndef ADDLE_NO_FAST_NODEADDRESS
        if (Q_LIKELY(_fastAddress != INVALID_FAST_ADDRESS))
        {
            return std::hash<fast_address_t>()(_fastAddress);
        }
        else
#endif // ADDLE_NO_FAST_NODEADDRESS
        {
#ifndef ADDLE_NO_SMALL_NODEADDRESS
            if (_smallSize != INVALID_SMALL_SIZE)
            {
                return boost::hash_range(
                        _smallData,
                        _smallData + _smallSize
                    );
            }
            else
            {
                return boost::hash_range(
                        largeData().begin(),
                        largeData().end()
                    );
            }
#else // ADDLE_NO_SMALL_NODEADDRESS
        return boost::hash_range(
                _data.begin(),
                _data.end()
            );
#endif 
        }
    }
    
    friend uint qHash(const NodeAddress& address, uint seed = 0)
    {
        return static_cast<uint>(address.hash()) ^ seed;
    }

#if defined(ADDLE_DEBUG) || defined(ADDLE_TEST)
    QByteArray repr() const
    {
        auto i      = this->begin();
        auto end    = this->end();
            
        if (i == end)
        {
            return QByteArrayLiteral("{}");
        }
        else
        {
            QByteArray result = QByteArrayLiteral("{ ");
            
            while (true)
            {
                result += QByteArray::number(qulonglong(*i));
                
                ++i;
                if (i == end) break;
                
                result += QByteArrayLiteral(", ");
            }

            result += QByteArrayLiteral(" }");
            return result;
        }
    }
    
    friend QDebug operator<<(QDebug debug, const NodeAddress& nodeAddress)
    {
        return debug << nodeAddress.repr().constData();
    }
#endif // defined(ADDLE_DEBUG) || defined(ADDLE_TEST)

    friend void swap(NodeAddress& a1, NodeAddress& a2)
    {
        a1.swap(a2);
    }
    
#ifdef ADDLE_TEST
    friend char* ::QTest::toString<NodeAddress>(const NodeAddress&);
    friend class ::DataTree_UTest;
#endif
    
    friend class NodeAddressBuilder;
    friend struct NodeChunk;
    friend struct std::hash<NodeAddress>;
};

#ifndef ADDLE_NO_SMALL_NODEADDRESS
NodeAddress::~NodeAddress()
{
    if (Q_UNLIKELY(_smallSize == INVALID_SMALL_SIZE))
        largeData().~large_data_t();
}

inline NodeAddress::NodeAddress(const NodeAddress& other)
    : _smallSize(other._smallSize)
#ifndef ADDLE_NO_FAST_NODEADDRESS
    , _fastAddress(other._fastAddress)
#endif // ADDLE_NO_FAST_NODEADDRESS
{
    if (Q_UNLIKELY(_smallSize == INVALID_SMALL_SIZE))
    {
        new (&_largeDataStorage) large_data_t(other.largeData());
    }
    else
    {
        std::memcpy(
                _smallData,
                other._smallData,
                _smallSize * sizeof(small_t)
            );
    }
}

inline NodeAddress::NodeAddress(NodeAddress&& other)
    : _smallSize(other._smallSize)
#ifndef ADDLE_NO_FAST_NODEADDRESS
    , _fastAddress(other._fastAddress)
#endif // ADDLE_NO_FAST_NODEADDRESS
{
    if (Q_UNLIKELY(_smallSize == INVALID_SMALL_SIZE))
    {
        new (&_largeDataStorage) large_data_t(std::move(other.largeData()));
        other.largeData().~large_data_t();
        other._smallSize = 0;
    }
    else
    {
        std::memcpy(
                _smallData,
                other._smallData,
                _smallSize * sizeof(small_t)
            );
    }
}

inline NodeAddress& NodeAddress::operator=(const NodeAddress& other)
{
    if (Q_UNLIKELY(_smallSize == INVALID_SMALL_SIZE))
        largeData().~large_data_t();
    
    _smallSize = other._smallSize;
    
    if (Q_UNLIKELY(_smallSize == INVALID_SMALL_SIZE))
    {
        new (&_largeDataStorage) large_data_t(other.largeData());
    }
    else
    {
        std::memcpy(
                _smallData,
                other._smallData,
                _smallSize * sizeof(small_t)
            );
    }
    
#ifndef ADDLE_NO_FAST_NODEADDRESS
    _fastAddress = other._fastAddress;
#endif // ADDLE_NO_FAST_NODEADDRESS
    return *this;
}

NodeAddress& NodeAddress::operator=(NodeAddress&& other)
{
    if (Q_UNLIKELY(_smallSize == INVALID_SMALL_SIZE))
        largeData().~large_data_t();
    
    _smallSize = other._smallSize;
    
    if (Q_UNLIKELY(_smallSize == INVALID_SMALL_SIZE))
    {
        new (&_largeDataStorage) large_data_t(std::move(other.largeData()));
        other.largeData().~large_data_t();
        other._smallSize = 0;
    }
    else
    {
        std::memcpy(
                _smallData,
                other._smallData,
                _smallSize * sizeof(small_t)
            );
    }
    
#ifndef ADDLE_NO_FAST_NODEADDRESS
    _fastAddress = other._fastAddress;
#endif // ADDLE_NO_FAST_NODEADDRESS
    return *this;
}
#endif // ADDLE_NO_SMALL_NODEADDRESS

inline void NodeAddress::swap(NodeAddress& other)
{
#ifndef ADDLE_NO_SMALL_NODEADDRESS
    if (_smallSize != INVALID_SMALL_SIZE 
            && other._smallSize != INVALID_SMALL_SIZE)
    {
        std::swap(_smallData, other._smallData);
    }
    else if (_smallSize == INVALID_SMALL_SIZE 
            && other._smallSize != INVALID_SMALL_SIZE)
    {
        large_data_t temp;
        temp.swap(largeData());
        largeData().~large_data_t();
        
        std::memcpy(
                _smallData,
                other._smallData,
                other._smallSize * sizeof(small_t)
            );
        
        new (&other._largeDataStorage) large_data_t();
        other.largeData().swap(temp);
    }
    else if (_smallSize != INVALID_SMALL_SIZE
            && other._smallSize == INVALID_SMALL_SIZE)
    {
        large_data_t temp;
        temp.swap(other.largeData());
        other.largeData().~large_data_t();
        
        std::memcpy(
                other._smallData,
                _smallData,
                _smallSize * sizeof(small_t)
            );
        
        new (&_largeDataStorage) large_data_t();
        largeData().swap(temp);
    }
    else
    {
        largeData().swap(other.largeData());
    }
    std::swap(_smallSize, other._smallSize);
#else // ADDLE_NO_SMALL_NODEADDRESS
    std::swap(_data, other._data);
#endif // ADDLE_NO_SMALL_NODEADDRESS
    
#ifndef ADDLE_NO_FAST_NODEADDRESS
    std::swap(_fastAddress, other._fastAddress);
#endif // ADDLE_NO_FAST_NODEADDRESS
}

class NodeAddressBuilder
{
#ifndef ADDLE_NO_SMALL_NODEADDRESS
    using large_t       = NodeAddress::large_t;
    using large_data_t  = NodeAddress::large_data_t;
    
    using small_t       = NodeAddress::small_t;
    using small_data_t  = NodeAddress::small_data_t;
    
    static constexpr std::size_t    SMALL_ADDRESS_CAPACITY  = NodeAddress::SMALL_ADDRESS_CAPACITY;
    static constexpr std::size_t    SMALL_ADDRESS_MAX_VALUE = NodeAddress::SMALL_ADDRESS_MAX_VALUE;
    static constexpr unsigned short INVALID_SMALL_SIZE      = NodeAddress::INVALID_SMALL_SIZE;
#else
    using list_t        = NodeAddress::list_t;
#endif // ADDLE_NO_SMALL_NODEADDRESS    
public:
    NodeAddressBuilder() = default;
    NodeAddressBuilder(const NodeAddressBuilder&) = delete;
    NodeAddressBuilder(NodeAddressBuilder&&) = delete;
    
    NodeAddressBuilder& operator=(const NodeAddressBuilder&) = delete;
    NodeAddressBuilder& operator=(NodeAddressBuilder&&) = delete;
    
    NodeAddressBuilder(const std::initializer_list<std::size_t>& indices)
    {
        initFromRange(
                std::forward<const std::initializer_list<std::size_t>&>(indices)
            );
    }
    
    template<
        typename Range,
        std::enable_if_t<
            std::is_convertible_v<
                typename boost::range_reference<boost::remove_cv_ref_t<Range>>::type,
                std::size_t
            >,
        void*> = nullptr>
    NodeAddressBuilder(Range&& indices)
    {
        initFromRange(std::forward<Range>(indices));
    }
    
    explicit NodeAddressBuilder(const NodeAddress& address)
        : _address(address)
    {
    }
    
    explicit NodeAddressBuilder(NodeAddress&& address)
        : _address(std::move(address))
    {
    }
    
    void append(std::size_t index)
    {
#ifndef ADDLE_NO_SMALL_NODEADDRESS
        if (index <= SMALL_ADDRESS_MAX_VALUE 
            && _address._smallSize < SMALL_ADDRESS_CAPACITY)
        {
            _address._smallData[_address._smallSize] 
                = static_cast<small_t>(index);
            ++_address._smallSize;
        }
        else
        {
            if (_address._smallSize != INVALID_SMALL_SIZE)
            {
                large_data_t temp;
                temp.reserve(_address._smallSize + 1);
                
                auto i      = _address._smallData;
                auto end    = _address._smallData + _address._smallSize;
                for(; i != end; ++i)
                {
                    temp.append(*i);
                }
                
                _address._smallSize = INVALID_SMALL_SIZE;
                    
                new (&_address._largeDataStorage) large_data_t(std::move(temp));
            }
            
            _address.largeData().append(index);
        }
#else // ADDLE_NO_SMALL_NODEADDRESS
        _address._data.append(index);
#endif // ADDLE_NO_SMALL_NODEADDRESS
        
#ifndef ADDLE_NO_FAST_NODEADDRESS
        _address.updateFastAddress();
#endif
    }
    
    NodeAddressBuilder& operator<<(std::size_t index)
    {
        append(index);
        return *this;
    }
    
    std::size_t size() const
    {
        return _address.size();
    }
    
    void reserve(std::size_t size)
    {
#ifndef ADDLE_NO_SMALL_NODEADDRESS
        if (size <= SMALL_ADDRESS_CAPACITY) return;
        
        if (_address._smallSize != INVALID_SMALL_SIZE)
        {
            large_data_t temp;
            temp.reserve(size);
            
            auto i      = _address._smallData;
            auto end    = _address._smallData + _address._smallSize;
            for(; i != end; ++i)
            {
                temp.append(*i);
            }
            
            _address._smallSize = INVALID_SMALL_SIZE;
            new (&_address._largeDataStorage) large_data_t(std::move(temp));
        }
        else
        {
            _address.largeData().reserve(size);
        }
#else
        _address._data.reserve(size);
#endif
    }
    
    NodeAddress address() const &
    {
        return _address;
    }
    
    NodeAddress address() &&
    {
        return std::move(_address);
    }
    
    operator NodeAddress () const & { return address(); }
    operator NodeAddress () && { return address(); }
    
private:
#ifndef ADDLE_NO_SMALL_NODEADDRESS
    template<
        typename Range,
        std::enable_if_t<
            std::is_convertible_v<
                typename boost::range_category<boost::remove_cv_ref_t<Range>>::type, 
                std::forward_iterator_tag
            >,
        void*> = nullptr>
    void initFromRange(Range&& indices)
    {
        auto size   = boost::size(indices);
        auto begin  = std::begin(indices);
        auto end    = std::end(indices);
        
        if (
            size <= SMALL_ADDRESS_CAPACITY
            && std::all_of(
                begin,
                end,
                [] (std::size_t i) { return i <= SMALL_ADDRESS_MAX_VALUE; }
            )
        )
        {
            _address._smallSize = size;
                
            auto i = begin;
            auto j = _address._smallData;
            
            for (; i != end; ++i)
            {
                (*j) = static_cast<small_t>(*i);
                ++j;
            }
        }
        else
        {
            _address._smallSize = INVALID_SMALL_SIZE;
            
            if constexpr (std::is_same_v<boost::remove_cv_ref_t<Range>, large_data_t>)
            {
                new (&_address._largeDataStorage) large_data_t(
                        std::forward<Range>(indices)
                    );
            }
            else
            {
                new (&_address._largeDataStorage) large_data_t(begin, end);
            }
        }
        
#ifndef ADDLE_NO_FAST_NODEADDRESS
        _address.updateFastAddress();
#endif
    }
    
    template<
        typename Range,
        std::enable_if_t<
            !std::is_convertible_v<
                typename boost::range_category<boost::remove_cv_ref_t<Range>>::type, 
                std::forward_iterator_tag
            >,
        void*> = nullptr>
    void initFromRange(Range&& indices)
    {
        large_data_t temp(std::begin(indices), std::end(indices));
        initFromRange(std::move(temp));
    }
#else // ADDLE_NO_SMALL_NODEADDRESS
    template<typename Range>
    void initFromRange(Range&& indices)
    {
        _address._data = list_t(std::begin(indices), std::end(indices));
        
#ifndef ADDLE_NO_FAST_NODEADDRESS
        _address.updateFastAddress();
#endif // ADDLE_NO_FAST_NODEADDRESS
    }
    
    void initFromRange(const list_t& indices)
    {
        _address._data = indices;
        
#ifndef ADDLE_NO_FAST_NODEADDRESS
        _address.updateFastAddress();
#endif // ADDLE_NO_FAST_NODEADDRESS
    }
    
    void initFromRange(list_t&& indices)
    {
        _address._data = std::move(indices);
        
#ifndef ADDLE_NO_FAST_NODEADDRESS
        _address.updateFastAddress();
#endif // ADDLE_NO_FAST_NODEADDRESS 
    }
#endif // ADDLE_NO_SMALL_NODEADDRESS
    
    NodeAddress _address;
};

inline NodeAddress::NodeAddress(const std::initializer_list<std::size_t>& init)
    : NodeAddress(NodeAddressBuilder(init))
{
}

inline NodeAddress NodeAddress::parent() const
{
    if (isRoot()) 
        return NodeAddress();
    
    return NodeAddressBuilder(
            boost::iterator_range<NodeAddress::iterator>(begin(), --end())
        );
}
    
/**
 * A continuous run of (sibling) node addresses
 */
struct NodeChunk
{
    NodeAddress address;
    std::size_t length;
    
    inline bool coversAddress(const NodeAddress& otherAddress) const
    {
        if (address.isRoot())
            return true;
        
        if (address.size() > otherAddress.size())
            return false;
   
        auto ancestorIndex = address.commonAncestorIndex(otherAddress);
        
        if (ancestorIndex == address.selfIndex())
            return true;
        if (ancestorIndex != address.parentIndex())
            return false;

        auto divergingIndex = otherAddress.begin() 
            + std::distance(address.begin(), ancestorIndex) 
            + 1;
        
        return (*divergingIndex) >= address.lastIndex() 
            && (*divergingIndex) < address.lastIndex() + length;
    }
    
    bool operator==(const NodeChunk& other) const
    {
        return address == other.address && length == other.length;
    }
    
    bool operator!=(const NodeChunk& other) const
    {
        return address != other.address || length != other.length;
    }
    
    friend bool operator<(const NodeChunk& lhs, const NodeChunk& rhs)
    {
        return lhs.address < rhs.address; 
    }
    
    friend bool operator<(const NodeAddress& lhs, const NodeChunk& rhs)
    {
        return lhs < rhs.address; 
    }
    
    friend bool operator<(const NodeChunk& lhs, const NodeAddress& rhs)
    {
        return lhs.address < rhs; 
    }
    
#ifdef ADDLE_DEBUG
    friend QDebug operator<<(QDebug debug, const NodeChunk& chunk)
    {
        return debug 
            << "address:"   << chunk.address
            << "length:"    << chunk.length;
    }
#endif
};

} // namespace aux_datatree

using DataTreeNodeAddress           = aux_datatree::NodeAddress;
using DataTreeNodeAddressBuilder    = aux_datatree::NodeAddressBuilder;
using DataTreeNodeChunk             = aux_datatree::NodeChunk;

} // namespace Addle

namespace std 
{
    template<>
    struct hash<::Addle::aux_datatree::NodeAddress>
    {
        std::size_t operator()(const ::Addle::aux_datatree::NodeAddress& address) const
        {
            return address.hash();
        }
    };
}

#ifdef ADDLE_TEST
template<>
inline char* ::QTest::toString(
        const ::Addle::aux_datatree::NodeAddress& nodeAddress
    )
{
    return qstrdup(nodeAddress.repr().constData());
}
#endif

Q_DECLARE_METATYPE(Addle::DataTreeNodeAddress);
Q_DECLARE_METATYPE(Addle::DataTreeNodeChunk);

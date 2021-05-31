#pragma once

#include <cstdint>
#include <variant>

#include <boost/mp11.hpp>
#include <boost/iterator.hpp>

#include <QList>

#ifdef ADDLE_DEBUG
#include <QtDebug>      // QDebug operator<< for NodeAddress
#endif

#ifdef ADDLE_TEST
#include <QTest>        // QTest::toString for NodeAddress
class DataTree_UTest;   // friend access for NodeAddress
#endif

#include "utilities/collections.hpp"
#include "utilities/metaprogramming.hpp"

namespace Addle {
namespace aux_datatree {

// Generically represents the location of a node in a tree as a sequence of
// 0-based integer indices. For typical values, no dynamic memory is allocated 
// and comparisions/hashes are very fast.
//
// TODO: check and see if _fastAddress actually gains us anything over
// automatic vectorization.
class NodeAddress
{
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
    
    using fast_address_t = std::uint_fast64_t;
    
    static constexpr std::uint64_t  INVALID_FAST_ADDRESS = UINT_FAST64_MAX;
public:        
    using value_type        = std::size_t;
    using const_reference   = const std::size_t&;
    using reference         = const_reference;
    using difference_type   = std::ptrdiff_t;
    using size_type         = std::size_t;
    
    class iterator : public boost::iterator_facade<
            iterator,
            std::size_t,
            std::random_access_iterator_tag,
            std::size_t
        >
    {
    public:
        iterator() = default;
        iterator(const iterator&) = default;
        
    private:
        iterator(const small_t* base)
            : _base(base)
        {
        }
        
        iterator(large_data_t::const_iterator base)
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
        
        bool equal(const iterator other) const
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
                // invalid comparison
                assert(false);
            }
        };
        
        std::ptrdiff_t distance_to(const iterator& other) const
        {
            return std::visit(visitor_distance {}, _base, other._base);
        }
        
        std::variant<const small_t*, large_data_t::const_iterator> _base;
        
        friend class boost::iterator_core_access;
        friend class NodeAddress;
    };
    
    using const_iterator = iterator;
    
    NodeAddress() = default;
    
    inline ~NodeAddress();
    inline NodeAddress(const NodeAddress&);
    inline NodeAddress(NodeAddress&&);
    
    inline NodeAddress(const std::initializer_list<std::size_t>& init);
    
    inline NodeAddress& operator=(const NodeAddress&);
    inline NodeAddress& operator=(NodeAddress&&);
    
    
    inline bool operator==(const NodeAddress& other) const
    {
        if (Q_LIKELY(
            _fastAddress != INVALID_FAST_ADDRESS 
            && other._fastAddress != INVALID_FAST_ADDRESS
        ))
        {
            return _fastAddress == other._fastAddress;
        }
        else
        {
            if (_smallSize != other._smallSize)
                return false;
            
            if (_smallSize != INVALID_SMALL_SIZE)
                return std::equal(
                        _smallData, 
                        _smallData + _smallSize, 
                        other._smallData);
            else
                return largeData() == other.largeData();
        }
    }
    
    inline bool operator!=(const NodeAddress& other) const
    {
        return !(*this == other);
    }
    
    inline bool operator<(const NodeAddress& other) const
    {
        if (Q_LIKELY(
            _fastAddress != INVALID_FAST_ADDRESS 
            && other._fastAddress != INVALID_FAST_ADDRESS
        ))
        {
            return _fastAddress < other._fastAddress;
        }
        else
        {            
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
        }
    }
    
    inline bool operator<=(const NodeAddress& other) const
    {
        return !(*this > other);
    }
    
    inline bool operator>(const NodeAddress& other) const
    {
        if (Q_LIKELY(
            _fastAddress != INVALID_FAST_ADDRESS 
            && other._fastAddress != INVALID_FAST_ADDRESS
        ))
        {
            return _fastAddress > other._fastAddress;
        }
        else
        {
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
        }
    }
    
    inline bool operator>=(const NodeAddress& other) const
    {
        return !(*this < other);
    }
    
    inline std::size_t operator[](std::size_t depth) const
    { 
        if (_smallSize != INVALID_SMALL_SIZE)
        {
            assert(depth < _smallSize);
            return _smallData[depth];
        }
        else
        {
            return largeData()[depth];
        }
    }
    
    inline std::size_t lastIndex() const 
    { 
        if (isRoot()) 
            return 0;
        else if (_smallSize != INVALID_SMALL_SIZE)
            return _smallData[_smallSize - 1];
        else
            return largeData().last();
    }
    
    inline std::size_t size() const     
    { 
        return (_smallSize != INVALID_SMALL_SIZE) ? _smallSize : largeData().size(); 
    }
    
    inline iterator begin() const
    { 
        if (_smallSize != INVALID_SMALL_SIZE)
            return iterator(_smallData);
        else
            return iterator(largeData().begin());
    }
    
    inline iterator end() const  
    {
        if (_smallSize != INVALID_SMALL_SIZE)
            return iterator(_smallData + _smallSize);
        else
            return iterator(largeData().end());
    }
    
    inline bool isRoot() const { return !_smallSize; }
    
    inline iterator lastCommonAncestor(const NodeAddress& other) const
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
        if (isRoot() && !other.isRoot())
            return true;
        if (other.isRoot())
            return false;
        
        if (other.size() <= size())
            return false;
        
        return (lastCommonAncestor(other)) == --(this->end());
    }

    inline bool isDescendantOf(const NodeAddress& other) const
    {
        return other.isAncestorOf(*this);
    }
    
    inline NodeAddress parent() const;
    
    inline void swap(NodeAddress& other);
    
private:
    inline NodeAddress(const large_data_t& largeData);
    inline NodeAddress(large_data_t&& largeData);
    inline NodeAddress(const small_t (&smallData)[SMALL_ADDRESS_CAPACITY], unsigned short smallSize);
    
    const large_data_t& largeData() const
    {
        return *reinterpret_cast<const large_data_t*>(std::addressof(_largeDataStorage));
    }
    
    large_data_t& largeData()
    {
        return *reinterpret_cast<large_data_t*>(std::addressof(_largeDataStorage));
    }
    
    inline void setFastAddress()
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
        
        if (_smallSize != INVALID_SMALL_SIZE)
        {
            for (std::size_t i = 0; i < _smallSize; ++i)
            {
                std::size_t v = _smallData[i];
                if (Q_UNLIKELY(v > MAX_CELL_VALUE))
                {
                    _fastAddress = INVALID_FAST_ADDRESS;
                    return;
                }
                
                _fastAddress = _fastAddress << CELL_WIDTH | v + 1;
            }
        }
        else
        {
            for (std::size_t v : largeData())
            {
                if (v > MAX_CELL_VALUE)
                {
                    _fastAddress = INVALID_FAST_ADDRESS;
                    return;
                }
                
                _fastAddress = _fastAddress << CELL_WIDTH | v + 1;
            }
        }
        
        _fastAddress 
            <<= std::max(CELL_COUNT - size, (std::size_t) 0) * CELL_WIDTH;
    }
        
    unsigned short _smallSize = 0;
    
    union
    {
        small_data_t                _smallData;
        std::aligned_storage_t<
            sizeof(large_data_t), 
            alignof(large_data_t)>  _largeDataStorage;
    };
    
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
    
    friend uint qHash(const NodeAddress& address, uint seed = 0)
    {
        if (Q_LIKELY(address._fastAddress != INVALID_FAST_ADDRESS))
        {
            return ::qHash(address._fastAddress, seed);
        }
        else
        {
            if (address._smallSize != INVALID_SMALL_SIZE)
                return ::qHashRange(
                        address._smallData,
                        address._smallData + address._smallSize,
                        seed
                    );
            else
                return ::qHashRange(
                        address.largeData().begin(),
                        address.largeData().end(),
                        seed
                    );
        }
    }
    
    friend void swap(NodeAddress& a1, NodeAddress& a2)
    {
        a1.swap(a2);
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
#endif
    
#ifdef ADDLE_DEBUG
    friend QDebug operator<<(QDebug debug, const NodeAddress& nodeAddress)
    {
        return debug << nodeAddress.repr().constData();
    }
#endif

#ifdef ADDLE_TEST
    friend char* ::QTest::toString<NodeAddress>(const NodeAddress&);
    friend class ::DataTree_UTest;
#endif
    
    friend class NodeAddressBuilder;
    friend struct NodeChunk;
};

NodeAddress::~NodeAddress()
{
    if (Q_UNLIKELY(_smallSize == INVALID_SMALL_SIZE))
        largeData().~large_data_t();
}

inline NodeAddress::NodeAddress(const NodeAddress& other)
    : _smallSize(other._smallSize)
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
    
    setFastAddress();
}

inline NodeAddress::NodeAddress(NodeAddress&& other)
    : _smallSize(other._smallSize)
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
    
    setFastAddress();
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
    
    setFastAddress();
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
    
    setFastAddress();
    return *this;
}
    
inline NodeAddress::NodeAddress(const large_data_t& largeData)
    : _smallSize(INVALID_SMALL_SIZE)
{
    new (&_largeDataStorage) large_data_t(largeData);
    setFastAddress();
}

inline NodeAddress::NodeAddress(large_data_t&& largeData)
    : _smallSize(INVALID_SMALL_SIZE)
{
    new (&_largeDataStorage) large_data_t(std::move(largeData));
    setFastAddress();
}

inline NodeAddress::NodeAddress(
        const small_t (&smallData)[SMALL_ADDRESS_CAPACITY], 
        unsigned short smallSize
    )
    : _smallSize(smallSize)
{
    assert (smallSize <= SMALL_ADDRESS_CAPACITY);
    std::memcpy(
            _smallData,
            smallData,
            _smallSize * sizeof(small_t)
        );
    setFastAddress();
}


inline void NodeAddress::swap(NodeAddress& other)
{
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
    std::swap(_fastAddress, other._fastAddress);
}
    
class NodeAddressBuilder
{
    using large_t       = NodeAddress::large_t;
    using large_data_t  = NodeAddress::large_data_t;
    
    using small_t       = NodeAddress::small_t;
    using small_data_t = NodeAddress::small_data_t;
    
    static constexpr std::size_t    SMALL_ADDRESS_CAPACITY  = NodeAddress::SMALL_ADDRESS_CAPACITY;
    static constexpr std::size_t    SMALL_ADDRESS_MAX_VALUE = NodeAddress::SMALL_ADDRESS_MAX_VALUE;
    static constexpr unsigned short INVALID_SMALL_SIZE      = NodeAddress::INVALID_SMALL_SIZE;
    
public:
    NodeAddressBuilder() = default;
    NodeAddressBuilder(const NodeAddressBuilder&) = delete;
    NodeAddressBuilder(NodeAddressBuilder&&) = delete;
    
    NodeAddressBuilder& operator=(const NodeAddressBuilder&) = delete;
    NodeAddressBuilder& operator=(NodeAddressBuilder&&) = delete;
    
    NodeAddressBuilder(const std::initializer_list<std::size_t>& indices)
    {
        initFromRange(std::forward<const std::initializer_list<std::size_t>&>(indices));
    }
    
    template<
        typename Range,
        std::enable_if_t<
            std::is_convertible_v<
                typename boost::range_reference<boost::remove_cv_ref_t<Range>>::type,
                std::size_t
            >
            && std::is_convertible_v<
                typename boost::range_category<boost::remove_cv_ref_t<Range>>::type, 
                std::forward_iterator_tag
            >,
        void*> = nullptr>
    NodeAddressBuilder(Range&& indices)
    {
        initFromRange(std::forward<Range>(indices));
    }
    
    template<
        typename Range,
        std::enable_if_t<
            std::is_convertible_v<
                typename boost::range_reference<boost::remove_cv_ref_t<Range>>::type,
                std::size_t
            >
            && !std::is_convertible_v<
                typename boost::range_category<boost::remove_cv_ref_t<Range>>::type, 
                std::forward_iterator_tag
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
    }
    
    NodeAddressBuilder& operator<<(std::size_t index)
    {
        append(index);
        return *this;
    }
    
    std::size_t size() const
    {
        return (_address._smallSize != INVALID_SMALL_SIZE) ?
            _address._smallSize : 
            _address.largeData().size();
    }
    
    void reserve(std::size_t size)
    {
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
        auto begin  = boost::begin(indices);
        auto end    = boost::end(indices);
        
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
        large_data_t temp(boost::begin(indices), boost::end(indices));
        initFromRange(std::move(temp));
    }
    
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
   
        auto ancestor = address.lastCommonAncestor(otherAddress);
        auto ancestorDepth = std::distance(address.begin(), ancestor);
        
        if (ancestorDepth == address.size())
            return true;
        else if (ancestorDepth != address.size() - 1)
            return false;
        
        auto otherAtDepth = otherAddress.begin() + ancestorDepth + 1;
        
        return (*otherAtDepth) >= address.lastIndex() 
            && (*otherAtDepth) < address.lastIndex() + length;
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

using DataTreeNodeAddress   = aux_datatree::NodeAddress;
using DataTreeNodeChunk     = aux_datatree::NodeChunk;

} // namespace Addle

#ifdef ADDLE_TEST
template<>
inline char* ::QTest::toString(
        const ::Addle::aux_datatree::NodeAddress& nodeAddress
    )
{
    return qstrdup(nodeAddress.repr().constData());
}
#endif


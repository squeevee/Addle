#pragma once

#include <cstdint>
#include <optional>
#include <bitset>

#include <boost/mp11.hpp>
#include <boost/iterator.hpp>
#include <boost/container_hash/hash.hpp>

#include <QList>
#include <QAtomicInteger>

#if defined(ADDLE_DEBUG) || defined(ADDLE_TEST)
#include <QtDebug>      // QDebug operator<< for NodeAddress and NodeChunk
#include "utilities/debugging/qdebug_extensions.hpp"
#include "utilities/debugging/debugstring.hpp"
#endif

#ifdef ADDLE_TEST
#include <QTest>        // QTest::toString for NodeAddress and NodeChunk
class DataTree_UTest;   // friend access for NodeAddress
#endif


#include "utilities/ranges.hpp"
#include "utilities/metaprogramming.hpp"

namespace Addle {
namespace aux_datatree {

// Generically represents the location of a node in a tree as a sequence of
// 0-based integer indices.
    
// TODO: It's expected that typcial node addresses will have fewer than 8
// indices and no index values greater than 255, meaning that a plurality of
// practical NodeAddresses could fit within the space of a 64-bit pointer and 
// would not need to allocate dynamic memory. I'm convinced that this can be 
// done while keeping `sizeof(NodeAddress) == sizeof(void*)` but it would
// involve a lot of bit-level futzing and would not really represent that much
// of a performance gain in the grand scheme.
    
class NodeAddress
{
    using index_t   = std::size_t;
    using list_t    = QList<index_t>;
    
public:        
    using value_type        = std::size_t;
    using const_reference   = const std::size_t&;
    using reference         = const_reference;
    using difference_type   = std::ptrdiff_t;
    using size_type         = std::size_t;
    
    using iterator          = list_t::const_iterator;
    using const_iterator    = list_t::const_iterator;
    
    inline NodeAddress() = default;
    
    inline NodeAddress(const NodeAddress&) = default;
    inline NodeAddress(NodeAddress&&) = default;
    
    inline NodeAddress& operator=(const NodeAddress&) = default;
    inline NodeAddress& operator=(NodeAddress&&) = default;
    
    inline NodeAddress(const std::initializer_list<std::size_t>& init);
    // defined below NodeAddressBuilder
    
    inline bool operator==(const NodeAddress& other) const
    {
        return _list == other._list;
    }
    
    inline bool operator!=(const NodeAddress& other) const
    {
        return !(*this == other);
    }
    
    inline bool operator<(const NodeAddress& other) const
    {
        return std::lexicographical_compare(
                _list.cbegin(), _list.cend(),
                other._list.cbegin(), other._list.cend()
            );
    }
    
    inline bool operator<=(const NodeAddress& other) const
    {
        return !(*this > other);
    }
    
    inline bool operator>(const NodeAddress& other) const
    {
        return std::lexicographical_compare(
                other._list.cbegin(), other._list.cend(),
                _list.cbegin(), _list.cend()
            );
    }
    
    inline bool operator>=(const NodeAddress& other) const
    {
        return !(*this < other);
    }
    
    inline std::size_t operator[](std::size_t depth) const
    { 
        assert(depth <= _list.size());
        return noDetach(_list)[depth];
    }
    
    inline std::size_t lastIndex() const 
    {
        assert(!_list.isEmpty());
        return noDetach(_list).last();
    }
    
    inline std::size_t size() const     
    { 
        return _list.size();
    }
    
    inline iterator begin() const
    { 
        return _list.cbegin();
    }
    
    inline iterator end() const  
    {
        return _list.cend();
    }
    
    inline bool isRoot() const 
    { 
        return _list.isEmpty();
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
        return commonAncestorIndex(other) == selfIndex() 
            && size() != other.size();
    }

    inline bool isDescendantOf(const NodeAddress& other) const
    {
        return other.isAncestorOf(*this);
    }
    
    inline bool isAncestorOrSame(const NodeAddress& other) const
    {
        return commonAncestorIndex(other) == selfIndex();
    }
    
    inline bool isDescendantOrSame(const NodeAddress& other) const
    {
        return other.isAncestorOrSame(*this);
    }
        
    inline NodeAddress parent() const;
    // defined below NodeAddressBuilder

    inline void swap(NodeAddress& other)
    {
        _list.swap(other._list);
    }
    
    inline NodeAddress operator<<(std::size_t index) const &;
    inline NodeAddress&& operator<<(std::size_t index) &&;
    
    inline NodeAddress operator+(NodeAddress) const &;
    inline NodeAddress&& operator+(NodeAddress) &&;
    // defined below NodeAddressBuilder
    
    inline std::optional<NodeAddress> lowerTruncate(NodeAddress) const;
    
private:
    std::size_t hash_p() const
    {
        return boost::hash_range(
                _list.cbegin(),
                _list.cend()
            );
    }
    
    list_t _list;
    
    friend void swap(NodeAddress& a1, NodeAddress& a2)
    {
        a1.swap(a2);
    }
    
    friend uint qHash(const NodeAddress& address, uint seed = 0)
    {
        return static_cast<uint>(address.hash_p()) ^ seed;
    }

#if defined(ADDLE_DEBUG) || defined(ADDLE_TEST)
    friend QDebug operator<<(QDebug debug, const NodeAddress& nodeAddress)
    {          
        auto i      = nodeAddress.begin();
        auto end    = nodeAddress.end();
        
        if (debug.verbosity() > QDebug::DefaultVerbosity)
            debug << "DataTreeNodeAddress(";
        
        if (i == end)
        {
            debug << "{}";
        }
        else
        {
            debug << "{";
            while (true)
            {
                std::size_t index = *i;
                ++i;
                if (i != end)
                {
                    const QDebugStateSaver s(debug);
                    debug.nospace();
                    debug << index << ",";
                }
                else
                {
                    debug << index;
                    break;
                }
            }
            debug << "}";
        }
        
        if (debug.verbosity() > QDebug::DefaultVerbosity)
            debug << ")";
        
        return debug;
    }
#endif // defined(ADDLE_DEBUG) || defined(ADDLE_TEST)

#ifdef ADDLE_TEST
    friend class ::DataTree_UTest;
#endif
    
    friend class NodeAddressBuilder;
    friend struct NodeChunk;
    friend struct std::hash<NodeAddress>;
};

class NodeAddressBuilder
{
    using list_t        = NodeAddress::list_t;
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
    
    NodeAddressBuilder(NodeAddress::const_iterator begin, NodeAddress::const_iterator end)
    {
        initFromRange(boost::iterator_range<NodeAddress::const_iterator>(begin, end));
    }
    
    explicit NodeAddressBuilder(const NodeAddress& address)
        : _address(address)
    {
    }
    
    explicit NodeAddressBuilder(NodeAddress&& address)
        : _address(std::move(address))
    {
    }
    
    NodeAddressBuilder& append(std::size_t index)
    {
        _address._list.append(index);
        return *this;
    }
    
    NodeAddressBuilder& operator<<(std::size_t index)
    {
        return append(index);
    }
    
    std::size_t& operator[](std::size_t depth)
    {
        return _address._list[depth];
    }
    
    NodeAddressBuilder& setIndex(std::size_t depth, std::size_t index)
    {
        _address._list[depth] = index;
        return *this;
    }
    
    NodeAddressBuilder& setLastIndex(std::size_t index)
    {
        _address._list.last() = index;
        return *this;
    }
    
    NodeAddressBuilder& offsetLastIndex(std::ptrdiff_t delta)
    {
        _address._list.last() += delta;
        return *this;
    }
    
    std::size_t size() const
    {
        return _address.size();
    }
    
    void reserve(std::size_t size)
    {
        _address._list.reserve(size);
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
    template<typename Range>
    void initFromRange(Range&& indices)
    {
        _address._list = list_t(std::begin(indices), std::end(indices));
    }
    
    void initFromRange(const list_t& indices)
    {
        _address._list = indices;
    }
    
    void initFromRange(list_t&& indices)
    {
        _address._list = std::move(indices);
    }
    
    NodeAddress _address;
};

inline NodeAddress::NodeAddress(const std::initializer_list<std::size_t>& init)
    : NodeAddress(NodeAddressBuilder(init))
{
}

inline NodeAddress NodeAddress::parent() const
{
    assert(!isRoot());
    
    return NodeAddressBuilder(
            boost::iterator_range<NodeAddress::iterator>(begin(), --end())
        );
}

inline NodeAddress NodeAddress::operator<<(std::size_t index) const &
{
    NodeAddressBuilder builder(*this);
    builder.append(index);
    
    return std::move(builder);
}

inline NodeAddress&& NodeAddress::operator<<(std::size_t index) &&
{
    NodeAddressBuilder builder(std::move(*this));
    builder.append(index);
    
    return std::move((*this) = std::move(builder));
}

inline NodeAddress NodeAddress::operator+(NodeAddress other) const &
{
    NodeAddressBuilder builder(*this);
    builder.reserve(builder.size() + other.size());
    
    for (std::size_t index : other)
        builder.append(index);
    
    return std::move(builder);
}

inline NodeAddress&& NodeAddress::operator+(NodeAddress other) &&
{
    NodeAddressBuilder builder(*this);
    builder.reserve(builder.size() + other.size());
    
    for (std::size_t index : other)
        builder.append(index);
    
    return std::move((*this) = std::move(builder));
}

inline std::optional<NodeAddress> NodeAddress::lowerTruncate(NodeAddress other) const
{
    if (other.isRoot())
        return *this;
    if (other.commonAncestorIndex(*this) == other.end())
        return {};
    
    NodeAddressBuilder builder(begin() + other.size(), end());
    return std::move(builder);
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
    
    class iterator : public boost::iterator_facade<
            iterator,
            const NodeAddress,
            std::random_access_iterator_tag
        >
    {
    public:
        iterator() = default;
        iterator(const iterator&) = default;
        iterator(iterator&&) = default;
        
        iterator(const NodeAddress& address) : _address(address) {}
        iterator(NodeAddress&& address) : _address(std::move(address)) {}
        
        iterator& operator=(const iterator&) = default;
        iterator& operator=(iterator&&) = default;
        
    private:
        const NodeAddress& dereference() const { return _address; }
        bool equal(const iterator& other) const 
        { 
            return _address.lastIndex() == other._address.lastIndex(); 
        }
        
        void increment() { advance(1); }
        void decrement() { advance(-1); }
        
        void advance(std::ptrdiff_t d)
        {
            assert(_address.lastIndex() >= d);
            _address = NodeAddressBuilder(std::move(_address)).offsetLastIndex(d); 
        }
        
        std::ptrdiff_t distance_to(const iterator& other) const
        {
            return _address.lastIndex() - other._address.lastIndex();
        }
        
        NodeAddress _address;
        
        friend class boost::iterator_core_access;
    };
    
    iterator begin() const { return iterator(address); }
    iterator end() const { return iterator({ address.lastIndex() + length }); }
    
#ifdef ADDLE_DEBUG
    friend QDebug operator<<(QDebug debug, const NodeChunk& chunk)
    {
        return debug 
            << "address:" << chunk.address 
            << "length:"  << chunk.length;
    }
#endif
};

// STL-style comparator which arranges NodeAddress and NodeChunk into a
// breadth-first search ordering
struct NodeAddressBFSComparator
{
    bool operator()(const NodeAddress& lhs, const NodeAddress& rhs) const
    {
        if (lhs.size() != rhs.size()) 
            return lhs.size() < rhs.size();
        else 
            return lhs < rhs;
    }
    
    bool operator()(const NodeChunk& lhs, const NodeChunk& rhs) const
    {
        return (*this)(lhs.address, rhs.address);
    }
    
    bool operator()(const NodeAddress& lhs, const NodeChunk& rhs) const
    {
        return (*this)(lhs, rhs.address);
    }
    
    bool operator()(const NodeChunk& lhs, const NodeAddress& rhs) const
    {
        return (*this)(lhs.address, rhs);
    }
};

template<class OutContainer, typename InRange>
void _chunkifyAddresses_impl(OutContainer& out, InRange&& in)
{
    auto i = std::begin(in);
    auto&& end = std::end(in);
    
    using _range_ref_t = typename boost::range_reference<std::remove_reference_t<InRange>>::type;
    using address_ref_t =  boost::mp11::mp_if<
            std::is_same<std::remove_reference_t<_range_ref_t>, NodeAddress>,
            _range_ref_t&&,
            NodeAddress
        >;
    
    if (i == end) return;
    
    auto j = i;
    ++j;
    
    std::size_t span = 1;
    
    for (; j != end; ++j)
    {
        address_ref_t a1 = *i;
        address_ref_t a2 = *j;
        
#ifdef ADDLE_DEBUG
        assert(!(a1 > a2) || a2.size() > a1.size());
#endif
        
        bool isSibling = (a2.commonAncestorIndex(a1) == a2.parentIndex());
        
        if (!isSibling || a2.lastIndex() > a1.lastIndex() + span)
        {   
            out.push_back(NodeChunk { std::forward<address_ref_t>(a1), span });
            span = 1;
            i = j;
        }
        else if (Q_LIKELY(isSibling && a2.lastIndex() == a1.lastIndex() + span))
        {
            ++span;
        }
    }
    out.push_back(NodeChunk { *i, span });
    
    std::sort(out.begin(), out.end());
}

// Takes a range of NodeAddress and groups them into an equivalent collection 
// of NodeChunk. Performs optimally if addresses are sorted by breath-first 
// search.
template<class OutContainer = QList<NodeChunk>, typename InRange = QList<NodeAddress>>
OutContainer chunkifyAddresses(InRange&& in)
{
    static_assert(std::is_convertible_v<
        typename boost::range_reference<std::remove_reference_t<InRange>>::type,
        NodeAddress
    >);
    
    OutContainer result;
    
    if (std::is_convertible_v<
            typename boost::range_category<boost::remove_cv_ref_t<InRange>>::type,
            std::forward_iterator_tag
        >
        && std::is_sorted(
            std::begin(const_cast<const InRange&&>(in)),
            std::end(const_cast<const InRange&&>(in)),
            NodeAddressBFSComparator{}
        ))
    {
        _chunkifyAddresses_impl(result, std::forward<InRange>(in));
    }
    else if constexpr (std::is_convertible_v<
            typename boost::range_category<boost::remove_cv_ref_t<InRange>>::type,
            std::random_access_iterator_tag
        >
        && std::is_rvalue_reference_v<InRange&&>
        && !std::is_const_v<std::remove_reference_t<
            typename boost::range_reference<std::remove_reference_t<InRange>>::type
        >>)
    {
        std::sort(std::begin(in), std::end(in), NodeAddressBFSComparator{});
        
        _chunkifyAddresses_impl(result, std::forward<InRange>(in));
    }
    else
    {
        QVarLengthArray<NodeAddress> addresses(std::begin(in), std::end(in));
        std::sort(addresses.begin(), addresses.end(), NodeAddressBFSComparator{});
        
        _chunkifyAddresses_impl(result, addresses);
    }
    
    return result;
}

/**
 * Transforms `chunks` into a sorted set of distinct non-overlapping chunks.
 */
template<class Container = QList<NodeChunk>>
void cleanupChunkSet(Container& chunks, bool elideDescendants = true)
{
    if (Q_UNLIKELY(chunks.empty())) return;
    
    auto i = chunks.begin();
    
    auto j = i + 1;
    if (j == chunks.end()) return;
    
    if (elideDescendants)
        std::sort(chunks.begin(), chunks.end());
    else
        std::sort(chunks.begin(), chunks.end(), NodeAddressBFSComparator {});
    
    while (j != chunks.end())
    {   
        if ((*i).address.commonAncestorIndex((*j).address) 
                == (*i).address.parentIndex()
            && (*i).address.lastIndex() + (*i).length 
                >= (*j).address.lastIndex())
        {
            // i and j overlap or abutt, merge
            (*i).length = (*j).address.lastIndex() + (*j).length
                - (*i).address.lastIndex();
                
            j = chunks.erase(j);
            i = j - 1;
        }
        else if (elideDescendants && (*i).coversAddress((*j).address))
        {
            // i fully covers j (as an ancestor), elide
            j = chunks.erase(j);
            i = j - 1;
        }
        else
        {
            ++i;
            ++j;
        }
    }
    
    if (!elideDescendants)
        std::sort(chunks.begin(), chunks.end());
}

} // namespace aux_datatree

using DataTreeNodeAddress           = aux_datatree::NodeAddress;
using DataTreeNodeAddressBuilder    = aux_datatree::NodeAddressBuilder;
using DataTreeNodeChunk             = aux_datatree::NodeChunk;

} // namespace Addle

namespace std {
template<>
struct hash<::Addle::aux_datatree::NodeAddress>
{
    std::size_t operator()(const ::Addle::aux_datatree::NodeAddress& address) const
    {
        return address.hash_p();
    }
};
}

#ifdef ADDLE_TEST

QTEST_TOSTRING_IMPL_BY_QDEBUG((::Addle::aux_datatree::NodeAddress))
QTEST_TOSTRING_IMPL_BY_QDEBUG((::Addle::aux_datatree::NodeChunk))

#endif

Q_DECLARE_METATYPE(Addle::DataTreeNodeAddress);
Q_DECLARE_METATYPE(Addle::DataTreeNodeChunk);

/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef HEIRARCHYLIST_HPP
#define HEIRARCHYLIST_HPP

#include <boost/variant.hpp>

#include "utilities/collections.hpp"

#include <QSet>
#include <QMetaType>
#include <QVariantHash>
#include <QSharedData>
#include <QSharedPointer>

#include <algorithm>
#include <list>
namespace Addle {

// A generic container class whose members are arranged as a nested heirarchy
// (i.e., a variable-width tree).

// I could see this class potentially being useful as a base implementation /
// interface, providing externally useful features like iterators, visitors,
// queries about structure or contents, and a vocabulary for transactional
// changes, but with retrieval and storage handled via virtual methods. Data
// models could expose a const reference to this class essentially as a "handle"
// with the actual data being lazy, cached, compressed, dynamically generated,
// serialized, remote -- what have you.

// This would give users of the data model richer (and more sugary) features in
// the domain of trees and nodes without every comparable data model needing to
// reimplement them or clutter their API.

// Some work would also be needed if we wanted to use this class to model
// something like a DOM where every node is a value *and* a group.

// At some point, we'll want a NodeId class (probably backed with QVariant)
// instead of externally storing pointers to Nodes so subclasses can dynamically
// generate and delete node objects if they want

// TODO: optimize comparison operations for deep trees
// TODO: clean up unneeded / redundant friend declarations

template<typename T>
class HeirarchyList
{
public:
    class Node
    {
        static constexpr int WHICH_GROUP = 0;
        static constexpr int WHICH_VALUE = 1;

    public:
        class Iterator
        {
        public:
            inline Iterator(const Iterator& x) = default;

            inline Node& operator*() const { return *_cursor; }
            inline Node* operator->() const { return _cursor; }

            inline bool operator==(const Iterator& x) const { return _cursor == x._cursor; }
            inline bool operator!=(const Iterator& x) const { return !(*this == x); }

            inline Iterator& operator++();
            inline Iterator operator++(int);

        private:
            inline Iterator(Node* cursor)
                : _cursor(cursor)
            {
            }
            Node* _cursor;
            friend class Node;
        };

        class ConstIterator
        {
        public:
            inline ConstIterator(const ConstIterator& x) = default;

            inline const Node& operator*() const { return *_cursor; }
            inline const Node* operator->() const { return _cursor; }

            inline bool operator==(const ConstIterator& x) const { return _cursor == x._cursor; }
            inline bool operator!=(const ConstIterator& x) const { return !(*this == x); }

            inline ConstIterator& operator++();
            inline ConstIterator operator++(int);

        private:
            inline ConstIterator(const Node* cursor) 
                : _cursor(cursor)
            {
            }
            const Node* _cursor;
            friend class Node;
        };

        inline Node& operator=(const Node& x);
        inline Node& operator=(Node&& x);
        inline Node& operator=(const T& value)
        {
            _value = value;
            return *this;
        }

        // template<typename Iterable>
        // inline Node& operator=(Iterable list)
        // {
        //     copyFrom(list.begin(), list.end());
        //     return *this;
        // }

        // template<typename Iterator_>
        // void copyFrom(const Iterator_& begin, const Iterator_& end)
        // {
        //     QList<Node*> nodes;

        //     for (Iterator_ i = begin; i != end; ++i)
        //     {
        //         const T& value = *i;
        //         nodes.append(Node(value, this));
        //     }
        // }

        inline Node* parent() const { return _parent; }
        inline int depth() const { return _depth; }
        inline int index() const { return _index; }
        inline QList<const Node*> path() const;

        inline const QVariant& metaData() const { return _metaData; }
        inline void setMetaData(QVariant metaData) { _metaData = metaData; }

        inline bool isGroup() const { return _value.which() == WHICH_GROUP; }
        inline bool isValue() const { return _value.which() == WHICH_VALUE; }

        inline bool isEmpty() const { return isGroup() && asGroup().isEmpty(); }
        inline int size() const { return isGroup() ? asGroup().size() : 0; }

        inline T takeFirstValue() const;

        inline bool operator==(const Node& x) const { return this == &x; }
        inline bool operator!=(const Node& x) const { return this != &x; }

        // comparisons defined in terms of absolute ordering by index
        // operations occur in unamortized linear time proportional to the depth
        // of the tree.

        inline bool operator<(const Node& x) const;
        inline bool operator<=(const Node& x) const { return (*this == x) || (*this < x); }
        inline bool operator>(const Node& x) const;
        inline bool operator>=(const Node& x) const { return (*this == x) || (*this > x); }

        // Assumes isValue

        inline T& asValue() { return boost::get<T&>(_value); }
        inline const T& asValue() const { return boost::get<const T&>(_value); }

        // Assumes isGroup

        inline Node& addValue(const T& value, int insertBefore = -1);
        inline Node& addGroup(int insertBefore = -1);

        inline Iterator begin();
        inline ConstIterator begin() const { return const_begin(); }
        inline ConstIterator const_begin() const;

        inline Iterator end();
        inline ConstIterator end() const { return const_end(); }
        inline ConstIterator const_end() const;

        inline const QList<Node*>& asGroup() const { return boost::get<const QList<Node*>&>(_value); }

        // Replace with nextSibling() function on Iterator ?
        inline typename QList<Node*>::iterator children_begin() { return asGroup().begin(); }
        inline typename QList<Node*>::iterator children_end() { return asGroup().end(); }

        inline Node& operator[](int index) { return *asGroup()[index]; }
        inline const Node& operator[](int index) const { return *asGroup()[index]; }
        inline const Node& at(int index) const { return *asGroup()[index]; }

        inline QList<T> takeValues() const;
        inline QList<Node*> takeDescendants(bool andSelf = true) const;

        inline void remove(int index);

        // Node will need to provide all non-const access and modification 
        // operations for its children to enforce that children are created with
        // correct parentage and deleted with correct pointer management.

        // Const access can be provided through the public const asGroup()

        // for use with ordered containers
        struct Comparator
        {
            inline bool operator()(const Node& a, const Node& b) const { return a < b; }
            inline bool operator()(const Node* a, const Node* b) const { return a && b && *a < *b; }
        };

        inline ~Node() noexcept;

        // Denotes an instance of a node having been removed from a heirarchy list
        class Removed
        {
            static constexpr int WHICH_NODE = 0;
            static constexpr int WHICH_REMOVED = 1;

        public:
            typedef typename HeirarchyList<T>::Node Node;
            typedef boost::variant<const Node*, boost::recursive_wrapper<Removed>> parent_t;

        private:
            struct RemovedData : QSharedData
            {
                inline RemovedData(QSharedPointer<const Node> node_, parent_t parent_, int index_, int depth_, int globalIndex_)
                    : node(node_), parent(parent_), index(index_), depth(depth_), globalIndex(globalIndex_)
                {
                }

                QSharedPointer<const Node> node;
                parent_t parent;
                QList<Removed> children;

                int index = -1;
                int depth = 0;
                int globalIndex = -1;
            };

        public:
            Removed() = default;
            Removed(const Removed&) = default;

            inline bool operator==(const Removed& x) const
            { 
                return _data == x._data
                    || _data->node == x._data->node
                    || ( _data->node && x._data->node
                        && *_data->node == *x._data->node );
            }
            inline bool operator!=(const Removed& x) const { return !(*this == x); }

            inline bool operator<(const Removed& x) const;
            inline bool operator<=(const Removed& x) const { return (*this == x) || (*this < x); }
            inline bool operator>(const Removed& x) const;
            inline bool operator>=(const Removed& x) const { return (*this == x) || (*this > x); }

            // inline bool operator==(const Node& x) const { return _data->node == &x || (_data->node && *_data->node == x); }
            // inline bool operator!=(const Node& x) const { return !(*this == x); }

            // inline bool operator<(const Node& x) const;
            // inline bool operator<=(const Node& x) const { return (*this == x) || (*this < x); }
            // inline bool operator>(const Node& x) const;
            // inline bool operator>=(const Node& x) const { return (*this == x) || (*this > x); }

            inline explicit operator bool() const { return _data && !_data->node.isNull(); }
            inline bool operator!() const { return !_data || _data->node.isNull(); }

            inline QSharedPointer<const Node> node() const { return _data->node; }
            inline const Node& operator*() const { return *_data->node; }
            inline QSharedPointer<const Node> operator->() const { return _data->node; }

            inline parent_t parent() const { return _data->parent; }
            inline int index() const { return _data->index; }
            inline int depth() const { return _data->depth; }

            bool parentIsNode() const { return _data && _data->parent.which() == WHICH_NODE; }
            bool parentIsRemoved() const { return _data && _data->parent.which() == WHICH_REMOVED; }

        private:
            struct adapter_compareParent
            {
                adapter_compareParent(const parent_t& parent)
                    : _parent(parent)
                {
                }

                inline parent_t nextAncestor() const;
                inline int index() const;
                inline int depth() const;

                inline bool operator!() const { return !((bool)(*this)); }
                explicit operator bool() const;

                bool operator==(const Removed& removed) const;
                bool operator!=(const Removed& removed) const { return !(*this == removed); }

                // equality operators when Node is virtual

            private:
                struct visitor_isValid
                {
                    typedef bool result_type;
                    template<typename V>
                    bool operator()(const V& x) const { return (bool)x; }
                };
                const parent_t& _parent;
            };

            inline Removed(QSharedPointer<const Node> node, parent_t parent, int index, int depth, int globalIndex = -1)
                : _data(new RemovedData(node, parent, index, depth, globalIndex))
            {
            }

            QSharedDataPointer<RemovedData> _data;

            friend class HeirarchyList<T>;

            // friend bool operator==(const Node& x, const Removed& y) { return y == x; }
            // friend bool operator!=(const Node& x, const Removed& y) { return y != x; }

            // friend bool operator<(const Node& x, const Removed& y) { return y > x; }
            // friend bool operator<=(const Node& x, const Removed& y) { return y >= x; }
            // friend bool operator>(const Node& x, const Removed& y) { return y < x; }
            // friend bool operator>=(const Node& x, const Removed& y) { return y <= x; }

            friend uint qHash(const Removed& x, uint seed = 0) 
            { 
                return x._data ?
                    QT_PREPEND_NAMESPACE(qHash(x._data->node, seed)) : 
                    QT_PREPEND_NAMESPACE(qHash(nullptr, seed));
            }
        };

    private:
        struct ListStats
        {
            QSet<Node*> nodes;
            QList<int> depths;
            
            inline void add(Node* node);
            inline void remove(Node* node);
        };

        struct adapter_compareParent
        {
            adapter_compareParent(Node* parent)
                : _parent(parent)
            {
            }

            inline Node* nextAncestor() const { return _parent->parent(); }
            inline int index() const { return _parent->index(); }
            inline int depth() const { return _parent->depth(); }

            bool operator!() const { return !_parent; }
            explicit operator bool() const { return (bool)_parent; }

            bool operator==(const Node& node) const { return _parent && *_parent == node; }
            bool operator!=(const Node& node) const { return !(*this == node); }

            // equality operators when Node is virtual

        private:
            Node* _parent;
        };

        inline Node() = default; //?

        inline Node(const T& value, Node* parent, int index)
            : _value(value), _parent(parent), _index(index)
        {
            if (_parent)
            {
                _depth = _parent->_depth + 1;
                _stats = _parent->_stats;
            }
            else 
            {
                _stats = new ListStats;
            }
        }

        inline Node(const QList<Node*>& list, Node* parent, int index)
            : _value(list), _parent(parent), _index(index)
        {
            if (_parent)
            {
                _depth = _parent->_depth + 1;
                _stats = _parent->_stats;
            }
            else 
            {
                _stats = new ListStats;
            }
        }

        inline Node(const Node& x);        
        inline Node(Node&& x) noexcept;

        inline void copyValue(const Node& x);

        inline Node* next(bool descend = true);
        inline const Node* next(bool descend = true) const;

        inline Node* find(const T& value) const;

        //non-const overload is private
        inline QList<Node*>& asGroup_p() { return boost::get<QList<Node*>&>(_value); }

        boost::variant<QList<Node*>, T> _value;
        Node* _parent = nullptr;

        int _index = -1;
        int _depth = 0;

        bool _manageChildren = true;

        QVariant _metaData;

        ListStats* _stats;
        friend class HeirarchyList<T>;
    };

    typedef typename Node::Removed NodeRemoved;

    inline HeirarchyList()
        : _root(QList<Node*>(), nullptr, -1)
    {
    }

    inline HeirarchyList(const HeirarchyList&) = default;

    inline Node& addValue(const T& value, int insertBefore = -1) { return _root.addValue(value, insertBefore); }
    inline Node& addGroup(int insertBefore = -1) { return _root.addGroup(insertBefore); }
    inline Node& adoptNode(Node& node, int insertBefore = -1);

    inline void remove(int index) { _root.remove(index); }

    inline Node& operator[](int index) { return _root[index]; }
    inline const Node& operator[](int index) const { return _root[index];}
    inline const Node& at(int index) const { return _root[index]; }

    inline int size() const { return _root.size(); }
    inline bool isEmpty() const { return _root.isEmpty(); }

    inline int nodeCount() const { return _root._stats->nodes.size(); }
    inline int maxDepth() const { return _root._stats->depths.size(); }

    inline Node& root() { return _root; }
    inline const Node& root() const { return const_root(); }
    inline const Node& const_root() const { return _root; }

    typename Node::Iterator begin() { return _root.begin(); }
    typename Node::ConstIterator begin() const { return const_begin(); }
    typename Node::ConstIterator const_begin() const { return _root.const_begin(); }

    typename Node::Iterator end() { return _root.end(); }
    typename Node::ConstIterator end() const { return const_end(); }
    typename Node::ConstIterator const_end() const  { return _root.const_end(); }

    template<typename Iterator>
    QList<NodeRemoved> removeNodes(Iterator begin, Iterator end);
    QList<NodeRemoved> removeNodes(const QList<Node*>& nodes) { return removeNodes(nodes.begin(), nodes.end()); }
    QList<NodeRemoved> removeNodes(const QSet<Node*>& nodes) { return removeNodes(nodes.begin(), nodes.end()); }

    // quiet remove
    // emancipate

private:
    inline HeirarchyList(Node&& node)
        : _root(std::move(node))
    {
    }

    template<
        typename NodeType1,
        typename NodeType2,
        typename ParentAdapter1 = typename NodeType1::adapter_compareParent,
        typename ParentAdapter2 = typename NodeType1::adapter_compareParent
    >
    // Performs a STL-style comparison on two node-like objects (including, e.g.,
    // NodeRemoved or NodeMoved)
    static inline bool compareNodes(const NodeType1& x, const NodeType2& y);

    Node _root;
};

template<typename T>
inline typename HeirarchyList<T>::Node* HeirarchyList<T>::Node::next(bool descend)
{
    if (descend && isGroup() && !isEmpty())
    {
        return asGroup_p().first();
    }

    if (!_parent) return nullptr;

    QList<Node*>& siblings = _parent->asGroup_p();

    if (_index < siblings.size() - 1)
    {
        return siblings[_index + 1];
    }
    else
    {
        return _parent->next(false);
    }
}

template<typename T>
inline const typename HeirarchyList<T>::Node* HeirarchyList<T>::Node::next(bool descend) const
{
    if (descend && isGroup() && !isEmpty())
    {
        return asGroup().first();
    }

    if (!_parent) return nullptr;

    QList<Node*>& siblings = _parent->asGroup_p();

    if (_index < siblings.size() - 1)
    {
        return siblings[_index + 1];
    }
    else
    {
        return _parent->next(false);
    }
}

template<typename T>
inline typename HeirarchyList<T>::Node::Iterator& HeirarchyList<T>::Node::Iterator::operator++()
{
    _cursor = _cursor->next();
    return *this;
}

template<typename T>
inline typename HeirarchyList<T>::Node::Iterator HeirarchyList<T>::Node::Iterator::operator++(int)
{
    Node* oldCursor = _cursor;
    ++(*this);
    return Iterator(oldCursor);
}

template<typename T>
inline typename HeirarchyList<T>::Node::ConstIterator& HeirarchyList<T>::Node::ConstIterator::operator++()
{
    _cursor = _cursor->next();
    return *this;
}

template<typename T>
inline typename HeirarchyList<T>::Node::ConstIterator HeirarchyList<T>::Node::ConstIterator::operator++(int)
{
    Node* oldCursor = _cursor;
    ++(*this);
    return ConstIterator(oldCursor);
}

template<typename T>
inline typename HeirarchyList<T>::Node::Iterator HeirarchyList<T>::Node::begin()
{
    if (!isGroup() || isEmpty())
        return Iterator(nullptr);

    return Iterator(asGroup_p().first());
}

template<typename T>
inline typename HeirarchyList<T>::Node::ConstIterator HeirarchyList<T>::Node::const_begin() const
{
    if (!isGroup() || isEmpty())
        return ConstIterator(nullptr);

    return ConstIterator(asGroup().first());
}

template<typename T>
inline typename HeirarchyList<T>::Node::Iterator HeirarchyList<T>::Node::end()
{
    if (!isGroup() || isEmpty())
        return Iterator(nullptr);

    return Iterator(next(false));
}

template<typename T>
inline typename HeirarchyList<T>::Node::ConstIterator HeirarchyList<T>::Node::const_end() const
{
    if (!isGroup() || isEmpty())
        return ConstIterator(nullptr);

    return ConstIterator(next(false));
}

template <typename T>
inline void HeirarchyList<T>::Node::copyValue(const Node& x)
{
    if (x.isGroup())
    {
        auto otherChildren = x.asGroup();   
        QList<Node*> children;
        children.reserve(otherChildren.size());

        int i = 0;
        for (Node* otherNode : otherChildren)
        {
            Node* node = new Node(*otherNode);
            node->_parent = this;
            node->_index = i;
            children.append(node);
            _stats->add(node);
            ++i;
        }

        _value = children;
    }
    else
    {
        _value = x._value;
    }
}

template <typename T>
inline typename HeirarchyList<T>::Node& HeirarchyList<T>::Node::operator=(const Node& x)
{
    copyValue(x);
    _metaData = x._metaData;

    return *this;
}

template <typename T> //?
inline typename HeirarchyList<T>::Node& HeirarchyList<T>::Node::operator=(Node&& x)
{
    _value = x._value;
    _metaData = x._metaData;

    x._value = QList<Node*>();

    if (x._parent)
        x._parent->remove(x._index);

    return *this;
}

template <typename T>
inline HeirarchyList<T>::Node::Node(const Node& x)
    : _parent(x._parent), _metaData(x._metaData), _index(x._index)
{
    if (_parent)
    {
        _depth = _parent->_depth + 1;
        _stats = _parent->_stats;
    }
    else 
    {
        _stats = new ListStats;
    }

    copyValue(x);
}

template <typename T> //?
inline HeirarchyList<T>::Node::Node(Node&& x) noexcept
    : _value(x._value),
    _parent(x._parent),
    _metaData(x._metaData),
    _index(x._index)
{

    if (_parent)
    {
        _depth = _parent->_depth + 1;
        _stats = _parent->_stats;
    }
    else 
    {
        _stats = new ListStats;
    }

    _stats->remove(&x);
    x._value = QList<Node*>();
}

template <typename T>
inline HeirarchyList<T>::Node::~Node() noexcept
{
    if (_manageChildren && isGroup())
    {
        for (Node* node : asGroup_p())
        {
            delete node;
        }
    }

    if (!_parent)
        delete _stats;
}

template<typename T>
inline T HeirarchyList<T>::Node::takeFirstValue() const
{
    if (isValue()) return asValue();

    for (auto& node : *this)
    {
        if (node.isValue()) return node.asValue();
    }

    return T();
}

template<typename T>
inline typename HeirarchyList<T>::Node& HeirarchyList<T>::Node::addValue(const T& value, int insertBefore)
{
    QList<Node*>& children = asGroup_p();
    if (insertBefore == -1) insertBefore = children.size();

    Node* node = new Node(value, this, insertBefore);
    children.insert(insertBefore, node);
    _stats->add(node);

    for (int i = insertBefore + 1; i < children.size(); ++i)
    {
        children[i]->_index = i;
    }

    return *node;
}

template<typename T>
inline typename HeirarchyList<T>::Node& HeirarchyList<T>::Node::addGroup(int insertBefore)
{
    QList<Node*>& children = asGroup_p();
    if (insertBefore == -1) insertBefore = children.size();
    
    Node* node = new Node(QList<Node*>(), this, insertBefore);
    children.insert(insertBefore, node);
    _stats->add(node);

    for (int i = insertBefore + 1; i < children.size(); ++i)
    {
        children[i]->_index = i;
    }

    return *node;
}

template<typename T>
inline void HeirarchyList<T>::Node::remove(int index)
{
    auto& children = asGroup_p();

    Node* node = children.at(index);
    children.removeAt(index);
    _stats->remove(node);
    delete node;

    for (int i = index; i < children.size(); ++i)
    {
        children[i]->_index = i;
    }
}

template<typename T>
inline void HeirarchyList<T>::Node::ListStats::add(Node* node)
{
    nodes.insert(node);

    while (depths.length() - 1 < node->_depth)
    {
        depths.append(0);
    }
    ++(depths[node->_depth]);
}

template<typename T>
inline void HeirarchyList<T>::Node::ListStats::remove(Node* node)
{
    nodes.remove(node);
    
    --(depths[node->_depth]);
    while ( !depths.isEmpty() && depths.last() == 0)
    {
        depths.removeLast();
    }

    if (node->isGroup())
    {
        for (Node* child : node->asGroup_p())
        {
            remove(child);
        }
    }
}


template<typename T>
inline QList<const typename HeirarchyList<T>::Node*> HeirarchyList<T>::Node::path() const
{
    QList<const Node*> result;
    const Node* cursor = _parent;
    while (cursor != nullptr)
    {
        result.prepend(cursor);
        cursor = cursor->_parent;
    }
    return result;
}

template<typename T>
template<typename Iterator>
QList<typename HeirarchyList<T>::NodeRemoved> HeirarchyList<T>::removeNodes(Iterator begin, Iterator end)
{
    QList<NodeRemoved> result;

    QHash<Node*, QList<int>> indicesToRemove;
    QList<Node*> source(begin, end);

    std::sort(source.begin(), source.end(), typename Node::Comparator());
    
    QList<Node*> nodesRemoving_list; // std::forward_list ?
    QSet<Node*> nodesRemoving;

    for (Node* node : source)
    {
        if (nodesRemoving.contains(node))
            continue;
        
        nodesRemoving.insert(node);
        nodesRemoving_list.append(node);

        _root._stats->remove(node);

        indicesToRemove[node->parent()].append(node->index());

        if (node->isGroup())
        {
            for (Node& child : *node)
            {
                if (nodesRemoving.contains(&child))
                    continue;

                nodesRemoving.insert(&child);
                nodesRemoving_list.append(&child);
            }
        }
    }

    for (auto i = indicesToRemove.keyValueBegin(); i != indicesToRemove.keyValueEnd(); ++i)
    {
        Node* parent = (*i).first;
        const QList<int>& indices = (*i).second;
        {
            int offset = 0;

            for (int remove : indices)
            {
                parent->asGroup_p().removeAt(remove + offset);
                --offset;
            }
        }

        for (int i = indices.first();
                i < parent->asGroup_p().size();
                ++i)
        {
            parent->asGroup_p()[i]->_index = i;
        }
    }

    QHash<Node*, NodeRemoved> removesByNode;
    result.reserve(nodesRemoving_list.size());
    int i = 0;
    for (Node* node : nodesRemoving_list)
    {
        typedef typename NodeRemoved::parent_t parent_t;
        parent_t parent =
            nodesRemoving.contains(node->parent()) ?
            parent_t(removesByNode[node->parent()]) 
            : parent_t(node->parent());
        
        auto remove = NodeRemoved(
            QSharedPointer<const Node>(node),
            parent,
            node->index(),
            node->depth(),
            i
        );

        node->_manageChildren = false;

        if (removesByNode.contains(node->parent()))
        {
            removesByNode[node->parent()]._data->children.append(remove);
        }

        result.append(remove);
        removesByNode[node] = remove;
        ++i;
    }

    return result;
}

template<typename T>
template<
    typename NodeType1,
    typename NodeType2,
    typename ParentAdapter1,
    typename ParentAdapter2
>
inline bool HeirarchyList<T>::compareNodes(const NodeType1& node1, const NodeType2& node2)
{
    if (node1 == node2)
    {
        return false;
    }
    else if (!ParentAdapter1(node1.parent()) && !ParentAdapter2(node2.parent()))
    {
        // root nodes of different trees are not comparable.
        return false;
    }
    else if (node1.parent() == node2.parent()) // TODO: use adapters when Node is virtual
    {
        return node1.index() < node2.index();
    }
    else
    {
        static_assert(std::is_same<decltype(node1.parent()), decltype(ParentAdapter1(node1.parent()).nextAncestor())>::value,
            "NodeType1::parent and ParentAdapter1::nextAncestor must return same type"
        );
        static_assert(std::is_same<decltype(node2.parent()), decltype(ParentAdapter2(node2.parent()).nextAncestor())>::value,
            "NodeType2::parent and ParentAdapter2::nextAncestor must return same type"
        );

        auto cursor1 = node1.parent();
        auto cursor2 = node2.parent();

        if (ParentAdapter2(cursor2) == node1)
            return true;
        else if (ParentAdapter1(cursor1) == node2)
            return false;

        int depth1 = node1.depth() - 1;
        int depth2 = node2.depth() - 1;

        while (depth1 > depth2 && depth1 > 0)
        {
            if (cursor2 == ParentAdapter1(cursor1).nextAncestor())
                return false;

            cursor1 = ParentAdapter1(cursor1).nextAncestor();
            --depth1;
        }

        while (depth2 > depth1 && depth2 > 0)
        {
            if (cursor1 == ParentAdapter2(cursor2).nextAncestor())
                return true;

            cursor2 = ParentAdapter2(cursor2).nextAncestor();
            --depth2;
        }

        if (depth1 < 0 || depth2 < 0)
        {
            // These nodes are not on the same tree
            return false;
        }

        while (ParentAdapter1(cursor1).nextAncestor() != ParentAdapter2(cursor2).nextAncestor())
        {
            cursor1 = ParentAdapter1(cursor1).nextAncestor();
            cursor2 = ParentAdapter2(cursor2).nextAncestor();
        }

        if (!ParentAdapter1(ParentAdapter1(cursor1).nextAncestor()) && !ParentAdapter2(ParentAdapter2(cursor2).nextAncestor()))
            // Nodes on different trees are not comparable.
            return false;

        return ParentAdapter1(cursor1).index() < ParentAdapter2(cursor2).index();
    }
}

template<typename T>
inline bool HeirarchyList<T>::Node::operator<(const Node& other) const
{
    return compareNodes(*this, other);
}

template<typename T>
inline bool HeirarchyList<T>::Node::operator>(const Node& other) const
{
    return compareNodes(other, *this);
}

template <typename T>
inline bool HeirarchyList<T>::Node::Removed::operator<(const Removed& other) const
{
    if (!(*this) || !other )
        return false;
    else if (_data->globalIndex != -1 && other._data->globalIndex != -1 )
        return _data->globalIndex < other._data->globalIndex;
    else
        return compareNodes(*this, other);
}

template <typename T>
inline bool HeirarchyList<T>::Node::Removed::operator>(const Removed& other) const
{
    if (!(*this) || !other )
        return false;
    else if (_data->globalIndex != -1 && other._data->globalIndex != -1 )
        return _data->globalIndex > other._data->globalIndex;
    else
        return compareNodes(other, *this);
}

// template <typename T>
// inline bool HeirarchyList<T>::Node::Removed::operator<(const Node& other) const
// {
//     return compareNodes(*this, other);
// }

// template <typename T>
// inline bool HeirarchyList<T>::Node::Removed::operator>(const Node& other) const
// {
//     return compareNodes(other, *this);
// }

template <typename T>
inline typename HeirarchyList<T>::Node::Removed::parent_t HeirarchyList<T>::Node::Removed::adapter_compareParent::nextAncestor() const
{ 
    struct
    {
        typedef parent_t result_type;
        inline parent_t operator()(const Node* x) const { return x->parent(); }
        inline parent_t operator()(const Removed& x) const { return x._data->parent; }
    } visitor;
    return _parent.apply_visitor(visitor);
}

template <typename T>
inline int HeirarchyList<T>::Node::Removed::adapter_compareParent::index() const
{
    struct
    {
        typedef int result_type;
        inline int operator()(const Node* x)  const { return x->index(); }
        inline int operator()(const Removed& x) const { return x._data->index; }
    } visitor;
    return _parent.apply_visitor(visitor);
}

template <typename T>
inline int HeirarchyList<T>::Node::Removed::adapter_compareParent::depth() const
{ 
    struct
    {
        typedef int result_type;
        inline int operator()(const Node* x)  const { return x->depth(); }
        inline int operator()(const Removed& x) const { return x._data->depth; }
    } visitor;
    return _parent.apply_visitor(visitor);
}

template <typename T>
inline HeirarchyList<T>::Node::Removed::adapter_compareParent::operator bool() const
{ 
    visitor_isValid visitor;
    return _parent.apply_visitor(visitor);
}

template <typename T>
bool HeirarchyList<T>::Node::Removed::adapter_compareParent::operator==(const Removed& removed) const
{
    struct visitor_equals
    {
        visitor_equals(const Removed& operand)
            : _operand(operand)
        {
        }

        typedef bool result_type;
        inline bool operator()(const Node* x) const { return false; }
        inline bool operator()(const Removed& x) const { return x == _operand; }

    private:
        const Removed& _operand;
    } visitor(removed);
    return _parent.apply_visitor(visitor);
}

// template <typename T>
// inline bool HeirarchyList<T>::Node::Removed::parent_t::operator==(const Node* node) const
// { 
//     return _removed.apply_visitor(visitor_equals<const Node*>(node));
// }

// template <typename T>
// inline bool HeirarchyList<T>::Node::Removed::parent_t::operator==(const Removed& removed) const
// { 
//     return _removed.apply_visitor(visitor_equals<Removed>(removed));
// }

} // namespace Addle

#endif // HEIRARCHYLIST_HPP
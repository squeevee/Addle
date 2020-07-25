#ifndef HEIRARCHYLIST_HPP
#define HEIRARCHYLIST_HPP

#include <boost/variant.hpp>

#include "utilities/qtextensions/qhash.hpp"
#include "utilities/qtextensions/qlist.hpp"

#include <QSet>
#include <QMetaType>
#include <QVariantHash>
#include <QSharedData>
#include <QSharedPointer>

/**
 * A generic container class whose members are arranged as a nested heirarchy.
 */
template<typename T>
class HeirarchyList
{
public:
    class Node
    {
        static constexpr int W_GROUP = 0;
        static constexpr int W_VALUE = 1;

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
            inline Iterator(Node* cursor, int index);
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

        template<typename Iterator_>
        void copyFrom(const Iterator_& begin, const Iterator_& end)
        {
            QList<Node*> nodes;

            for (Iterator_ i = begin; i != end; ++i)
            {
                const T& value = *i;
                nodes.append(Node(value, this));
            }
        }

        inline Node* parent() const { return _parent; }
        inline int depth() const { return _depth; }
        inline int index() const { return _index; }
        inline QList<const Node*> path() const;

        inline QVariant& metaData() { return _metaData; }
        inline const QVariant& metaData() const { return _metaData; }
        inline const QVariant& constMetaData() { return _metaData; }

        inline bool isGroup() const { return _value.which() == W_GROUP; }
        inline bool isValue() const { return _value.which() == W_VALUE; }

        inline bool isEmpty() const { return isGroup() && asGroup().isEmpty(); }
        inline int size() const { return isGroup() ? asGroup().size() : 0; }

        inline T takeFirstValue() const;

        //inline void dispose();

        // removes this node from its parent without destroying it. values of 
        // index() and parent() continue to point to their former location
        // (unless/until the node is inserted into another list). The caller
        // becomes responsible for deleting this node.
        // behavior is (currently) undefined if this is a root node
        inline void emancipate();

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

        // Transfers ownership of node as a child of this
        inline Node& adoptNode(Node& node, int insertBefore = -1);

        // inline NodePointer<T> copyNode(const Node& node, int insertBefore = -1);

        inline Iterator begin();
        inline ConstIterator begin() const { return const_begin(); }
        inline ConstIterator const_begin() const;

        inline Iterator end();
        inline ConstIterator end() const { return const_end(); }
        inline ConstIterator const_end() const;

        inline const QList<Node*>& asGroup() const { return boost::get<const QList<Node*>&>(_value); }

        inline typename QList<Node*>::iterator children_begin() { return asGroup().begin(); }
        inline typename QList<Node*>::iterator children_end() { return asGroup().end(); }

        inline Node& operator[](int index) { return *asGroup()[index]; }
        inline const Node& operator[](int index) const { return *asGroup()[index]; }
        inline const Node& at(int index) const { return *asGroup()[index]; }

        inline QList<T> takeValues() const;
        inline QList<Node*> takeDescendants(bool andSelf = true) const;

        inline void remove(int index);

        //template<typename Visitor>
        //inline typename Visitor::result_type apply_visitor(const Visitor& v) const;

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

    private:
        inline Node() = default;

        inline Node(const T& value, Node* parent, int index)
            : _value(value), _parent(parent), _index(index)
        {
            if (_parent)
                _depth = _parent->_depth + 1;
        }

        inline Node(const QList<Node*>& list, Node* parent, int index)
            : _value(list), _parent(parent), _index(index)
        {
            if (_parent)
                _depth = _parent->_depth + 1;
        }

        inline Node(const Node& x);        
        inline Node(Node&& x) noexcept;

        inline ~Node() noexcept;

        inline void copyValue(const Node& x);

        inline Node* next(bool descend = true);
        inline const Node* next(bool descend = true) const;

        inline Node* find(const T& value) const;

        //non-const overload is private
        inline QList<Node*>& asGroup() { return boost::get<QList<Node*>&>(_value); }

        boost::variant<QList<Node*>, T> _value;
        Node* _parent = nullptr;

        int _index = -1;
        int _depth = 0;

        QVariant _metaData;

        friend class QList<Node*>;
        friend class HeirarchyList<T>;
    };

    inline HeirarchyList();
    // inline HeirarchyList(std::initializer_list<T> list)
    //     : _root(QList<Node*>(list), nullptr, 0)
    // {
    // }

    inline HeirarchyList(const HeirarchyList&) = default;

    inline Node& addValue(const T& value, int insertBefore = -1);
    inline Node& addGroup(int insertBefore = -1);
    inline Node& adoptNode(Node& node, int insertBefore = -1);

    inline void remove(int index);

    inline Node& operator[](int index);
    inline const Node& operator[](int index) const;
    inline const Node& at(int index) const;

    inline int size() const;
    inline bool isEmpty() const;

    inline Node& root();
    inline const Node& root() const { return const_root(); }
    inline const Node& const_root() const;

    typename Node::Iterator begin();
    typename Node::ConstIterator begin() const { return const_begin(); }
    typename Node::ConstIterator const_begin() const;

    typename Node::Iterator end();
    typename Node::ConstIterator end() const { return const_end(); }
    typename Node::ConstIterator const_end() const;

private:
    inline HeirarchyList(Node&& node);

    struct Data : QSharedData
    {
        inline Data(Node&& root_) : root(std::move(root_)) { }
        inline Data(const T& value)
            : root(value, nullptr, -1)
        {
        }

        inline Data(QList<Node*> list)
            : root(list, nullptr, -1)
        {
        }

        Node root;
    };
    
    QSharedDataPointer<Data> _data;

    friend class HeirarchyList<T>::Node;
};

template<typename T>
inline typename HeirarchyList<T>::Node* HeirarchyList<T>::Node::next(bool descend)
{
    if (descend && isGroup() && !isEmpty())
    {
        return asGroup().first();
    }

    if (!_parent) return nullptr;

    QList<Node*>& siblings = _parent->asGroup();

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

    QList<Node*>& siblings = _parent->asGroup();

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

    return Iterator(asGroup().first());
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
    copyValue(x);

    if (_parent)
        _depth = _parent->_depth + 1;
}

template <typename T> //?
inline HeirarchyList<T>::Node::Node(Node&& x) noexcept
    : _value(x._value),
    _parent(x._parent),
    _metaData(x._metaData),
    _index(x._index)
{
    x._value = QList<Node*>();

    if (_parent)
        _depth = _parent->_depth + 1;
}

template <typename T>
inline HeirarchyList<T>::Node::~Node() noexcept
{
    if (isGroup())
    {
        for (Node* node : asGroup())
        {
            delete node;
        }
    }
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
inline typename HeirarchyList<T>::Node& HeirarchyList<T>::Node::adoptNode(Node& node, int insertBefore)
{
    auto& children = asGroup();
    if (insertBefore == -1) insertBefore = children.size();

    children.insert(insertBefore, &node);
    if (node._parent && node._parent->asGroup().at(node._index) == &node)
    {
        node._parent->remove(node.index());
    }
    else
    {
        _value = QList<Node*>();
        _metaData = QVariant();
    }

    node._parent = this;
    node._index = insertBefore;
    node._depth = _depth + 1;

    for (int i = insertBefore + 1; i < children.size(); ++i)
    {
        children[i]->_index = i;
    }

    return *children[insertBefore];
}

template<typename T>
inline typename HeirarchyList<T>::Node& HeirarchyList<T>::Node::addValue(const T& value, int insertBefore)
{
    QList<Node*>& children = asGroup();
    if (insertBefore == -1) insertBefore = children.size();

    children.insert(insertBefore, new Node(value, this, insertBefore));

    for (int i = insertBefore + 1; i < children.size(); ++i)
    {
        children[i]->_index = i;
    }

    return *children[insertBefore];
}

template<typename T>
inline typename HeirarchyList<T>::Node& HeirarchyList<T>::Node::addGroup(int insertBefore)
{
    QList<Node*>& children = asGroup();
    if (insertBefore == -1) insertBefore = children.size();
    
    children.insert(insertBefore, new Node(QList<Node*>(), this, insertBefore));

    for (int i = insertBefore + 1; i < children.size(); ++i)
    {
        children[i]->_index = i;
    }

    return *children[insertBefore];
}

template<typename T>
inline void HeirarchyList<T>::Node::remove(int index)
{
    auto& children = asGroup();

    //auto stash = stashPointers(index + count);

    delete children.at(index);
    children.removeAt(index);

    for (int i = index; i < children.size(); ++i)
    {
        children[i]->_index = i;
    }

    //applyPointers(stash, index);
}

template<typename T>
inline void HeirarchyList<T>::Node::emancipate()
{
    auto& siblings = _parent->asGroup();
    siblings.removeAt(_index);

    for (int i = _index; i < siblings.size(); ++i)
    {
        siblings[i]->_index = i;
    }
}

template<typename T>
inline HeirarchyList<T>::HeirarchyList()
    : _data(new Data(QList<Node*>()))
{
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
inline bool HeirarchyList<T>::Node::operator<(const Node& other) const
{
    if (*this == other)
    {
        // equal nodes are equivalent
        return false;
    }
    if (_parent == other._parent)
    {
        return index() < other.index();
    }
    else
    {
        QList<const Node*> pathA = path();
        QList<const Node*> pathB = other.path();

        auto i = pathA.begin();
        auto j = pathB.begin();

        if (*i != *j && *i != &other && *j != this)
            return false; // Nodes do not have a common ancestor.

        while (true)
        {
            if (i == pathA.end())
            {
                // The paths have not diverged but pathA has ended. This means
                // That x is a descendant of this
                return true;
            }

            if (j == pathB.end())
            {
                // "" this is a descendant of x.
                return false;
            }

            if (*i != *j)
            {
                // The paths have diverged
                return *i < *j;
            }

            ++i;
            ++j;
        }
    }
}

template<typename T>
inline bool HeirarchyList<T>::Node::operator>(const Node& other) const
{
if (*this == other)
    {
        return false;
    }
    if (_parent == other._parent)
    {
        return index() > other.index();
    }
    else
    {
        QList<const Node*> pathA = path();
        QList<const Node*> pathB = other.path();

        auto i = pathA.begin();
        auto j = pathB.begin();

        if (*i != *j && *i != &other && *j != this) 
            return false; // Nodes do not have a common ancestor.

        while (true)
        {
            if (i == pathA.end())
            {
                // The paths have not diverged but pathA has ended. This means
                // That x is a descendant of this
                return false;
            }

            if (j == pathB.end())
            {
                // "" this is a descendant of x.
                return true;
            }

            if (*i != *j)
            {
                // The paths have diverged
                return *i > *j;
            }

            ++i;
            ++j;
        }
    }
}

template<typename T>
inline typename HeirarchyList<T>::Node& HeirarchyList<T>::addValue(const T& value, int insertBefore) 
{ 
    return _data->root.addValue(value, insertBefore);
}
template<typename T>
inline typename HeirarchyList<T>::Node& HeirarchyList<T>::addGroup(int insertBefore)
{ 
    return _data->root.addGroup(insertBefore);
}

template<typename T>
inline typename HeirarchyList<T>::Node& HeirarchyList<T>::operator[](int index)
{
    return _data->root[index];
}

template<typename T>
inline const typename HeirarchyList<T>::Node& HeirarchyList<T>::operator[](int index) const
{
    return _data->root[index];
}

template<typename T>
inline const typename HeirarchyList<T>::Node& HeirarchyList<T>::at(int index) const
{
    return _data->root[index];
}

template<typename T>
inline int HeirarchyList<T>::size() const { return _data->root.size(); }

template<typename T>
inline bool HeirarchyList<T>::isEmpty() const { return _data->root.isEmpty(); }

template<typename T>
inline void HeirarchyList<T>::remove(int index) { _data->root.remove(index); }

template<typename T>
inline typename HeirarchyList<T>::Node& HeirarchyList<T>::root() { return _data->root; }

template<typename T>
inline const typename HeirarchyList<T>::Node& HeirarchyList<T>::const_root() const { return _data->root; }

template<typename T>
typename HeirarchyList<T>::Node::Iterator HeirarchyList<T>::begin() { return _data->root.begin(); }

template<typename T>
typename HeirarchyList<T>::Node::ConstIterator HeirarchyList<T>::const_begin() const { return _data->root.const_begin(); }

template<typename T>
typename HeirarchyList<T>::Node::Iterator HeirarchyList<T>::end() { return _data->root.end();}

template<typename T>
typename HeirarchyList<T>::Node::ConstIterator HeirarchyList<T>::const_end() const { return _data->root.const_end(); }

template<typename T>
inline HeirarchyList<T>::HeirarchyList(Node&& node)
    : _data(new Data(std::move(node)))
{
}

#endif // HEIRARCHYLIST_HPP
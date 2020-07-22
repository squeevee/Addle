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

template<typename T>
class HeirarchyList;

namespace HeirarchyListPrivate
{
    template<typename T>
    class NodePointer;

    template<typename T>
    class ConstNodePointer;

    template<typename T>
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
            QVector<Node> nodes;

            for (Iterator_ i = begin; i != end; ++i)
            {
                const T& value = *i;
                nodes.append(Node(value, this));
            }
        }

        inline Node* parent() const { return _parent; }
        inline int depth() const;
        inline int indexInParent() const { return _parent ? this - _parent->asGroup().data() : -1; }
        inline QList<const Node*> path() const;

        inline QVariant& metaData() { return _metaData; }
        inline const QVariant& metaData() const { return _metaData; }
        inline const QVariant& constMetaData() { return _metaData; }

        inline bool isGroup() const { return _value.which() == W_GROUP; }
        inline bool isValue() const { return _value.which() == W_VALUE; }

        inline bool isEmpty() const { return isGroup() && asGroup().isEmpty(); }
        inline int childrenCount() const { return isGroup() ? asGroup().count() : 0; }

        inline T takeFirstValue() const;

        inline void dispose();
        //inline HeirarchyList<T> emancipate();

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

        inline NodePointer<T> addValue(const T& value);
        inline NodePointer<T> addGroup();

        // Transfers ownership of node as a child of this
        inline NodePointer<T> adoptNode(Node& node);

        inline NodePointer<T> copyNode(const Node& node);

        inline Iterator begin();
        inline ConstIterator begin() const { return const_begin(); }
        inline ConstIterator const_begin() const;

        inline Iterator end();
        inline ConstIterator end() const { return const_end(); }
        inline ConstIterator const_end() const;

        inline const QVector<Node>& asGroup() const { return boost::get<const QVector<Node>&>(_value); }

        inline typename QVector<Node>::iterator children_begin() { return asGroup().begin(); }
        inline typename QVector<Node>::iterator children_end() { return asGroup().end(); }

        inline Node& operator[](int index) { return asGroup()[index]; }
        inline const Node& operator[](int index) const { return asGroup()[index]; }
        inline const Node& at(int index) const { return asGroup()[index]; }

        inline QVector<T> takeValues() const;
        inline QVector<Node> takeDescendants(bool andSelf = true) const;


        inline void remove(int index, int count = 1);

        //template<typename Visitor>
        //inline typename Visitor::result_type apply_visitor(const Visitor& v) const;

        // Node will need to provide all non-const access and modification 
        // operations for its children to enforce that children are created with
        // correct parentage and deleted with correct pointer management.

        // Const access can be provided through the public const asGroup()


        // THIS CONSTRUCTOR AND DESTRUCTOR ARE PROVIDED FOR THE BENEFIT OF
        // QVector. DO NOT CREATE OR TAKE OWNERSHIP OF INSTANCES OF Node.

        inline Node(Node&& x) noexcept;
        inline ~Node() noexcept;

    private:
        inline Node() = default;

        inline Node(const T& value, Node* parent, QVariant metaData = QVariant())
            : _value(value), _parent(parent), _metaData(metaData)
        {
        }

        inline Node(QVector<Node> list, Node* parent, QVariant metaData = QVariant())
            : _value(list), _parent(parent), _metaData(metaData)
        {
        }

        inline Node(const Node& x);
/*  */
        inline void rebase() noexcept;
        inline void nullifyPointers() noexcept;

        inline QList<QSet<NodePointer<T>*>> stashPointers(int offset = 0);
        inline void applyPointers(QList<QSet<NodePointer<T>*>> pointers, int offset = 0);

        inline Node* next(bool descend = true);
        inline const Node* next(bool descend = true) const;

        inline Node* find(const T& value) const;

        //non-const overload is private
        inline QVector<Node>& asGroup() { return boost::get<QVector<Node>&>(_value); }

        boost::variant<QVector<Node>, T> _value;
        Node* _parent = nullptr;

        QVariant _metaData;

        QSet<NodePointer<T>*> _pointers;
        QSet<ConstNodePointer<T>*> _constPointers;

        friend class QVector<Node>;
        friend class HeirarchyList<T>;
        friend class NodePointer<T>;
        friend class ConstNodePointer<T>;
    };

    template<typename T>
    uint qHash(const NodePointer<T>& pointer, uint seed = 0);

    template<typename T>
    class NodePointer
    {
    public:
        inline NodePointer() : _ptr(nullptr) { }
        inline NodePointer(const NodePointer& x);

        inline NodePointer(Node<T>* node);

        inline ~NodePointer();

        inline NodePointer& operator=(const NodePointer& x);
        inline NodePointer& operator=(Node<T>* node);

        explicit inline operator bool() const { return (bool)_ptr; }
        inline bool operator!() const { return !_ptr; }
        inline bool isNull() const { return !_ptr; }

        inline bool operator==(const NodePointer& x) const { return _ptr == x._ptr; }
        inline bool operator==(const Node<T>* node) const { return _ptr == node; }

        inline bool operator!=(const NodePointer& x) const { return _ptr != x._ptr; }
        inline bool operator!=(const Node<T>* node) const { return _ptr != node; }

        inline bool operator<(const NodePointer& x) const
        {
            return _ptr && x._ptr && *_ptr < *x._ptr;
        }

        inline bool operator<=(const NodePointer& x) const
        {
            return (*this == x) || (*this < x);
        }

        inline bool operator>(const NodePointer& x) const
        {
            return _ptr && x._ptr && *_ptr > *x._ptr;
        }

        inline bool operator>=(const NodePointer& x) const
        {
            return (*this == x) || (*this > x);
        }

        inline Node<T>& operator*() const { return *_ptr; }
        inline Node<T>* operator->() const { return _ptr; }

        // // because I can't seem to define a qHash function in global namespace
        // // and have it find this class through template deduction
        // inline operator quintptr() const noexcept { return reinterpret_cast<quintptr>(_ptr); }

    private:
        Node<T>* _ptr;

        friend uint qHash<T>(const NodePointer<T>&, uint);
        friend class Node<T>;
    };

    template<typename T>
    inline uint qHash(const NodePointer<T>& pointer, uint seed)
    {
        return QT_PREPEND_NAMESPACE(qHash(reinterpret_cast<quintptr>(pointer._ptr), seed));
    }
}

/**
 * A generic container class whose members are arranged as a nested heirarchy.
 */
template<typename T>
class HeirarchyList
{
public:
    typedef HeirarchyListPrivate::Node<T> Node;
    typedef HeirarchyListPrivate::NodePointer<T> NodePointer;

    inline HeirarchyList();
    // inline HeirarchyList(std::initializer_list<T> list)
    //     : _root(QVector<Node>(list), nullptr, 0)
    // {
    // }

    inline HeirarchyList(const HeirarchyList&) = default;

    inline NodePointer addValue(const T& value);
    inline NodePointer addGroup();

    inline Node& operator[](int index);
    inline const Node& operator[](int index) const;
    inline const Node& at(int index) const;

    inline int count() const;

    inline Node& root();
    inline const Node& root() const;

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
        inline Data(const T& value, Node* parent, QVariant metaData)
            : root(value, parent, metaData)
        {
        }

        inline Data(QVector<Node> list, Node* parent, QVariant metaData)
            : root(list, parent, metaData)
        {
        }

        Node root;
    };
    
    QSharedDataPointer<Data> _data;

    friend class HeirarchyListPrivate::Node<T>;
};

template<typename T>
inline void HeirarchyListPrivate::Node<T>::rebase() noexcept
{
    if (isGroup() && !isEmpty())
    {
        for (auto& node: asGroup())
        {
            node._parent = this;
        }
    }

    for (auto ref : qAsConst(_pointers))
    {
        ref->_ptr = this;
    }
    for (auto cRef : qAsConst(_constPointers))
    {
        //cRef->_ptr = this;
    }
}

template<typename T>
inline void HeirarchyListPrivate::Node<T>::nullifyPointers() noexcept
{
    for (auto ref : qAsConst(_pointers))
    {
        ref->_ptr = nullptr;
    }
    for (auto cRef : qAsConst(_constPointers))
    {
        //cRef->_ptr = nullptr;
    }
}

template<typename T>
inline QList<QSet<HeirarchyListPrivate::NodePointer<T>*>> HeirarchyListPrivate::Node<T>::stashPointers(int offset)
{
    auto& children = asGroup();

    QList<QSet<NodePointer<T>*>> pointers;
    pointers.reserve(children.count() - offset);

    for (int i = offset; i < children.count(); i++)
    {
        auto& child = children[i];
        pointers.append(child._pointers);
        child._pointers.clear();
    }

    return pointers;
}

template<typename T>
inline void HeirarchyListPrivate::Node<T>::applyPointers(QList<QSet<NodePointer<T>*>> pointers, int offset)
{
    auto& children = asGroup();

    for (int i = 0; i < qMin(pointers.count(), children.count() + offset); i++)
    {
        auto& child = children[i + offset];
        child._pointers = pointers[i];
        child.rebase();
    }
}

template<typename T>
inline typename HeirarchyListPrivate::Node<T>* HeirarchyListPrivate::Node<T>::next(bool descend)
{
    if (!_parent) return nullptr;

    if (descend && isGroup() && !isEmpty())
    {
        return &asGroup().first();
    }

    QVector<Node>& siblings = _parent->asGroup();
    ptrdiff_t index = this - siblings.data();

    if (index < siblings.count() - 1)
    {
        return &siblings[index + 1];
    }
    else
    {
        return _parent->next(false);
    }
}

template<typename T>
inline const typename HeirarchyListPrivate::Node<T>* HeirarchyListPrivate::Node<T>::next(bool descend) const
{
    if (descend && isGroup() && !isEmpty())
    {
        const QVector<Node>& children = asGroup();
        return &children.constFirst();
    }    
    
    if (!_parent) return nullptr;

    const QVector<Node>& siblings = _parent->asGroup();
    ptrdiff_t index = this - siblings.data();

    int c = siblings.count();
    if (index < siblings.count() - 1)
    {
        return &siblings[index + 1];
    }
    else
    {
        return _parent->next(false);
    }
}

template<typename T>
inline typename HeirarchyListPrivate::Node<T>::Iterator& HeirarchyListPrivate::Node<T>::Iterator::operator++()
{
    _cursor = _cursor->next();
    return *this;
}

template<typename T>
inline typename HeirarchyListPrivate::Node<T>::Iterator HeirarchyListPrivate::Node<T>::Iterator::operator++(int)
{
    Node* oldCursor = _cursor;
    ++(*this);
    return Iterator(oldCursor);
}

template<typename T>
inline typename HeirarchyListPrivate::Node<T>::ConstIterator& HeirarchyListPrivate::Node<T>::ConstIterator::operator++()
{
    _cursor = _cursor->next();
    return *this;
}

template<typename T>
inline typename HeirarchyListPrivate::Node<T>::ConstIterator HeirarchyListPrivate::Node<T>::ConstIterator::operator++(int)
{
    Node* oldCursor = _cursor;
    ++(*this);
    return ConstIterator(oldCursor);
}

template<typename T>
inline typename HeirarchyListPrivate::Node<T>::Iterator HeirarchyListPrivate::Node<T>::begin()
{
    if (!isGroup() || isEmpty())
        return Iterator(nullptr);

    return Iterator(&asGroup().first());
}

template<typename T>
inline typename HeirarchyListPrivate::Node<T>::ConstIterator HeirarchyListPrivate::Node<T>::const_begin() const
{
    if (!isGroup() || isEmpty())
        return ConstIterator(nullptr);

    return ConstIterator(&asGroup().first());
}

template<typename T>
inline typename HeirarchyListPrivate::Node<T>::Iterator HeirarchyListPrivate::Node<T>::end()
{
    if (!isGroup() || isEmpty())
        return Iterator(nullptr);

    return Iterator(next(false));
}

template<typename T>
inline typename HeirarchyListPrivate::Node<T>::ConstIterator HeirarchyListPrivate::Node<T>::const_end() const
{
    if (!isGroup() || isEmpty())
        return ConstIterator(nullptr);

    return ConstIterator(next(false));
}

template <typename T>
inline typename HeirarchyListPrivate::Node<T>& HeirarchyListPrivate::Node<T>::operator=(const Node& x)
{
    _value = x._value;
    _metaData = x._metaData;

    nullifyPointers();

    _pointers = x._pointers;
    _constPointers = x._constPointers;

    rebase();
    return *this;
}

template <typename T>
inline typename HeirarchyListPrivate::Node<T>& HeirarchyListPrivate::Node<T>::operator=(Node&& x)
{
    _value = x._value;
    _metaData = x._metaData;

    nullifyPointers();

    _pointers = x._pointers;
    _constPointers = x._constPointers;

    rebase();

    x._pointers.clear();
    x._constPointers.clear();

    return *this;
}

template <typename T>
inline HeirarchyListPrivate::Node<T>::Node(const Node& x)
    : _value(x._value), _parent(x._parent), _metaData(x._metaData)
{
    rebase();
}

template <typename T>
inline HeirarchyListPrivate::Node<T>::Node(Node&& x) noexcept
    : _value(x._value),
    _parent(x._parent),
    _metaData(x._metaData),
    _pointers(x._pointers),
    _constPointers(x._constPointers)
{
    x._value = QVector<Node>();
    x._pointers.clear();
    x._constPointers.clear();

    rebase();
}

template <typename T>
inline HeirarchyListPrivate::Node<T>::~Node() noexcept
{
    nullifyPointers();
}

template<typename T>
inline T HeirarchyListPrivate::Node<T>::takeFirstValue() const
{
    if (isValue()) return asValue();

    for (auto& node : *this)
    {
        if (node.isValue()) return node.asValue();
    }

    return T();
}

template<typename T>
inline HeirarchyListPrivate::NodePointer<T> HeirarchyListPrivate::Node<T>::adoptNode(Node& node)
{
    auto& children = asGroup();

    children.append(std::move(node));
    if (node._parent)
    {
        node._parent->remove(node.indexInParent());
    }
    else
    {
        _value = QVector<Node>();
        _metaData = QVariant();
    }

    node._parent = this;
    node.rebase();

    return &children.last();
}

template<typename T>
inline HeirarchyListPrivate::NodePointer<T> HeirarchyListPrivate::Node<T>::addValue(const T& value)
{
    QVector<Node>& children = asGroup();

    //auto stash = stashPointers();
    children.append(Node(value, this));
    //applyPointers(stash);
    rebase();
    return NodePointer<T>(&children.last());
}

template<typename T>
inline HeirarchyListPrivate::NodePointer<T> HeirarchyListPrivate::Node<T>::addGroup()
{
    QVector<Node>& children = asGroup();
    
    //auto stash = stashPointers();
    children.append(Node(QVector<Node>(), this));
    //applyPointers(stash);
    rebase();
    return NodePointer<T>(&children.last());
}

template<typename T>
inline void HeirarchyListPrivate::Node<T>::remove(int index, int count)
{
    auto& children = asGroup();

    auto stash = stashPointers(index + count);
    children.remove(index, count);
    applyPointers(stash, index);
}

template<typename T>
inline HeirarchyListPrivate::NodePointer<T>::NodePointer(const NodePointer& x)
    : _ptr(x._ptr)
{
    if (_ptr)
        _ptr->_pointers.insert(this);
}

template<typename T>
inline HeirarchyListPrivate::NodePointer<T>::NodePointer(Node<T>* node)
    : _ptr(node)
{
    if (_ptr)
        _ptr->_pointers.insert(this);
}

template<typename T>
inline HeirarchyListPrivate::NodePointer<T>::~NodePointer()
{
    if (_ptr)
        _ptr->_pointers.remove(this);
}

template<typename T>
inline typename HeirarchyListPrivate::NodePointer<T>& HeirarchyListPrivate::NodePointer<T>::operator=(const NodePointer& x)
{
    if (*this != x)
    {
        if (_ptr)
            _ptr->_pointers.remove(this);

        _ptr = x._ptr;

        if (_ptr)
            _ptr->_pointers.insert(this);
    }
    return *this;
}

template<typename T>
inline typename HeirarchyListPrivate::NodePointer<T>& HeirarchyListPrivate::NodePointer<T>::operator=(Node<T>* node)
{
    if (_ptr != node)
    {
        if (_ptr)
            _ptr->_pointers.remove(this);
        
        _ptr = node;

        if (_ptr)
            _ptr->_pointers.insert(this);
    }
    return *this;
}

template<typename T>
inline HeirarchyList<T>::HeirarchyList()
    : _data(new Data(QVector<Node>(), nullptr, QVariant()))
{
}

template<typename T>
inline int HeirarchyListPrivate::Node<T>::depth() const
{
    int depth = 0;
    const Node* cursor = this;
    while (cursor->_parent)
    {
        cursor = cursor->_parent;
        ++depth;
    }
    return depth;
}

template<typename T>
inline QList<const HeirarchyListPrivate::Node<T>*> HeirarchyListPrivate::Node<T>::path() const
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
inline bool HeirarchyListPrivate::Node<T>::operator<(const Node& x) const
{
    if (*this == x || !_parent || !x._parent)
    {
        // equal nodes are equivalent and root nodes are not comparable.
        return false;
    }
    if (_parent == x._parent)
    {
        return indexInParent() < x.indexInParent();
    }
    else
    {
        QList<const Node*> pathA = path();
        QList<const Node*> pathB = x.path();

        auto i = pathA.begin();
        auto j = pathB.begin();

        if (*i != *j) return false; // Nodes do not have a common ancestor.

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
inline bool HeirarchyListPrivate::Node<T>::operator>(const Node& x) const
{
if (*this == x)
    {
        return false;
    }
    if (_parent == x._parent)
    {
        return indexInParent() > x.indexInParent();
    }
    else
    {
        QList<const Node*> pathA = path();
        QList<const Node*> pathB = x.path();

        auto i = pathA.begin();
        auto j = pathB.begin();

        if (*i != *j) return false; // Nodes do not have a common ancestor.

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
inline typename HeirarchyListPrivate::NodePointer<T> HeirarchyList<T>::addValue(const T& value) { return _data->root.addValue(value); }
template<typename T>
inline typename HeirarchyListPrivate::NodePointer<T> HeirarchyList<T>::addGroup() { return _data->root.addGroup(); }

template<typename T>
inline typename HeirarchyListPrivate::Node<T>& HeirarchyList<T>::operator[](int index)
{
    return _data->root[index];
}

template<typename T>
inline const typename HeirarchyListPrivate::Node<T>& HeirarchyList<T>::operator[](int index) const
{
    return _data->root[index];
}

template<typename T>
inline const typename HeirarchyListPrivate::Node<T>& HeirarchyList<T>::at(int index) const
{
    return _data->root[index];
}

template<typename T>
inline int HeirarchyList<T>::count() const
{
    _data->root->updateIndex();
    return _data->root->count();
}

template<typename T>
inline typename HeirarchyListPrivate::Node<T>& HeirarchyList<T>::root() { return _data->root; }

template<typename T>
inline const typename HeirarchyListPrivate::Node<T>& HeirarchyList<T>::root() const { return _data->root; }

template<typename T>
typename HeirarchyListPrivate::Node<T>::Iterator HeirarchyList<T>::begin() { return _data->root.begin(); }

template<typename T>
typename HeirarchyListPrivate::Node<T>::ConstIterator HeirarchyList<T>::const_begin() const { return _data->root.const_begin(); }

template<typename T>
typename HeirarchyListPrivate::Node<T>::Iterator HeirarchyList<T>::end() { return _data->root.end();}

template<typename T>
typename HeirarchyListPrivate::Node<T>::ConstIterator HeirarchyList<T>::const_end() const { return _data->root.const_end(); }

template<typename T>
inline HeirarchyList<T>::HeirarchyList(Node&& node)
    : _data(new Data(std::move(node)))
{
}

#endif // HEIRARCHYLIST_HPP
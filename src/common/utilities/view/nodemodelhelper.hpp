#ifndef NODEMODELHELPER_HPP
#define NODEMODELHELPER_HPP

#include <map>
#include <set>
#include <QList>

#include "utilities/heirarchylist.hpp"
#include <boost/variant.hpp>

// Qt's item model framework, while fairly feature-rich and adaptable on the View
// side of things, imposes some awkward design constraints on the data side.
// 1) Data sources are expected to provide before and after events for structural
// changes, 2) all the information about such changes must be encapuslated in
// the *before* event, and 3) an event may only communicate to the framework
// about sections of the data represented by contiguous rows and columns.

// As I see it, this biases data models to propagate change events in small
// chunks that are easier to forecast before they are committed, and are more
// suitable to be consumed by the view. This is both an entanglement issue and a
// (potentially catastrophic) efficiency one.

// This class is an attempt to adapt between the needs of a Qt item model, and 
// a more arbitrary and transactional approach to changing data.

// After a change has been made to the data, that change is reported to this
// helper. The helper is now "nostalgic", and will emulate* the state of the data
// as it was before the change. The helper then indicates to the item model where
// it needs to be changed, chunk by chunk. As the item model updates itself, the
// helper will emulate a sequence of transitional data states, until the item
// model is fully updated and the helper is no longer nostalgic.

// The helper groups changes together into contiguous sections, to minimize the
// number of calls to `QAbstractItemModel::beginInsertRows()`, etc. Insertions
// are processed root to leaf, top to bottom, while removals are processed in
// reverse order.

// * Only information about the locations and relationships of relevant nodes is
// actually emulated. This should not impose much cost, particularly not long-term.

// (As is, this class requires the data to be stored as HeirarchyList<T>::Node,
// but with a little work, it could be made to work with any tree-like structure
// of smiilar capability.)
namespace Addle {

template<typename Node>
class NodeModelHelper
{
    typedef typename Node::Comparator Comparator;
    typedef typename Node::Removed NodeRemoved;

public:
    enum Operation //prepend Op_ ? avoid confusion with Removed and Moved typenames
    {
        None,
        Insert,
        Remove,
        Move
    };

    NodeModelHelper() = default;
    NodeModelHelper(const NodeModelHelper&) = delete;
    NodeModelHelper& operator=(const NodeModelHelper&) = delete;

    void clear();

    bool isNostalgic() const 
    { 
        return !_inserts_byParent.empty()
            || !_removes_byParent.empty()
            || !_removes_byNode.isEmpty();
    }
    Operation operation() const { return _operation; }

    int sizeOf(const Node* parent) const;
    int indexOf(const Node* parent) const;
    const Node* nodeAt(const Node* parent, int row) const;

    void nodesInserted(QList<Node*> inserted);
    bool insertArgs(const Node** parent, int* first, int* last);
    void nextInsert();

    void nodesRemoved(QList<typename Node::Removed> removed);
    bool removeArgs(const Node** parent, int* first, int* last);
    void nextRemove();
    
private:
    void updateInsert();
    void updateRemove();

    Operation _operation = None;

    typedef std::map<
        const Node*,
        std::set<const Node*, Comparator>,
        Comparator
    > inserts_map_t;
    inserts_map_t _inserts_byParent;

    typedef std::map<
        const Node*,
        std::set<NodeRemoved>,
        Comparator
    > removes_map_t;
    removes_map_t _removes_byParent;

    QHash<const Node*, NodeRemoved> _removes_byNode;

    const Node* _nextInsertParent = nullptr;
    int _nextInsertFirst = 0;
    int _nextInsertLast = 0;

    const Node* _nextRemoveParent = nullptr;
    int _nextRemoveFirst = 0;
    int _nextRemoveLast = 0;
};

template<typename Node>
void NodeModelHelper<Node>::clear()
{
    _operation = None;

    _inserts_byParent.clear();
    _removes_byParent.clear();
    _removes_byNode.clear();

    _nextInsertParent = nullptr;
    _nextInsertFirst = 0;
    _nextInsertLast = 0;
}

template<typename Node>
int NodeModelHelper<Node>::sizeOf(const Node* parent) const
{
    int offset = 0;

    if (_operation != None && !isNostalgic())
    {
        {
            auto i = _inserts_byParent.find(parent);
            if (i != _inserts_byParent.end())
            {
                offset -= i->second.size();
            }
        }

        {
            auto i = _removes_byParent.find(parent);
            if (i != _removes_byParent.end())
            {
                offset += i->second.size();
            }
        }
    }

    return parent->size() + offset;
}

template<typename Node>
int NodeModelHelper<Node>::indexOf(const Node* node) const
{
    if (!isNostalgic()) return node->index();

    // TODO: amortize with hash maps

    switch(_operation)
    {
    case None:
        return node->index();
    case Insert:
        {
            if (_inserts_byParent.find(node->parent()) == _inserts_byParent.end())
            {
                // None of the node's siblings were inserted, its index
                // is therefore unaffected.
                return node->index();
            }
            else
            {
                auto& peers = _inserts_byParent.at(node->parent());
                if (peers.find(node) != peers.end())
                {
                    // The node was inserted (and is ignored because of nostalgia),
                    // therefore this is an error case.
                    // TODO: throw        
                    return -1;
                }

                int offset = 0;

                for (const Node* peer : peers)
                {
                    if (peer->index() < node->index())
                        --offset;
                    else
                        break;
                }

                return node->index() + offset;
            }
        }
    case Remove:
        {
            if (_removes_byParent.find(node->parent()) == _removes_byParent.end())
            {
                if (_removes_byNode.contains(node))
                    // One of the node's ancestors was removed (but it is still
                    // accounted for by nostalgia). It will be processed at the
                    // same time as its siblings so its original index is
                    // satisfactory.
                    return _removes_byNode[node].index();
                else
                    // None of the node's siblings have been removed, its index
                    // is therefore unaffected.
                    return node->index();
            }
            else
            {
                auto& peers = _removes_byParent.at(node->parent());
                if (_removes_byNode.contains(node))
                { 
                    NodeRemoved nodeRemoved = _removes_byNode[node];
                    if (peers.find(nodeRemoved) != peers.end())
                    {
                        // The node was removed (but is still accounted for by
                        // nostalgia), we will take its reported index at face value.
                        return nodeRemoved.index();
                    }
                }

                int unremoved = 0;
                int lastRemoved = -1;
                for (NodeRemoved removedPeer : peers)
                {
                    if (removedPeer.index() > lastRemoved + 1)
                    {
                        int gap = removedPeer.index() - (lastRemoved + 1);
                        
                        // node->index() - unremoved is the position of this
                        // node within its gap, and lastRemoved + 1 is the
                        // starting position of the gap within the emulated
                        // sibling list.

                        if (unremoved + gap > node->index())
                        {
                            // The node is in a gap between removed nodes.
                            return node->index() - unremoved + lastRemoved + 1;
                        }
                        unremoved += gap;
                    }
                    lastRemoved = removedPeer.index();
                }

                return node->index() - unremoved + lastRemoved + 1;
            }
        }
    case Move:
    default:
        {
            // TODO
            return -1;
        }
    }
}
template<typename Node>
const Node* NodeModelHelper<Node>::nodeAt(const Node* parent, int row) const
{
    if (!isNostalgic())
        return &parent->at(row);

    if (row < 0 || row > sizeOf(parent))
        return nullptr; // TODO throw
    
    // TODO amortize

    switch(_operation)
    {
    case None:
        return &parent->at(row);
    case Insert: 
        {
            // see the Removed section of indexOf for a run-down of this logic.
            // It's the same thing with roles reversed.

            int lastAdded = -1;
            int unadded = 0;
            for (const Node* peer : _inserts_byParent.at(parent))
            {
                if (peer->index() > lastAdded + 1)
                {
                    int gap = peer->index() - (lastAdded + 1);

                    if (unadded + gap > row)
                    {
                        return &parent->at(unadded + row);
                    }

                    unadded += gap;
                }
                lastAdded = peer->index();
            }

            return &parent->at(unadded + row);
        }
    case Remove:
        {
            if (_removes_byParent.find(parent) == _removes_byParent.end())
            {
                // Either none of parent's children were removed, or parent itself
                // and therefore all its children were removed. Either way, the
                // relative positions are unaffected.
                return &parent->at(row);
            }
            else
            {
                auto& peers = _removes_byParent.at(parent);

                int offset = 0;

                for (NodeRemoved removedPeer : peers)
                {
                    if (removedPeer.index() == row)
                        return removedPeer.node().data();
                    else if (removedPeer.index() < row)
                        --offset;
                    else
                        break;
                }

                return &parent->at(row + offset);
            }
        }
    case Move:
        {
            // TODO
            return nullptr;
        }
    }
}

template<typename Node>
void NodeModelHelper<Node>::nodesInserted(QList<Node*> inserted)
{
    // assert op is None or Insert

    _operation = Insert;

    for (const Node* node : noDetach(inserted))
    {
        _inserts_byParent[node->parent()].insert(node);
    }

    updateInsert();
}

template<typename Node>
bool NodeModelHelper<Node>::insertArgs(const Node** parent, int* first, int* last)
{
    if (parent) *parent = _nextInsertParent;
    if (first) *first = _nextInsertFirst;
    if (last) *last = _nextInsertLast;

    return isNostalgic();
}

template<typename Node>
void NodeModelHelper<Node>::nextInsert()
{
    auto i = _inserts_byParent.find(_nextInsertParent);
    if (i != _inserts_byParent.end())
    {
        auto& children = i->second;
        auto j = children.begin();

        while (j != children.end() && (*j)->index() <= _nextInsertLast)
        {
            j = children.erase(j);
        }

        if (children.empty())
            _inserts_byParent.erase(i);
    }

    updateInsert();
}

template<typename Node>
void NodeModelHelper<Node>::updateInsert()
{
    if (_inserts_byParent.empty())
    {
        _nextInsertParent = nullptr;
        _nextInsertFirst = 0;
        _nextInsertLast = 0;
        return;
    }

    _nextInsertParent = _inserts_byParent.begin()->first;
    auto& children = _inserts_byParent.begin()->second;
    //assert !children.empty()
    
    auto i = children.begin();
    _nextInsertFirst = (*i)->index();
    _nextInsertLast = _nextInsertFirst;
    while (++i != children.end())
    {
        int index = (*i)->index();
        if (index > _nextInsertLast + 1)
            break;
        else
            _nextInsertLast = index;
    }
}

template<typename Node>
void NodeModelHelper<Node>::nodesRemoved(QList<typename Node::Removed> removed)
{
    _operation = Remove;

    for (NodeRemoved nodeRemoved : noDetach(removed))
    {
        _removes_byNode[nodeRemoved.node().data()] = nodeRemoved;
        
        const Node* parentNode = nodeRemoved.parentIsNode() ?
            boost::get<const Node*>(nodeRemoved.parent()) :
            boost::get<NodeRemoved>(nodeRemoved.parent()).node().data();

        _removes_byParent[parentNode].insert(nodeRemoved);
    }

    updateRemove();
}

template<typename Node>
bool NodeModelHelper<Node>::removeArgs(const Node** parent, int* first, int* last)
{
    if (parent) *parent = _nextRemoveParent;
    if (first) *first = _nextRemoveFirst;
    if (last) *last = _nextRemoveLast;

    return isNostalgic();
}

template<typename Node>
void NodeModelHelper<Node>::nextRemove()
{
    auto i = _removes_byParent.find(_nextRemoveParent);
    if (i != _removes_byParent.end())
    {
        auto& children = i->second;
        auto j = children.rbegin();
        auto&& end = children.rend();

        while (j != end && (*j)->index() >= _nextRemoveFirst)
        {
            auto node = j->node();
            _removes_byNode.remove(node.data());

            if (node->isGroup())
            {
                for (const Node& descendant : *node)
                {
                    _removes_byNode.remove(&descendant);
                }
            }

            std::advance(j, 1);
            children.erase(j.base());
        }

        if (children.empty())
            _removes_byParent.erase(i);
    }

    updateRemove();
}

template<typename Node>
void NodeModelHelper<Node>::updateRemove()
{
    if (_removes_byParent.empty())
    {
        _nextRemoveParent = nullptr;
        _nextRemoveFirst = 0;
        _nextRemoveLast = 0;
        return;
    }

    _nextRemoveParent = _removes_byParent.rbegin()->first;
    auto& children = _removes_byParent.rbegin()->second;
    //assert !children.empty()
    
    auto i = children.rbegin();
    _nextRemoveFirst = (*i).index();
    _nextRemoveLast = _nextRemoveFirst;
    while (++i != children.rend())
    {
        int index = (*i).index();
        if (index > _nextRemoveFirst + 1)
            break;
        else
            _nextRemoveFirst = index;
    }
}

} // namespace Addle

#endif // NODEMODELHELPER_HPP
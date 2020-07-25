#ifndef NODEMODELHELPER_HPP
#define NODEMODELHELPER_HPP

#include <map>
#include <set>
#include <QList>

// Provides the tangentially related services of "nostalgic" model access and
// batch-processing node insertions and removals.

// The helper is said to be "nostalgic" if the item model has indicated that its
// data has changed (e.g., by calling `nodesAdded`) but that those changes have
// not been fully processed. The helper will backwards-extrapolate some properties 
// (such as indices and sizes) of the item model before the changes were made. Then,
// as the changes are processed, it will "update itself" until it reaches the
// present state (and is no longer nostalgic).

// This allows the corresponding presenters to be free to modify their data
// without necessarily needing to predict where the data will be inserted, and
// moreover spares the API a number of "_aboutToChange" signals.

// The helper groups changes together into contiguous sections, to minimize the
// number of calls to `QAbstractItemModel::beginInsertRows()`, etc.

template<typename Node>
class NodeModelHelper
{
    typedef typename Node::Comparator Comparator;
public:
    NodeModelHelper() = default;
    NodeModelHelper(const NodeModelHelper&) = delete;
    NodeModelHelper& operator=(const NodeModelHelper&) = delete;

    void clear();

    bool isNostalgic() const { return _nostalgic; }

    int sizeOf(const Node* parent);
    int indexOf(const Node* parent);

    void nodesInserted(QList<Node*> inserted);
    bool getInsertArgs(const Node** parent, int* first, int* last);
    void nextInsert();

    void nodesRemoved(QList<Node*> removed);
    bool getRemoveArgs(const Node** parent, int* first, int* last);
    void nextRemove();
    
    //const Node* nodeAt(const Node* parent, int row);

private:
    void updateInsert();

    bool _nostalgic = false;

    std::map<
        const Node*,
        std::set<const Node*, Comparator>,
        Comparator
    > _insertions_byParent;

    std::map<
        const Node*,
        std::set<const Node*, Comparator>,
        Comparator
    > _removals_byParent;

    const Node* _nextInsertParent = nullptr;
    int _nextInsertFirst = 0;
    int _nextInsertLast = 0;
};

template<typename Node>
void NodeModelHelper<Node>::clear()
{
    _nostalgic = false;

    _insertions_byParent.clear();

    _nextInsertParent = nullptr;
    _nextInsertFirst = 0;
    _nextInsertLast = 0;
}

template<typename Node>
void NodeModelHelper<Node>::nodesInserted(QList<Node*> inserted)
{
    for (const Node* node : qAsConst(inserted))
    {
        _insertions_byParent[node->parent()].insert(node);
    }

    updateInsert();
}

template<typename Node>
int NodeModelHelper<Node>::sizeOf(const Node* parent)
{
    int offset = 0;

    {
        auto i = _insertions_byParent.find(parent);
        if (i != _insertions_byParent.end())
        {
            offset -= i->second.size();
        }
    }

    int size = parent->size();
    return size + offset;
}

template<typename Node>
int NodeModelHelper<Node>::indexOf(const Node* node)
{
    int offset = 0;

    {
        auto i = _insertions_byParent.find(node->parent());
        if (i != _insertions_byParent.end())
        {
            for (const Node* sibling : i->second)
            {
                if (sibling->index() < node->index())
                    --offset;
                else
                    break;
            }
        }
    }

    return node->index() + offset;
}

// template<typename Node>
// const Node* NodeModelHelper<Node>::nodeAt(const Node* parent, int row)
// {
//     int offset = 0;

//     {
//         auto i = _insertions_byParent.find(parent);
//         if (i != _insertions_byParent.end())
//         {
//             for (const Node* sibling : i->second)
//             {
//                 if (sibling->index() <= row)
//                     ++offset;
//                 else
//                     break;
//             }
//         }
//     }

//     return &parent->at(row + offset);
// }

template<typename Node>
bool NodeModelHelper<Node>::getInsertArgs(const Node** parent, int* first, int* last)
{
    if (parent) *parent = _nextInsertParent;
    if (first) *first = _nextInsertFirst;
    if (last) *last = _nextInsertLast;

    return !_insertions_byParent.empty();
}

template<typename Node>
void NodeModelHelper<Node>::nextInsert()
{
    auto i = _insertions_byParent.find(_nextInsertParent);
    if (i != _insertions_byParent.end())
    {
        auto& children = i->second;
        auto j = children.begin();

        while (j != children.end() && (*j)->index() <= _nextInsertLast)
        {
            j = children.erase(j);
        }

        if (children.empty())
            _insertions_byParent.erase(i);
    }

    updateInsert();
}

template<typename Node>
void NodeModelHelper<Node>::updateInsert()
{
    auto i = _insertions_byParent.begin();
    if (i != _insertions_byParent.end())
    {
        _nostalgic = true;

        _nextInsertParent = i->first;
        auto& children = i->second;
        //assert !children.empty()
        
        auto j = children.begin();
        _nextInsertFirst = (*j)->index();
        _nextInsertLast = _nextInsertFirst;
        while (++j != children.end())
        {
            int index = (*j)->index();
            if (index > _nextInsertLast + 1)
                break;
            else
                _nextInsertLast = index;
        }
    }
    else 
    {
        _nostalgic = false;

        _nextInsertParent = nullptr;
        _nextInsertFirst = 0;
        _nextInsertLast = 0;
    }
}

// template<typename Node>
// bool NodeModelHelper<Node>::getInsertArgs(const Node** parent, int* first, int* last)
// {
//     if (parent) *parent = _nextInsertParent;
//     if (first) *first = _nextInsertFirst;
//     if (last) *last = _nextInsertLast;

//     return !_insertions_byParent.empty();
// }

// template<typename Node>
// void NodeModelHelper<Node>::nextInsert()
// {
//     auto i = _insertions_byParent.find(_nextInsertParent);
//     if (i != _insertions_byParent.end())
//     {
//         auto& children = i->second;
//         auto j = children.begin();

//         while (j != children.end() && (*j)->index() <= _nextInsertLast)
//         {
//             j = children.erase(j);
//         }

//         if (children.empty())
//             _insertions_byParent.erase(i);
//     }

//     updateInsert();
// }

// template<typename Node>
// void NodeModelHelper<Node>::updateInsert()
// {
//     auto i = _insertions_byParent.begin();
//     if (i != _insertions_byParent.end())
//     {
//         _nostalgic = true;

//         _nextInsertParent = i->first;
//         auto& children = i->second;
//         //assert !children.empty()
        
//         auto j = children.begin();
//         _nextInsertFirst = (*j)->index();
//         _nextInsertLast = _nextInsertFirst;
//         while (++j != children.end())
//         {
//             int index = (*j)->index();
//             if (index > _nextInsertLast + 1)
//                 break;
//             else
//                 _nextInsertLast = index;
//         }
//     }
//     else 
//     {
//         _nostalgic = false;

//         _nextInsertParent = nullptr;
//         _nextInsertFirst = 0;
//         _nextInsertLast = 0;
//     }
// }


#endif // NODEMODELHELPER_HPP
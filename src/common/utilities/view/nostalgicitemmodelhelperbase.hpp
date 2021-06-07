#pragma once

#include <algorithm>
#include <deque>

#include <QHash>
#include <QModelIndex>
#include <QAbstractItemModel>

// #include "utilities/datatree/observer.hpp"

namespace Addle {
    
// In Qt's Item Model framework, every structural change to a data source must
// be announced before the modification is actually made, and changes must be
// made in discrete contiguous sections. This does not lend itself to
// transactional data usage, and is overall rather an awkward constraint.
//
// This helper base class serves as an abstraction layer between a data tree and
// a QAbstractItemModel subclass. The helper uses a nostalgic visitor to
// transition the Item Model's structure through node events after they occur.
//
// Also keeps a temporary index of addresses to map to QModelIndex during
// nostalgic operations
template<class ItemModel, class Tree>
class NostalgicItemModelHelperBase
{
//     using NodesAddedEvent = DataTreeNodesAddedEvent<Tree>;
//     using NodesRemovedEvent = DataTreeNodesRemovedEvent<Tree>;
    using handle_t = aux_datatree::const_node_handle_t<Tree>;

protected:
    NostalgicItemModelHelperBase() = default;
    
    inline bool isNostalgic() const { return false; /*_visitor.isNostalgic();*/ }
    
    void setTree(const Tree& tree)
    {
        static_cast<ItemModel*>(this)->beginResetModel();
        
        _root = datatree_root(tree);
        
        _indices.clear();
        _addresses.clear();
        
//         _visitor.clear();
        
        static_cast<ItemModel*>(this)->endResetModel();
    }
    
    void unsetTree()
    {
        static_cast<ItemModel*>(this)->beginResetModel();
        
        _root = {};
        
        _indices.clear();
        _addresses.clear();
        
//         _visitor.clear();
        
        static_cast<ItemModel*>(this)->endResetModel();
    }
    
    
    QModelIndex index_impl(int row, const QModelIndex& parent) const
    {
        // ###
        Q_UNUSED(row);
        Q_UNUSED(parent);
        Q_UNREACHABLE();
//         if (isNostalgic())
//         {
//             if (!parent.isValid())
//                 return static_cast<const ItemModel*>(this)->createIndex(row, 0, (quintptr)0);
//             
//             DataTreeNodeAddress target = noDetach(_addresses)[parent.internalId()] << row;
//             
//             if (_indices.contains(target))
//                 return static_cast<const ItemModel*>(this)->createIndex(row, 0, noDetach(_indices)[target]);
//             
//             std::size_t index = _addresses.size();
//             _addresses.push_back(target);
//             _indices[target] = index;
//             return static_cast<const ItemModel*>(this)->createIndex(row, 0, index);
//         }
//         else
//         {
//             // TODO figure out a way to do this for handle_t not convertible to
//             // a raw pointer. The accessor to the tree can be assumed so as long
//             // as nodes have some kind of unique id that fits in quintptr we
//             // should be fine.
//             
//             if (!parent.isValid())
//             {
//                 return static_cast<const ItemModel*>(this)
//                     ->createIndex(row, 0, const_cast<void*>(
//                         static_cast<const void*>(
//                             datatree_node_child_at(_root, row)
//                         )
//                     ));
//             }
//             
//             handle_t parentNode = static_cast<handle_t>(parent.internalPointer());
//             return static_cast<const ItemModel*>(this)
//                 ->createIndex(row, 0, const_cast<void*>(
//                     static_cast<const void*>(
//                         datatree_node_child_at(parentNode, row)
//                     )
//                 ));
//         }
    }
    
    QModelIndex parent_impl(const QModelIndex& index) const
    {
        if (isNostalgic())
        {
            return QModelIndex(); // TODO
        }
        else
        {
            handle_t node = static_cast<handle_t>(index.internalPointer());
            handle_t parent = datatree_node_parent(node);
            
            if (parent == _root)
                return QModelIndex();
            
            std::size_t row = datatree_node_index(parent);
            return static_cast<const ItemModel*>(this)
                ->createIndex(row, 0, const_cast<void*>(
                    static_cast<const void*>(parent)
                ));
        }
    }
    
    std::size_t rowCount_impl(const QModelIndex& parent) const
    {
        if (isNostalgic())
        {
            return 0; // TODO
        }
        else
        {
            if (parent.isValid())
            {
                return datatree_node_child_count(
                        static_cast<handle_t>(parent.internalPointer())
                    );
            }
            else
            {
                return datatree_node_child_count(_root);
            }       
        }
    }
    
    handle_t getNode(const QModelIndex& index) const
    {
        if (isNostalgic())
        {
            return handle_t {};
        }
        else
        {
            if (index.isValid())
            {
                return static_cast<handle_t>(index.internalPointer());
            }
            else
            {
                return _root;
            }
        }
    }
    
//     void onNodesAdded_impl(NodesAddedEvent added)
//     {
//         // assert !_visitor.isNostalgic()
//         
//         _visitor.pushEvent(added);
//         
// //         while (_visitor.isNostalgic())
// //         {
// //             
// //         }
//     }
    
//     void onNodesRemoved_impl(NodesRemovedEvent removed)
//     {
//         
//     }
    
private:
    
    handle_t _root = {};
    
    mutable QList<DataTreeNodeAddress> _addresses;
    mutable QHash<DataTreeNodeAddress, quintptr> _indices;
    
//     DataTreeNostalgicNodeEventVisitor<Tree> _visitor;
};

} // namespace Addle

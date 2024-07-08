#ifndef BTREE_POC_BTREE_B_PLUS_TREE_HPP_
#define BTREE_POC_BTREE_B_PLUS_TREE_HPP_

#include "INode.hpp"
#include "InternalNode.hpp"
#include "LeafNode.hpp"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <stack>
#include <utility>

namespace btreepoc::btree
{
    // Represents a B+Tree.
    template<typename TKey, typename TValue>
    class BPlusTree
    {
    public:
        typedef std::uint32_t SizeType;
        typedef std::shared_ptr<INode> NodePointer;
        typedef LeafNode<TKey, TValue> LeafNodeType;
        typedef InternalNode<TKey> InternalNodeType;
        typedef std::shared_ptr<LeafNodeType> LeafNodePointer;
        typedef std::shared_ptr<InternalNodeType> InternalNodePointer;

    public:
        // Constructor.
        inline BPlusTree(SizeType order)
        : order_(order),
          root_(std::make_shared<LeafNodeType>(order_))
        {
        }

#ifdef BTREE_POC_UNIT_TESTS
        // Special constructor for unit tests (TESTS ONLY).
        inline BPlusTree(SizeType order, NodePointer root)
        : order_(order),
          root_(root)
        {
        }

        // Compare the keys of this b+tree and another one. This method is available only
        // for unit tests.
        //
        // Returns true if they are equivalent, false otherwise.
        bool CompareKeys(const BPlusTree<TKey, TValue>& other)
        {
            return root_->CompareKeys(*other.root_.get());
        }
#endif

        // Insert a new key value pair in the tree. If the key is already present the
        // insertion fail.
        //
        // Returns true if the insertion succeed, false otherwise.
        inline bool Insert(const TKey& key, const TValue& value)
        {
            return InsertRecursive(root_, key, value, std::stack<InternalNodePointer>());
        }

    private:
        SizeType order_;
        NodePointer root_;

        inline bool InsertRecursive(NodePointer node,
                                    const TKey& key,
                                    const TValue& value,
                                    std::stack<InternalNodePointer> parents)
        {
            bool result;

            if(node->is_leaf())
            {
                auto leaf = std::static_pointer_cast<LeafNodeType>(node);
                result    = leaf->Insert({ key, value });
                if(result)
                {
                    Balance(leaf, parents);
                }
            }
            else
            {
                auto internal = std::static_pointer_cast<InternalNodeType>(node);
                auto child    = internal->FindChildContainsKey(key);
                parents.push(internal);
                result = InsertRecursive(child, key, value, parents);
            }

            return result;
        }

        // Calculate the index of the siblings that will be part of the balancing. Usually
        // we try to use the left and right siblings of the node being balanced. However
        // if the node being balanced is the first or last child of its parent we might
        // have to pick two siblings on the same side. It is also possible that the parent
        // have less than tree children (only possible at root or a child of root), in
        // this situation all children are used.
        inline std::pair<SizeType, SizeType>
          CalculateIndexSiblingsBalance(SizeType index_in_parent, SizeType parent_size)
        {
            std::int32_t first_index = static_cast<std::int32_t>(index_in_parent) -
                                       (index_in_parent == parent_size ? 2 : 1);
            first_index = std::max(first_index, 0);

            SizeType last_index =
              std::min(static_cast<SizeType>(first_index + 2), parent_size);

            return { first_index, last_index };
        }

        // Create a copy of all the cell in the nodes we will need to balance.
        inline std::vector<typename LeafNodeType::CellType>
          CopyCellsBalance(InternalNodePointer parent,
                           std::int32_t first_index,
                           std::int32_t last_index)
        {
            std::vector<typename LeafNodeType::CellType> cells;
            for(auto sibling_index = first_index; sibling_index <= last_index;
                ++sibling_index)
            {
                LeafNodePointer sibling;
                if(sibling_index < static_cast<std::int32_t>(parent->size()))
                {
                    sibling = std::static_pointer_cast<LeafNodeType>(
                      (*parent)[sibling_index].value);
                }
                else
                {
                    sibling = std::static_pointer_cast<LeafNodeType>(
                      parent->right_most_pointer());
                }
                std::copy(sibling->begin(), sibling->end(), std::back_inserter(cells));
            }
            return cells;
        }

        // Remove from a parent the children that will be rebalanced. If one of the
        // children is the righ-most-child it is not remove, because it is not possible to
        // remove the right-most-child of an internal node and it will eventually be
        // replaced by the new right-most-child by the balancing algorithm.
        inline void RemoveChildFromParentBeforeBalance(InternalNodePointer parent,
                                                       std::int32_t first_index,
                                                       std::int32_t last_index)
        {
            for(auto sibling_index =
                  std::min<std::int32_t>(last_index, parent->size() - 1);
                first_index <= sibling_index;
                --sibling_index)
            {
                parent->RemoveAt(sibling_index);
            }
        }

        // Calculate how many node is needed to store a given amount of cells.
        inline SizeType CalculateNumberNodesAfterBalance(SizeType cells_count)
        {
            return (cells_count + order_ - 2) / (order_ - 1);
        }

        // Calculate the number of cells at a node after balancing. We try to have the
        // same amout of cells in each node. When it is not possible the left most nodes
        // might have one more cells than right most nodes.
        inline SizeType CalculateCellsAtNodeBalance(SizeType cells_count,
                                                    SizeType nodes_count,
                                                    SizeType node_index)
        {
            return (cells_count + nodes_count - node_index - 1) / nodes_count;
        }

        // When the root is overflowing it can't be split into more nodes. We need to
        // create a new root with the current root as it's only child, effectivelly
        // growing the tree by one level. The node will then need to be splitted by the
        // balancing algorithm
        //
        // Note: In our real implementation we might not be able to replace the      root
        // node, because it will need to remain in the same       position in the file. We
        // will need to handle this, probablly       by creating a new InternalNode using
        // the same Page.
        inline InternalNodePointer GrowNewRoot(InternalNodeType::ChildPointer node)
        {
            auto new_root = std::make_shared<InternalNodeType>(order_);
            new_root->set_right_most_pointer(node);
            root_ = new_root;
            return new_root;
        }

        // This methods balance the tree after an insertion or a deletion. It is higly
        // inspired by Antonio Sarosi's implementation of mkdb.
        //
        // https://github.com/antoniosarosi/mkdb/blob/master/src/storage/btree.rs
        //
        inline void Balance(LeafNodePointer node, std::stack<InternalNodePointer> parents)
        {
            bool is_root = parents.empty();

            if(is_root)
            {
                if(node->overflow())
                {
                    auto new_root = GrowNewRoot(node);
                    parents.push(new_root);
                }
                else
                {
                    // We don't need to check if the root is underflow or not because the
                    // root is allowed to underflow.
                    return;
                }
            }

            // Find in the parent the index of the cell pointing back at `node`.
            auto parent = std::static_pointer_cast<InternalNodeType>(parents.top());
            std::int32_t index_in_parent = parent->FindChildIndex(node);

            // Find the siblings that will be part of the balancing.
            auto [first_sibling_index, last_sibling_index] =
              CalculateIndexSiblingsBalance(index_in_parent, parent->size());
            bool must_restore_right_most = last_sibling_index == parent->size();

            auto cells =
              CopyCellsBalance(parent, first_sibling_index, last_sibling_index);
            RemoveChildFromParentBeforeBalance(parent,
                                               first_sibling_index,
                                               last_sibling_index);
            SizeType num_nodes = CalculateNumberNodesAfterBalance(cells.size());

            // Create new nodes add cells in them and insert them into the parent.
            auto next_cell = cells.begin();
            for(auto node_index = 0u; node_index < num_nodes; ++node_index)
            {
                auto cells_at_node =
                  CalculateCellsAtNodeBalance(cells.size(), num_nodes, node_index);
                auto new_node = std::make_shared<LeafNodeType>(order_);
                for(auto x = 0u; x < cells_at_node; ++x)
                {
                    new_node->Insert(*(next_cell++));
                }

                // Insert the node in the parents. If must_restore_right_most is true the
                // last node must replaece the righ-most-pointer instead of being
                // inserted.
                if(node_index < num_nodes - 1 || !must_restore_right_most)
                {
                    parent->Insert({ new_node->back().key, new_node });
                }
                else
                {
                    parent->set_right_most_pointer(new_node);
                }
            }
        }
    };
} // namespace btreepoc::btree

#endif // BTREE_POC_BTREE_B_PLUS_TREE_HPP_
#ifndef BTREE_POC_BTREE_B_PLUS_TREE_HPP_
#define BTREE_POC_BTREE_B_PLUS_TREE_HPP_

#include "INode.hpp"
#include "InternalNode.hpp"
#include "LeafNode.hpp"

#include <cstdint>
#include <memory>
#include <stack>
#include <stdexcept>

namespace btreepoc::btree
{
    // Represents a B+Tree.
    template<typename TKey, typename TValue>
    class BPlusTree
    {
    public:
        typedef std::uint32_t OrderType;
        typedef std::shared_ptr<INode> NodePointer;
        typedef LeafNode<TKey, TValue> LeafNodeType;
        typedef InternalNode<TKey> InternalNodeType;
        typedef std::shared_ptr<LeafNodeType> LeafNodePointer;
        typedef std::shared_ptr<InternalNodeType> InternalNodePointer;

    public:
        // Constructor.
        inline BPlusTree(OrderType order)
        : order_(order),
          root_(std::make_shared<LeafNodeType>(order_))
        {
        }

#ifdef BTREE_POC_UNIT_TESTS
        // Special constructor for unit tests (TESTS ONLY).
        inline BPlusTree(OrderType order, NodePointer root)
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
            if(root_->is_leaf())
            {
                auto leaf   = std::static_pointer_cast<LeafNodeType>(root_);
                bool result = leaf->Insert({ key, value });
                if(result)
                {
                    Balance(leaf, std::stack<InternalNodePointer>());
                }
                return result;
            }
            else
            {
                // TODO : Implement
                throw new std::runtime_error("Not implemented yet");
            }
        }

    private:
        OrderType order_;
        NodePointer root_;

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
                    // The root is overflowing. The root can't be split into more node.
                    // What we will do is create a new empty root with the current root as
                    // it's only child. Then we will procede to balance this child wich
                    // will be splitted.
                    //
                    // Note: In our real implementation we might not be able to replace
                    // the
                    //       root node, because it will need to remain in the same
                    //       position in the file. We will need to handle this, probablly
                    //       by creating a new InternalNode using the same Page.
                    auto new_root = std::make_shared<InternalNodeType>(order_);
                    new_root->set_right_most_pointer(node);
                    root_ = new_root;
                    parents.push(new_root);
                }
                else
                {
                    return; // if the root underflow it's ok, no need to balance.
                }
            }

            // Find in the parent the index of the cell pointing back at `node`.
            // auto parent = std::static_pointer_cast<InternalNodeType>(parents.top());

            // We take a copy of all the cells that will need to be redistributed. We also
            // remove the reference to to node from their parent. remove them from their
            // nodes.
            auto c = node->end() - node->begin();
            std::vector<typename LeafNodeType::CellType> cells;
            cells.reserve(node->size());
            std::copy(node->begin(), node->end(), std::back_inserter(cells));

            // We calculate how many node will be needed to store all the cells.
            OrderType num_nodes = (cells.size() + order_ - 1) / order_;

            // Create new nodes add cells in them and insert them into the parent.
            auto next_cell = cells.begin();
            for(auto node_index = 0u; node_index < num_nodes; ++node_index)
            {
                // Distribute cells.size() elements into num_nodes nodes as evenly as
                // possible. Buckets with lower indices (left-most) get one extra elements
                // if needed.
                auto cells_at_node =
                  (cells.size() + num_nodes - node_index - 1) / num_nodes;
                auto new_node = std::make_shared<LeafNodeType>(order_);
                for(auto x = 0u; x < cells_at_node; ++x)
                {
                    new_node->Insert(*(next_cell++));
                }

                // Insert the node in the parents. The last node must be the
                // right-most-child.
                if(node_index < num_nodes - 1)
                {
                    parents.top()->Insert({ new_node->back().key, new_node });
                }
                else
                {
                    parents.top()->set_right_most_pointer(new_node);
                }
            }
        }
    };
} // namespace btreepoc::btree

#endif // BTREE_POC_BTREE_B_PLUS_TREE_HPP_
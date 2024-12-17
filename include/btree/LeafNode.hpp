#ifndef BTREE_POC_BTREE_LEAF_NODE_HPP_
#define BTREE_POC_BTREE_LEAF_NODE_HPP_

#include "INode.hpp"
#include "NodeBase.hpp"

namespace btreepoc::btree
{
    // This class represents a leaf node in a B+tree
    template<typename TKey, typename TValue>
    class LeafNode : public INode, public NodeBase<TKey, TValue>
    {
    public:
        typedef NodeBase<TKey, TValue> Base;

        // Constructor.
        inline LeafNode(Base::SizeType order)
        : Base(order)
        {
        }

        // Indicate if this node is a leaf node.
        inline bool is_leaf() const { return true; };

        // Indicate if the node is underflowing (has less cells than allowed for a node in
        // a balanced tree)
        inline bool underflow() const { return Base::underflow(); }

        // Indicate if the node is overflowing (has more cells than allowed for a node in
        // a balanced tree)
        inline bool overflow() const { return Base::overflow(); }

#ifdef BTREE_POC_UNIT_TESTS
        // Compare the keys of this node and an other node. This method is available only
        // for unit tests.
        //
        // Returns true if they are equivalent, false otherwise.
        bool CompareKeys(const INode& other)
        {
            auto other_leaf = dynamic_cast<const LeafNode<TKey, TValue>*>(&other);
            if(!other_leaf)
            {
                return false;
            }

            return Base::CompareKeys(*static_cast<const Base*>(other_leaf));
        };
#endif
    };
} // namespace btreepoc::btree

#endif // BTREE_POC_BTREE_LEAF_NODE_HPP_
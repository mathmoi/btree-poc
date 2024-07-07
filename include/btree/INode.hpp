#ifndef BTREE_POC_BTREE_INODE_HPP_
#define BTREE_POC_BTREE_INODE_HPP_

namespace btreepoc::btree
{
    // Interface for B+tree nodes.
    class INode
    {
    public:
        // Indicate if this node is a leaf node.
        virtual bool is_leaf() const = 0;

        // Indicate if the node is underflowing (has less cells than allowed for a node in
        // a balanced tree)
        virtual bool underflow() const = 0;

        // Indicate if the node is overflowing (has more cells than allowed for a node in
        // a balanced tree)
        virtual bool overflow() const = 0;

        virtual ~INode() = default;

#ifdef BTREE_POC_UNIT_TESTS
        // Compare the keys of this node and an other node. This method is available only
        // for unit tests.
        //
        // Returns true if they are equivalent, false otherwise.
        virtual bool CompareKeys(const INode& other) = 0;
#endif
    };

} // namespace btreepoc::btree

#endif // BTREE_POC_BTREE_INODE_HPP_
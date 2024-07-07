#ifndef BTREE_POC_BTREE_INTERNAL_NODE_HPP_
#define BTREE_POC_BTREE_INTERNAL_NODE_HPP_

#include "INode.hpp"
#include "NodeBase.hpp"

#include <memory>

namespace btreepoc::btree
{
    // This class represents internal nodes in a b+tree. Internal nodes contains keys and
    // pointer to children nodes that can be other internal node or leaf nodes.
    template<typename TKey>
    class InternalNode : public INode, public NodeBase<TKey, std::shared_ptr<INode>>
    {
    public:
        typedef std::shared_ptr<INode> ChildPointer;
        typedef NodeBase<TKey, ChildPointer> Base;

        // Constructor.
        inline InternalNode(Base::SizeType order)
        : Base(order)
        {
        }

        // Indicate if this node is a leaf node.
        inline bool is_leaf() const { return false; };

        // Indicate if the node is underflowing (has less cells than allowed for a node in
        // a balanced tree)
        inline bool underflow() const { return Base::underflow(); }

        // Indicate if the node is overflowing (has more cells than allowed for a node in
        // a balanced tree)
        inline bool overflow() const { return Base::overflow(); }

        // Returns the right-most-pointer.
        inline ChildPointer right_most_pointer() const { return right_most_pointer_; }

        // Set the right-most-pointer.
        inline void set_right_most_pointer(ChildPointer pointer)
        {
            right_most_pointer_ = pointer;
        }

#ifdef BTREE_POC_UNIT_TESTS
        // Compare the keys of this node and an other node. This method is available only
        // for unit tests.
        //
        // Returns true if they are equivalent, false otherwise.
        bool CompareKeys(const INode& other)
        {
            auto other_internal = dynamic_cast<const InternalNode<TKey>*>(&other);
            if(!other_internal)
            {
                return false;
            }

            if(this->size() != other_internal->size())
            {
                return false;
            }

            auto it_this  = this->begin();
            auto it_other = other_internal->begin();
            while(it_this < this->end())
            {
                if(!it_this->value->CompareKeys(*it_other->value))
                {
                    return false;
                }
                it_this++;
                it_other++;
            }

            if(!right_most_pointer_->CompareKeys(*other_internal->right_most_pointer_))
            {
                return false;
            }

            return Base::CompareKeys(*static_cast<const Base*>(other_internal));
        };
#endif

    private:
        ChildPointer right_most_pointer_;
    }; // namespace btreepoc::btree
} // namespace btreepoc::btree

#endif // BTREE_POC_BTREE_INTERNAL_NODE_HPP_
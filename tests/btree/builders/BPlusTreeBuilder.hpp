#ifndef BTREE_POC_TESTS_BTREE_BUILDERS_B_PLUS_TREE_BUILDER_HPP_
#define BTREE_POC_TESTS_BTREE_BUILDERS_B_PLUS_TREE_BUILDER_HPP_

#include <btree/BPlusTree.hpp>

#include <cassert>
#include <initializer_list>
#include <memory>
#include <stack>

namespace btreepoc::btree::tests::builders
{
    template<typename TKey, typename TValue>
    class BPlusTreeBuilder
    {
    public:
        typedef BPlusTreeBuilder<TKey, TValue> SelfType;
        typedef BPlusTree<TKey, TValue> BTreeType;

        BPlusTreeBuilder() { Init(); }

        inline SelfType& WithOrder(BTreeType::SizeType order)
        {
            order_ = order;
            return *this;
        }

        inline SelfType& AddInternalNode()
        {
            AddNode(std::make_shared<typename BTreeType::InternalNodeType>(order_));
            return *this;
        }

        inline SelfType& AddLeafNode()
        {
            AddNode(std::make_shared<typename BTreeType::LeafNodeType>(order_));
            return *this;
        }

        inline SelfType& AddInternalNode(TKey key)
        {
            AddNode(key, std::make_shared<typename BTreeType::InternalNodeType>(order_));
            return *this;
        }

        inline SelfType& AddLeafNode(TKey key)
        {
            AddNode(key, std::make_shared<typename BTreeType::LeafNodeType>(order_));
            return *this;
        }

        inline SelfType& AddKeys(std::initializer_list<TKey> keys)
        {
            assert(current_node_->is_leaf());

            auto leaf_node =
              std::static_pointer_cast<typename BTreeType::LeafNodeType>(current_node_);

            for(auto key : keys)
            {
                leaf_node->Insert({ key, key * 2 });
            }

            return *this;
        }

        inline SelfType& EndNode()
        {
            if(!stack_.empty())
            {
                current_node_ = stack_.top();
                stack_.pop();
            }
            return *this;
        }

        inline BTreeType Build()
        {
            BTreeType result(
              order_,
              current_node_ ? current_node_
                            : std::make_shared<typename BTreeType::LeafNodeType>(order_));
            Init();
            return result;
        }

    private:
        BTreeType::SizeType order_;
        std::stack<typename BTreeType::NodePointer> stack_;
        BTreeType::NodePointer current_node_;

        void Init()
        {
            order_ = 3;
            while(!stack_.empty())
            {
                stack_.pop();
            }
            current_node_.reset();
        }

        inline void AddNode(BTreeType::NodePointer new_node)
        {
            if(current_node_)
            {
                // If there is a current node it must be a InternalNode without a right
                // most child. The new node will become the right most child of the
                // current node.
                assert(!current_node_->is_leaf());
                auto parent =
                  std::static_pointer_cast<typename BTreeType::InternalNodeType>(
                    current_node_);
                assert(!parent->right_most_pointer());

                stack_.push(current_node_);
                current_node_ = new_node;
                parent->set_right_most_pointer(current_node_);
            }
            else
            {
                current_node_ = new_node;
            }
        }

        inline void AddNode(TKey max_key, BTreeType::NodePointer new_node)
        {
            assert(current_node_);
            assert(!current_node_->is_leaf());

            auto parent = std::static_pointer_cast<typename BTreeType::InternalNodeType>(
              current_node_);
            parent->Insert({ max_key, new_node });
            stack_.push(current_node_);
            current_node_ = new_node;
        }
    };
} // namespace btreepoc::btree::tests::builders

#endif // BTREE_POC_TESTS_BTREE_BUILDERS_B_PLUS_TREE_BUILDER_HPP_
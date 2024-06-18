#ifndef BTREE_INTERNAL_NODE_HPP_
#define BTREE_INTERNAL_NODE_HPP_

#include "Node.hpp"

#include <array>
#include <cassert>
#include <memory>

namespace btree
{
    template<typename TKey, const int order>
    class InternalNode : public Node<TKey, order>
    {
    public:
        typedef std::shared_ptr<Node<TKey, order>> ChildPointer;
        typedef std::array<std::pair<TKey, ChildPointer>, Node<TKey, order>::MAX_KEYS>
          ItemsArray;

        InternalNode()
        : Node<TKey, order>(false)
        {
        }

        TKey GetKeyAt(std::uint32_t index)
        {
            assert(index < this->items_count_);
            return items_[index].first;
        }

        ChildPointer GetChildAt(std::uint32_t index) const
        {
            assert(index <= this->items_count_);

            // If the index is zero we need to return the left most pointer.
            if(0 == index)
            {
                return left_most_child_;
            }

            return items_[index - 1].second;
        }

        // TODO : Introduce a type for a key/child pair (maybe InternalNode::Cell)
        // returns the key that split de nodes and the new node.
        std::pair<TKey, std::shared_ptr<InternalNode<TKey, order>>> Split()
        {
            auto new_node  = std::make_shared<InternalNode<TKey, order>>();
            auto mid_index = this->items_count_ / 2;
            TKey mid_key   = items_[mid_index].first;

            std::copy(items_.begin() + mid_index + 1,
                      items_.end(),
                      new_node->items_.begin());
            new_node->left_most_child_ = items_[mid_index].second;
            new_node->items_count_     = this->items_count_ - mid_index - 1;
            this->items_count_         = mid_index;

            return std::make_pair(mid_key, new_node);
        }

        void Insert(ChildPointer left_child, TKey key, ChildPointer right_child)
        {
            assert(left_child);
            assert(right_child);
            assert(this->items_count_ == 0);

            left_most_child_   = left_child;
            items_[0]          = std::make_pair(key, right_child);
            this->items_count_ = 1;
        }

        // The new_child must be the upper part of a split of an existing children of the
        // current node. This way, the lower part is already a child of the current node
        // and we only need to find the new node after the existing lower part. This means
        // that the new child, can never be placed in the left_most_child even when it's
        // the left_most_child that was split.
        void Insert(TKey new_key, ChildPointer new_child)
        {
            assert(new_child);
            assert(this->has_space());

            auto [index, found] = this->SearchKeyIndex(new_key);
            assert(!found); // We do not support duplicate keys.

            std::copy_backward(items_.begin() + index,
                               items_.begin() + this->items_count_,
                               items_.begin() + this->items_count_ + 1);
            ++this->items_count_;

            items_[index] = std::make_pair(new_key, new_child);
        }

        ChildPointer FindChild(const TKey& key)
        {
            auto [index, found] = this->SearchKeyIndex(key);

            // If we found the exact key we need to return the child at the next index
            index += found ? 1 : 0;

            return GetChildAt(index);
        }

        void Print(std::ostream& out) const
        {
            out << "node" << this->id() << " [label=\"";
            for(std::uint32_t x = 0; x < this->items_count_; ++x)
            {
                out << "<f" << x * 2 << "> |<f" << x * 2 + 1 << "> " << items_[x].first
                    << " |";
            }
            out << "<f" << this->items_count_ * 2 << "> \"];\n";

            for(std::uint32_t x = 0; x < this->items_count_ + 1; ++x)
            {
                out << "node" << this->id() << ":f" << x * 2 << " -> node"
                    << GetChildAt(x)->id() << ";\n";
            }
        }

    private:
        /*void SetChildAt(std::uint32_t index, ChildPointer child)
        {
            assert(index <= this->items_count_);

            // If the index is past the list of items, we need to set the right  most
        child. if (this->items_count_ == index)
            {
                left_most_child_ = child;
            }
            else
            {
                items_[index].second = child;
            }
        }*/

        ItemsArray items_;
        ChildPointer left_most_child_;
    };
} // namespace btree

#endif // BTREE_INTERNAL_NODE_HPP_
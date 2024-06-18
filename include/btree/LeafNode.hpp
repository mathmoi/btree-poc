#ifndef BTREE_LEAF_NODE_HPP_
#define BTREE_LEAF_NODE_HPP_

#include "Node.hpp"

#include <memory>

namespace btree
{
    template<typename TKey, typename TValue, const int order>
    class LeafNode : public Node<TKey, order>
    {
    public:
        typedef std::array<std::pair<TKey, TValue>, Node<TKey, order>::MAX_KEYS>
          ItemsArray;

        LeafNode()
        : Node<TKey, order>(true),
          next_(nullptr)
        {
        }

        void Insert(const TKey& key, const TValue& value)
        {
            auto [index, found] = this->SearchKeyIndex(key);
            if(!found)
            {
                std::copy_backward(items_.begin() + index,
                                   items_.begin() + this->items_count_,
                                   items_.begin() + this->items_count_ + 1);
                ++this->items_count_;
            }
            items_[index] = std::make_pair(key, value);
        }

        TKey GetKeyAt(std::uint32_t index)
        {
            assert(index < this->items_count_);
            return items_[index].first;
        }

        std::pair<TKey, std::shared_ptr<LeafNode<TKey, TValue, order>>> Split()
        {
            std::shared_ptr<LeafNode<TKey, TValue, order>> new_leaf =
              std::make_shared<LeafNode<TKey, TValue, order>>();
            auto mid_index = this->items_count_ / 2;
            TKey mid_key   = items_[mid_index].first;

            std::copy(items_.begin() + mid_index, items_.end(), new_leaf->items_.begin());
            new_leaf->items_count_ = this->items_count_ - mid_index;
            this->items_count_     = mid_index;

            new_leaf->next_ = next_;
            next_           = new_leaf.get();

            return std::make_pair(mid_key, new_leaf);
        }

        void Print(std::ostream& out) const
        {
            out << "node" << this->id() << " [label=\"";
            for(std::uint32_t x = 0; x < this->items_count_; ++x)
            {
                out << "<f" << x << "> " << items_[x].first << ':' << items_[x].second
                    << ' ';
                if(x < this->items_count_ - 1)
                {
                    out << '|';
                }
            }
            out << "\"];\n";
        }

    private:
        ItemsArray items_;
        LeafNode<TKey, TValue, order>* next_;
    };
} // namespace btree

#endif // BTREE_LEAF_NODE_HPP_
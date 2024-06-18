#ifndef BTREE_NODE_HPP_
#define BTREE_NODE_HPP_

#include <cstdint>
#include <ostream>

namespace btree
{
    template<typename TKey, const int order>
    class Node
    {
    public:
        static const int MAX_KEYS = order * 2;

        Node(bool is_leaf)
        : items_count_(0),
          id_(next_id++),
          is_leaf_(is_leaf) {};

        virtual ~Node() {};

        bool is_leaf() const { return is_leaf_; };

        std::uint32_t items_count() const { return items_count_; }

        bool has_space() const { return items_count_ < order * 2; }

        std::uint32_t id() const { return id_; }

        virtual TKey GetKeyAt(std::uint32_t index) = 0;

        virtual void Print(std::ostream& out) const = 0;

        // Returns a pair containing the new middle key and a new node containing the
        // original upper part of this node.
        // virtual std::shared_ptr<Node<TKey, order>> Split() = 0;

    protected:
        // Search a key index. If the key is not found, the index returned is the index
        // where the key would be if it was. If there is duplicate, returns the first
        // index.
        std::pair<std::uint32_t, bool> SearchKeyIndex(const TKey& key)
        {
            std::pair<std::uint32_t, bool> result(0, false);

            std::int32_t left  = 0;
            std::int32_t right = this->items_count_ - 1;
            while(left <= right)
            {
                auto mid = left + (right - left) / 2;
                if(GetKeyAt(mid) == key)
                {
                    result.first  = mid;
                    result.second = true;
                    right         = mid - 1;
                }
                else if(GetKeyAt(mid) < key)
                {
                    left = mid + 1;
                }
                else
                {
                    right = mid - 1;
                }
            }

            if(!result.second)
            {
                result.first = left;
            }

            return result;
        }

    protected:
        std::uint32_t items_count_;

    private:
        static std::uint32_t next_id;

        std::uint32_t id_;
        bool is_leaf_;
    };

    template<typename TKey, const int order>
    std::uint32_t Node<TKey, order>::next_id = 0;

} // namespace btree

#endif // BTREE_NODE_HPP_
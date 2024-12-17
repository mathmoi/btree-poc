#ifndef BTREE_POC_BTREE_NODE_BASE_HPP_
#define BTREE_POC_BTREE_NODE_BASE_HPP_

#include "Cell.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

namespace btreepoc::btree
{
    // Base class containings the common functionalities for B+tree internal and leaf
    // nodes.
    template<typename TKey, typename TValue>
    class NodeBase
    {
    public:
        // Type for the size of a node.
        typedef std::uint32_t SizeType;

        // Type for the keys in this node type.
        typedef TKey KeyType;

        // Type of the cells stored in the node
        typedef Cell<TKey, TValue> CellType;

        // Type of an iterator on the cells of a node.
        typedef std::vector<CellType>::const_iterator ConstIterator;

        // Constructor
        inline explicit NodeBase(SizeType order)
        : order_(order)
        {
        }

        // Returns the number of cells
        inline SizeType size() const { return data_.size(); }

        // Indicate if the node is underflowing (has less cells than allowed for a node in
        // a balanced tree)
        inline bool underflow() const { return size() < min_cell_count(); }

        // Indicate if the node is overflowing (has more cells than allowed for a node in
        // a balanced tree)
        inline bool overflow() const { return max_cell_count() < size(); }

        // Returns an iterator to the first cell.
        inline ConstIterator begin() const { return data_.begin(); }

        // Returns an iterator past the last cell.
        inline ConstIterator end() const { return data_.end(); }

        // Return a reference to the last cell
        inline const CellType& back() const { return data_.back(); }

        // Insert a new cell into the node. If a cell with the same key already exists the
        // insertion fails.
        //
        // Returns true if the insertion succed, false otherwise.
        inline bool Insert(const CellType& cell)
        {
            auto find_result = FindIterator(cell.key);
            if(find_result.found)
            {
                // The key is already present, we cannot insert.
                return false;
            }

            data_.insert(find_result.it, cell);
            return true;
        }

        // Remove the cell at a given index.
        inline void RemoveAt(SizeType index) { data_.erase(data_.begin() + index); }

        // Returns a reference to a cell in the node.
        inline const CellType& operator[](SizeType pos) const { return data_[pos]; }

#ifdef BTREE_POC_UNIT_TESTS
        // Compare the keys of this node and an other node. This method is available only
        // for unit tests.
        //
        // Returns true if they are equivalent, false otherwise.
        bool CompareKeys(const NodeBase& other)
        {
            if(this == &other)
            {
                return true;
            }

            if(data_.size() != other.data_.size())
            {
                return false;
            }

            auto it_this  = data_.begin();
            auto it_other = other.data_.begin();
            while(it_this < data_.end())
            {
                if(it_this->key != it_other->key)
                {
                    return false;
                }
                it_this++;
                it_other++;
            }

            return true;
        }
#endif

    protected:
        struct FindIteratorResult
        {
            bool found;
            std::vector<CellType>::const_iterator it;
        };

        // Find the index of a key or the position where the key would be inserted if it
        // is not found.
        //
        // The method returns two values : a boolean indicating if the key was found in
        // the node and the position of the key (if found) or the position it would be (if
        // not found).
        inline FindIteratorResult FindIterator(const TKey& key) const
        {
            auto it = std::lower_bound(data_.begin(),
                                       data_.end(),
                                       key,
                                       [](const CellType& left, const TKey& right)
                                       { return left.key < right; });

            bool found = it != data_.end() && it->key == key;
            return { found, it };
        }

    private:
        SizeType order_;
        std::vector<CellType> data_;

        inline SizeType min_cell_count() const { return (order_ + 1) / 2 - 1; }

        inline SizeType max_cell_count() const { return order_ - 1; }
    };

} // namespace btreepoc::btree

#endif // BTREE_POC_BTREE_NODE_BASE_HPP_
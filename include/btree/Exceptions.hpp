#ifndef BTREE_POC_BTREE_EXCEPTIONS_HPP_
#define BTREE_POC_BTREE_EXCEPTIONS_HPP_

#include <stdexcept>

namespace btreepoc::btree
{
    // Base class for all exceptions thrown by the btree module.
    class BTreeException : public std::runtime_error
    {
    public:
        explicit BTreeException(const std::string& message)
        : std::runtime_error(message)
        {
        }
    };
} // namespace btreepoc::btree

#endif // BTREE_POC_BTREE_EXCEPTIONS_HPP_
#pragma once

#include <algorithm>
#include <bit>
#include <cassert>
#include <cstddef>
#include <functional>
#include <type_traits>

#include <unordered_map>
#include <vector>

namespace bs
{

/// @brief A binary heap, which overwrites the `T` value if it is already in the heap.
/// To do this, member function `T::unique_id()` is used to find the same `T` object.
///
/// @tparam T type of value to store
/// @tparam Compare ordering of `T`
/// @tparam UniqueIdHash hasher of `T::unique_id()`
/// @tparam UniqueIdEqual equality check of `T::unique_id()`
template <typename T, typename Compare = std::less<T>,
          typename UniqueIdHash = std::hash<std::invoke_result_t<decltype(&T::unique_id), T>>,
          typename UniqueIdEqual = std::equal_to<std::invoke_result_t<decltype(&T::unique_id), T>>>
class AlterBinaryHeap
{
public:
    static_assert(std::is_member_function_pointer_v<decltype(&T::unique_id)>);

    using UniqueId = std::invoke_result_t<decltype(&T::unique_id), T>;

private:
    struct Node
    {
        T value;
        std::size_t heap_index;

        auto operator<=>(const Node& other) const -> std::weak_ordering
        {
            const Compare comp;

            if (comp(value, other.value))
                return std::weak_ordering::less;
            if (comp(other.value, value))
                return std::weak_ordering::greater;

            return std::weak_ordering::equivalent;
        }
    };

public:
    /// @brief Random Access Iterator
    struct ConstIterator
    {
        friend class AlterBinaryHeap;

    private:
        const std::vector<Node*>& _heap;
        std::size_t _heap_index;

    private:
        auto node() const -> const Node&
        {
            return *_heap[_heap_index];
        }

    public:
        ConstIterator(const std::vector<Node*>& heap, std::size_t heap_index) : _heap(heap), _heap_index(heap_index)
        {
        }

        auto operator*() const -> const T&
        {
            return node()->value;
        }

        auto operator->() const -> const T*
        {
            return &node()->value;
        }

        auto operator[](std::ptrdiff_t index) const -> const T&
        {
            return _heap[_heap_index + index]->value;
        }

        auto operator-(const ConstIterator& other) const -> std::ptrdiff_t
        {
            return _heap_index - other._heap_index;
        }

        bool operator==(const ConstIterator& other) const
        {
            return _heap_index == other._heap_index;
        }

        auto operator<=>(const ConstIterator& other) const
        {
            return _heap_index <=> other._heap_index;
        }

        auto operator+(std::ptrdiff_t diff) const -> ConstIterator
        {
            return ConstIterator(_heap, _heap_index + diff);
        }

        auto operator+=(std::ptrdiff_t diff) -> ConstIterator&
        {
            _heap_index += diff;
            return *this;
        }

        auto operator-(std::ptrdiff_t diff) const -> ConstIterator
        {
            return ConstIterator(_heap, _heap_index - diff);
        }

        auto operator-=(std::ptrdiff_t diff) -> ConstIterator&
        {
            _heap_index -= diff;
            return *this;
        }

        auto operator++() -> ConstIterator&
        {
            ++_heap_index;
            return *this;
        }

        auto operator++(int) -> ConstIterator
        {
            auto it = *this;
            operator++();
            return it;
        }

        auto operator--() -> ConstIterator&
        {
            --_heap_index;
            return *this;
        }

        auto operator--(int) -> ConstIterator
        {
            auto it = *this;
            operator--();
            return it;
        }
    };

public:
    AlterBinaryHeap() = default;

    AlterBinaryHeap(std::size_t reserve_size)
    {
        _map.reserve(reserve_size);
        _heap.reserve(reserve_size);
    }

public: // Element access
    auto top() const -> const T&
    {
        return *_heap.front();
    }

public: // Capacity
    bool empty() const
    {
        return _heap.empty();
    }

    auto size() const -> std::size_t
    {
        return _heap.size();
    }

public: // Modifiers
    void push(const T& value)
    {
        push_impl(value);
    }

    void push(T&& value)
    {
        push_impl(std::move(value));
    }

    void pop()
    {
        elem_swap(0, size() - 1);

        [[maybe_unused]] const bool erased = _map.erase(_heap.back()->value.unique_id());
        assert(erased);
        _heap.pop_back();

        if (!empty())
            bubble_down(0);
    }

public: // Iterators
    auto cbegin() const noexcept -> ConstIterator
    {
        return ConstIterator(_heap, 0);
    }

    auto begin() const noexcept -> ConstIterator
    {
        return cbegin();
    }

    auto cend() const noexcept -> ConstIterator
    {
        return ConstIterator(_heap, size());
    }

    auto end() const noexcept -> ConstIterator
    {
        return cend();
    }

public: // Lookup
    auto find(const UniqueId& id) const -> ConstIterator
    {
        const auto it = _map.find(id);
        if (it != _map.cend())
            return ConstIterator(_heap, it->second.heap_index);

        return cend();
    }

private:
    template <typename TVal>
    void push_impl(TVal&& val)
    {
        const auto uid = val.unique_id();
        auto existing_iter = _map.find(uid);

        if (existing_iter != _map.end())
        {
            existing_iter->second.value = T(std::forward<TVal>(val));
            if (!bubble_up(existing_iter->second.heap_index))
                bubble_down(existing_iter->second.heap_index);
        }
        else
        {
            auto [new_item, inserted] = _map.try_emplace(uid, std::forward<TVal>(val), size());
            assert(inserted);
            _heap.push_back(&new_item->second);

            bubble_up(new_item->second.heap_index);
        }
    }

private:
    /// @return whether bubble-up actually took place or not
    bool bubble_up(std::size_t heap_index)
    {
        assert(heap_index < size());

        bool result = false;

        while (true)
        {
            Node* parent = parent_node(heap_index);
            if (!parent)
                break;

            Node* cur = _heap[heap_index];
            if (*parent < *cur)
            {
                elem_swap(heap_index, parent->heap_index);
                result = true;
                heap_index = cur->heap_index;
                continue;
            }
            break;
        }

        return result;
    }

    /// @return whether bubble-down actually took place or not
    bool bubble_down(std::size_t heap_index)
    {
        assert(heap_index < size());

        bool result = false;

        while (true)
        {
            Node* left = left_child_node(heap_index);
            Node* right = right_child_node(heap_index);
            // 0 child
            if (!left)
            {
                assert(!right);
                break;
            }

            Node* cur = _heap[heap_index];

            // 2 children
            if (right)
            {
                Node* bigger = (*left < *right) ? right : left;

                if (*cur < *bigger)
                {
                    elem_swap(heap_index, bigger->heap_index);
                    result = true;
                    heap_index = cur->heap_index;
                    continue;
                }
            }
            // 1 child
            else
            {
                if (*cur < *left)
                {
                    elem_swap(heap_index, left->heap_index);
                    result = true;
                    heap_index = cur->heap_index;
                    continue;
                }
            }
            break;
        }

        return result;
    }

private:
    void elem_swap(std::size_t left_index, std::size_t right_index)
    {
        assert(left_index < size());
        assert(right_index < size());

        using std::swap;
        swap(_heap[left_index]->heap_index, _heap[right_index]->heap_index);
        swap(_heap[left_index], _heap[right_index]);
    }

private:
    /// @param index zero-based index
    auto parent_node(std::size_t index) -> Node*
    {
        if (index == 0)
            return nullptr;

        const auto parent_idx = parent_index(index);
        assert(parent_idx < size());
        return _heap[parent_idx];
    }

    /// @param index zero-based index
    auto left_child_node(std::size_t index) -> Node*
    {
        const auto child_idx = left_child_index(index);
        if (child_idx < size())
            return _heap[child_idx];
        return nullptr;
    }

    /// @param index zero-based index
    auto right_child_node(std::size_t index) -> Node*
    {
        const auto child_idx = right_child_index(index);
        if (child_idx < size())
            return _heap[child_idx];
        return nullptr;
    }

private:
    /// @param index zero-based index
    static auto parent_index(std::size_t index) -> std::size_t
    {
        return (index + 1) / 2 - 1;
    }

    /// @param index zero-based index
    static auto left_child_index(std::size_t index) -> std::size_t
    {
        return (index + 1) * 2 - 1;
    }

    /// @param index zero-based index
    static auto right_child_index(std::size_t index) -> std::size_t
    {
        return ((index + 1) * 2 + 1) - 1;
    }

public:
    bool validate() const
    {
        return std::ranges::is_heap(_heap, [](const Node* left, const Node* right) { return *left < *right; });
    }

private:
    std::unordered_map<UniqueId, Node, UniqueIdHash, UniqueIdEqual> _map;

    std::vector<Node*> _heap;
};

} // namespace bs

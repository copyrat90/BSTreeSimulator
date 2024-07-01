#pragma once

#include <cassert>
#include <cstddef>
#include <functional>
#include <stdexcept>

#include "TraversalInfo.hpp"

namespace bs
{

template <typename Key, typename Value, typename Compare = std::less<Key>>
class BSTree
{
private:
    struct Node
    {
        Node* parent;
        Node* left;
        Node* right;

        Key key;
        Value value;
    };

    // To avoid constructing `Key`, `Value` for nil node
    struct alignas(alignof(Node)) NilNode
    {
        Node* parent;
    };

public:
    BSTree() : _nil_node{.parent = &get_nil()}, _root(&get_nil())
    {
    }

    ~BSTree()
    {
        clear();
    }

public:
    // Doesn't insert if same key present
    template <typename TKey, typename... TValArgs>
    bool insert(TKey&& key, TValArgs&&... val_args)
    {
        if (is_nil(*_root))
        {
            _root = new Node{
                .parent = &get_nil(),
                .left = &get_nil(),
                .right = &get_nil(),
                .key = std::forward<TKey>(key),
                .value = Value(std::forward<TValArgs>(val_args)...),
            };
            _size += 1;
            return true;
        }

        const bool inserted =
            insert_recurse(*_root, false, std::forward<TKey>(key), std::forward<TValArgs>(val_args)...);
        return inserted;
    }

    // Overwrite if same key present
    template <typename TKey, typename... TValArgs>
    bool insert_or_assign(TKey&& key, TValArgs&&... val_args)
    {
        if (is_nil(*_root))
        {
            _root = new Node{
                .parent = &get_nil(),
                .left = &get_nil(),
                .right = &get_nil(),
                .key = std::forward<TKey>(key),
                .value = Value(std::forward<TValArgs>(val_args)...),
            };
            _size += 1;
            return true;
        }

        const bool inserted =
            insert_recurse(*_root, true, std::forward<TKey>(key), std::forward<TValArgs>(val_args)...);
        return inserted;
    }

    bool erase(const Key& key)
    {
        const bool erased = erase_recurse(*_root, key);
        return erased;
    }

    auto find(const Key& key) -> Value*
    {
        Node& node = find_recurse(*_root, key);
        if (is_nil(node))
            return nullptr;
        return &node.value;
    }

    auto find(const Key& key) const -> const Value*
    {
        const Node& node = find_recurse(*_root, key);
        if (is_nil(node))
            return nullptr;
        return &node.value;
    }

public:
    template <typename Operation>
    void preorder(Operation op)
    {
        preorder_recurse(*_root, op, 0);
    }

    template <typename Operation>
    void preorder(Operation op) const
    {
        preorder_recurse(*_root, op, 0);
    }

    template <typename Operation>
    void inorder(Operation op)
    {
        inorder_recurse(*_root, op, 0);
    }

    template <typename Operation>
    void inorder(Operation op) const
    {
        inorder_recurse(*_root, op, 0);
    }

    template <typename Operation>
    void postorder(Operation op)
    {
        postorder_recurse(*_root, op, 0);
    }

    template <typename Operation>
    void postorder(Operation op) const
    {
        postorder_recurse(*_root, op, 0);
    }

public:
    bool empty() const
    {
        return _size == 0;
    }

    size_t size() const
    {
        return _size;
    }

public:
    void clear()
    {
        clear_recurse(*_root);
        _root = &get_nil();
        _size = 0;
    }

private:
    template <typename TKey, typename... TValArgs>
    bool insert_recurse(Node& cur, const bool assign, TKey&& key, TValArgs&&... val_args)
    {
        if (less(key, cur.key))
        {
            if (!is_nil(*cur.left))
                return insert_recurse(*cur.left, assign, std::forward<TKey>(key), std::forward<TValArgs>(val_args)...);
            else
            {
                cur.left = new Node{
                    .parent = &cur,
                    .left = &get_nil(),
                    .right = &get_nil(),
                    .key = std::forward<TKey>(key),
                    .value = Value(std::forward<TValArgs>(val_args)...),
                };
                _size += 1;
                return true;
            }
        }
        else if (greater(key, cur.key))
        {
            if (!is_nil(*cur.right))
                return insert_recurse(*cur.right, assign, std::forward<TKey>(key), std::forward<TValArgs>(val_args)...);
            else
            {
                cur.right = new Node{
                    .parent = &cur,
                    .left = &get_nil(),
                    .right = &get_nil(),
                    .key = std::forward<TKey>(key),
                    .value = Value(std::forward<TValArgs>(val_args)...),
                };
                _size += 1;
                return true;
            }
        }
        // equal

        if (assign)
            cur.value = Value(std::forward<TValArgs>(val_args)...);
        return false;
    }

    bool erase_recurse(Node& cur, const Key& key)
    {
        if (is_nil(cur))
            return false;

        if (less(key, cur.key))
            return erase_recurse(*cur.left, key);
        if (greater(key, cur.key))
            return erase_recurse(*cur.right, key);
        // equal
        return erase_node(cur);
    }

    bool erase_node(Node& cur)
    {
        if (is_nil(cur))
            return false;

        // 2 children
        if (!is_nil(*cur.left) && !is_nil(*cur.right))
        {
            // find the right-most node in the left subtree
            Node* right_most = cur.left;
            while (!is_nil(*right_most->right))
                right_most = right_most->right;

            // move the key & value to `cur`
            cur.key = std::move(right_most->key);
            cur.value = std::move(right_most->value);

            // remove `right_most`
            if (!erase_node(*right_most))
                throw std::logic_error("`right_most` should exist, at least `cur.left` exists");
            return true;
        }
        // 1 or 0 child
        else
        {
            Node& child = (!is_nil(*cur.left)) ? *cur.left : *cur.right;
            Node& parent = *cur.parent;

            if (is_nil(parent)) // `cur` is root
                _root = &child;
            else if (parent.left == &cur)
                parent.left = &child;
            else
                parent.right = &child;

            if (!is_nil(child))
                child.parent = &parent;

            delete &cur;
        }

        _size -= 1;
        return true;
    }

    auto find_recurse(Node& cur, const Key& key) -> Node&
    {
        if (is_nil(cur))
            return get_nil();

        if (less(key, cur.key))
            return find_recurse(*cur.left, key);
        if (greater(key, cur.key))
            return find_recurse(*cur.right, key);
        // equal
        return cur;
    }

    auto find_recurse(const Node& cur, const Key& key) const -> const Node&
    {
        if (is_nil(cur))
            return get_nil();

        if (less(key, cur.key))
            return find(*cur.left, key);
        if (greater(key, cur.key))
            return find(*cur.right, key);
        // equal
        return cur;
    }

private:
    template <typename Operation>
    void preorder_recurse(Node& cur, Operation op, std::size_t complete_index)
    {
        if (is_nil(cur))
            return;

        op(cur.key, cur.value,
           TraversalInfo{
               .complete_index = complete_index,
               .red = true,
           });
        preorder_recurse(*cur.left, op, complete_index * 2 + 1);
        preorder_recurse(*cur.right, op, complete_index * 2 + 2);
    }

    template <typename Operation>
    void preorder_recurse(Node& cur, Operation op, std::size_t complete_index) const
    {
        if (is_nil(cur))
            return;

        op(cur.key, cur.value,
           TraversalInfo{
               .complete_index = complete_index,
               .red = true,
           });
        preorder_recurse(*cur.left, op, complete_index * 2 + 1);
        preorder_recurse(*cur.right, op, complete_index * 2 + 2);
    }

    template <typename Operation>
    void inorder_recurse(Node& cur, Operation op, std::size_t complete_index)
    {
        if (is_nil(cur))
            return;

        inorder_recurse(*cur.left, op, complete_index * 2 + 1);
        op(cur.key, cur.value,
           TraversalInfo{
               .complete_index = complete_index,
               .red = true,
           });
        inorder_recurse(*cur.right, op, complete_index * 2 + 2);
    }

    template <typename Operation>
    void inorder_recurse(Node& cur, Operation op, std::size_t complete_index) const
    {
        if (is_nil(cur))
            return;

        inorder_recurse(*cur.left, op, complete_index * 2 + 1);
        op(cur.key, cur.value,
           TraversalInfo{
               .complete_index = complete_index,
               .red = true,
           });
        inorder_recurse(*cur.right, op, complete_index * 2 + 2);
    }

    template <typename Operation>
    void postorder_recurse(Node& cur, Operation op, std::size_t complete_index)
    {
        if (is_nil(cur))
            return;

        postorder_recurse(*cur.left, op, complete_index * 2 + 1);
        postorder_recurse(*cur.right, op, complete_index * 2 + 2);
        op(cur.key, cur.value,
           TraversalInfo{
               .complete_index = complete_index,
               .red = true,
           });
    }

    template <typename Operation>
    void postorder_recurse(Node& cur, Operation op, std::size_t complete_index) const
    {
        if (is_nil(cur))
            return;

        postorder_recurse(*cur.left, op, complete_index * 2 + 1);
        postorder_recurse(*cur.right, op, complete_index * 2 + 2);
        op(cur.key, cur.value,
           TraversalInfo{
               .complete_index = complete_index,
               .red = true,
           });
    }

    void clear_recurse(Node& cur)
    {
        if (is_nil(cur))
            return;

        clear_recurse(*cur.left);
        clear_recurse(*cur.right);
        delete &cur;
    }

private:
    bool is_nil(const Node& node) const
    {
        return &node == &get_nil();
    }

    auto get_nil() const -> const Node&
    {
        return reinterpret_cast<const Node&>(_nil_node);
    }

    auto get_nil() -> Node&
    {
        return reinterpret_cast<Node&>(_nil_node);
    }

private:
    static bool less(const Key& k1, const Key& k2)
    {
        return Compare{}(k1, k2);
    }

    static bool greater(const Key& k1, const Key& k2)
    {
        return Compare{}(k2, k1);
    }

    static bool equal(const Key& k1, const Key& k2)
    {
        return !less(k1, k2) && !greater(k1, k2);
    }

private:
    std::size_t _size = 0;

    NilNode _nil_node;
    Node* _root;
};

} // namespace bs

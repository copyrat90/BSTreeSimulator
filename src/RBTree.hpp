#pragma once

#include <cassert>
#include <cstddef>
#include <functional>
#include <stdexcept>

#include "TraversalInfo.hpp"

namespace bs
{

template <typename Key, typename Value, typename Compare = std::less<Key>>
class RBTree
{
private:
    struct Node
    {
        bool red;

        Node* parent;
        Node* left;
        Node* right;

        Key key;
        Value value;
    };

    // To avoid constructing `Key`, `Value` for nil node
    struct alignas(alignof(Node)) NilNode
    {
        bool red = false;

        Node* parent;
    };

public:
    RBTree() : _nil_node{.parent = &get_nil()}, _root(&get_nil())
    {
    }

    ~RBTree()
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
                .red = false,
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
                .red = false,
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
                    .red = true,
                    .parent = &cur,
                    .left = &get_nil(),
                    .right = &get_nil(),
                    .key = std::forward<TKey>(key),
                    .value = Value(std::forward<TValArgs>(val_args)...),
                };
                _size += 1;
                rebalance_insert(*cur.left);
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
                    .red = true,
                    .parent = &cur,
                    .left = &get_nil(),
                    .right = &get_nil(),
                    .key = std::forward<TKey>(key),
                    .value = Value(std::forward<TValArgs>(val_args)...),
                };
                _size += 1;
                rebalance_insert(*cur.right);
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

            // set this even if `child` is nil
            child.parent = &parent;

            if (!cur.red)
                rebalance_erase(child);

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
               .red = cur.red,
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
               .red = cur.red,
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
               .red = cur.red,
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
               .red = cur.red,
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
               .red = cur.red,
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
               .red = cur.red,
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
    void rebalance_insert(Node& cur)
    {
        assert(!is_nil(cur));
        assert(cur.red);

        Node& parent = *cur.parent;
        // If root, recolor to black
        if (&cur == _root)
        {
            assert(is_nil(parent));
            cur.red = false;
            return;
        }
        assert(!is_nil(parent));

        // Do nothing if parent is black
        if (!parent.red)
            return;

        // parent is red
        // grand is black
        Node& grand = *parent.parent;
        assert(!is_nil(grand));
        assert(!grand.red);

        const bool cur_is_left = (&cur == parent.left);
        const bool parent_is_left = (&parent == grand.left);

        Node& uncle = parent_is_left ? *grand.right : *grand.left;

        // 1. parent: red, uncle: red
        if (uncle.red)
        {
            parent.red = false;
            uncle.red = false;
            grand.red = true;
            rebalance_insert(grand);
        }
        // parent: red, uncle: black
        // 2-1. cur is right, parent is left
        else if (!cur_is_left && parent_is_left)
        {
            rotate_left(parent);
            rebalance_insert(parent); // go to 3-1 w/ `parent`
        }
        // 2-2. cur is left, parent is right
        else if (cur_is_left && !parent_is_left)
        {
            rotate_right(parent);
            rebalance_insert(parent);
        }
        // 3-1. cur is left, parent is left
        else if (cur_is_left && parent_is_left)
        {
            rotate_right(grand);
            parent.red = false;
            grand.red = true;
        }
        // 3-2. cur is right, parent is right
        else if (!cur_is_left && !parent_is_left)
        {
            rotate_left(grand);
            parent.red = false;
            grand.red = true;
        }
        else
            throw std::logic_error("Should not reach here");
    }

    /// @param child starts with erased node's child
    void rebalance_erase(Node& child)
    {
        // 0. if root, recolor it to black
        if (&child == _root)
        {
            child.red = false;
            return;
        }

        Node& parent = *child.parent;

        const bool child_is_left = (&child == parent.left);
        Node& sibling = child_is_left ? *parent.right : *parent.left;

        // 1. child: red
        if (child.red)
        {
            child.red = false;
            return;
        }
        // 2. child: black, sibling: red
        if (sibling.red)
        {
            sibling.red = false;
            parent.red = true;

            if (child_is_left)
                rotate_left(parent);
            else
                rotate_right(parent);

            rebalance_erase(child);
        }
        // 3. child: black, sibling: black, sib_left: black, sib_right: black
        else if (!sibling.left->red && !sibling.right->red)
        {
            sibling.red = true;

            rebalance_erase(parent);
        }
        // 4. child: black, sibling: black, sib_left: red, sib_right: black
        else if ((child_is_left && (sibling.left->red && !sibling.right->red)) ||
                 (!child_is_left && (sibling.right->red && !sibling.left->red)))
        {
            sibling.red = true;

            if (child_is_left)
            {
                sibling.left->red = false;
                rotate_right(sibling);
            }
            else
            {
                sibling.right->red = false;
                rotate_left(sibling);
            }

            rebalance_erase(child);
        }
        // 5. child: black, sibling: black, sib_left: ?, sib_right: red
        else if ((child_is_left && sibling.right->red) || (!child_is_left && sibling.left->red))
        {
            std::swap(parent.red, sibling.red);

            if (child_is_left)
            {
                sibling.right->red = false;
                rotate_left(parent);
            }
            else
            {
                sibling.left->red = false;
                rotate_right(parent);
            }
        }
        else
            throw std::logic_error("Should not reach here");
    }

private:
    void rotate_left(Node& cur)
    {
        assert(!is_nil(cur));

        Node& parent = *cur.parent;
        Node& right = *cur.right;
        assert(!is_nil(right));

        cur.right = right.left;
        if (!is_nil(*right.left))
            right.left->parent = &cur;

        cur.parent = &right;
        right.left = &cur;

        right.parent = &parent;
        if (is_nil(parent))
            _root = &right;
        else
        {
            if (parent.left == &cur)
                parent.left = &right;
            else
                parent.right = &right;
        }
    }

    void rotate_right(Node& cur)
    {
        assert(!is_nil(cur));

        Node& parent = *cur.parent;
        Node& left = *cur.left;
        assert(!is_nil(left));

        cur.left = left.right;
        if (!is_nil(*left.right))
            left.right->parent = &cur;

        cur.parent = &left;
        left.right = &cur;

        left.parent = &parent;
        if (is_nil(parent))
            _root = &left;
        else
        {
            if (parent.right == &cur)
                parent.right = &left;
            else
                parent.left = &left;
        }
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

public:
    int black_depth() const
    {
        return black_depth_recurse(*_root, 0);
    }

    bool validate() const
    {
        if (_root->red || get_nil().red)
            return false;
        if (!validate_no_double_red(*_root))
            return false;
        if (black_depth() < 0)
            return false;

        return true;
    }

private:
    bool validate_no_double_red(const Node& cur) const
    {
        if (is_nil(cur))
            return true;

        if (cur.red && (cur.left->red || cur.right->red))
            return false;

        if (!validate_no_double_red(*cur.left))
            return false;
        if (!validate_no_double_red(*cur.right))
            return false;

        return true;
    }

    int black_depth_recurse(const Node& cur, int black_depth) const
    {
        if (is_nil(cur))
            return black_depth;

        black_depth += !cur.red;

        const int left_black_depth = black_depth_recurse(*cur.left, black_depth);
        if (left_black_depth < 0)
            return -1;

        const int right_black_depth = black_depth_recurse(*cur.right, black_depth);
        if (right_black_depth < 0)
            return -1;

        if (left_black_depth != right_black_depth)
            return -1;

        return left_black_depth;
    }

private:
    std::size_t _size = 0;

    NilNode _nil_node;
    Node* _root;
};

} // namespace bs

#pragma once

#include <functional>

namespace bs
{

template <typename Key, typename Value, typename Compare = std::less<Key>>
class BSTree
{
private:
    struct Node
    {
        Key key;
        Value value;

        Node* parent;
        Node* left = nullptr;
        Node* right = nullptr;
    };

public:
    ~BSTree()
    {
        clear();
    }

public:
    // Doesn't insert if same key present
    bool insert(const Key& key, const Value& val)
    {
        if (!_root)
        {
            _root = new Node{.key = key, .value = val, .parent = nullptr};
            _size += 1;
            return true;
        }

        const bool inserted = insert_recurse(*_root, key, val, false);
        return inserted;
    }

    // Overwrite if same key present
    bool insert_or_assign(const Key& key, const Value& val)
    {
        if (!_root)
        {
            _root = new Node{.key = key, .value = val, .parent = nullptr};
            _size += 1;
            return true;
        }

        const bool inserted = insert_recurse(*_root, key, val, true);
        return inserted;
    }

    bool erase(const Key& key)
    {
        const bool erased = erase_recurse(_root, key);
        return erased;
    }

    auto find(const Key& key) -> Value*
    {
        Node* node = find_recurse(_root, key);
        if (!node)
            return nullptr;
        return &node->value;
    }

    auto find(const Key& key) const -> const Value*
    {
        Node* node = find_recurse(_root, key);
        if (!node)
            return nullptr;
        return &node->value;
    }

public:
    template <typename Operation>
    void preorder(Operation op)
    {
        preorder_recurse(_root, op);
    }

    template <typename Operation>
    void preorder(Operation op) const
    {
        preorder_recurse(_root, op);
    }

    template <typename Operation>
    void inorder(Operation op)
    {
        inorder_recurse(_root, op);
    }

    template <typename Operation>
    void inorder(Operation op) const
    {
        inorder_recurse(_root, op);
    }

    template <typename Operation>
    void postorder(Operation op)
    {
        postorder_recurse(_root, op);
    }

    template <typename Operation>
    void postorder(Operation op) const
    {
        postorder_recurse(_root, op);
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
        clear_recurse(_root);
        _size = 0;
    }

private:
    bool insert_recurse(Node& cur, const Key& key, const Value& val, const bool assign)
    {
        if (less(key, cur.key))
        {
            if (cur.left)
                return insert_recurse(*cur.left, key, val, assign);
            else
            {
                cur.left = new Node{.key = key, .value = val, .parent = &cur};
                _size += 1;
                return true;
            }
        }
        else if (greater(key, cur.key))
        {
            if (cur.right)
                return insert_recurse(*cur.right, key, val, assign);
            else
            {
                cur.right = new Node{.key = key, .value = val, .parent = &cur};
                _size += 1;
                return true;
            }
        }
        // equal (overwrite)
        else if (assign)
        {
            cur.value = val;
            return true;
        }
        // equal (no-overwrite)
        return false;
    }

    bool erase_recurse(Node* cur, const Key& key)
    {
        if (!cur)
            return false;

        if (less(key, cur->key))
            return erase_recurse(cur->left, key);
        if (greater(key, cur->key))
            return erase_recurse(cur->right, key);
        // equal

        // 2 children
        if (cur->left && cur->right)
        {
            // find the right-most node in the left subtree
            Node* right_most = cur->left;
            while (right_most->right)
                right_most = right_most->right;

            // move the key & value to `cur`
            cur->key = std::move(right_most->key);
            cur->value = std::move(right_most->value);

            // remove `right_most`
            Node* right_most_parent = right_most->parent;
            delete right_most;
            if (right_most_parent->left == right_most)
                right_most_parent->left = nullptr;
            else
                right_most_parent->right = nullptr;
        }
        // 1 or 0 child
        else
        {
            Node* child = (cur->left) ? cur->left : cur->right;
            Node* parent = cur->parent;
            delete cur;

            if (!parent) // `cur` is root
                _root = child;
            else if (parent->left == cur)
                parent->left = child;
            else
                parent->right = child;
        }

        _size -= 1;
        return true;
    }

    auto find_recurse(Node* cur, const Key& key) -> Node*
    {
        if (!cur)
            return nullptr;

        if (less(key, cur->key))
            return find_recurse(cur->left, key);
        if (greater(key, cur->key))
            return find_recurse(cur->right, key);
        // equal
        return cur;
    }

    auto find_recurse(const Node* cur, const Key& key) const -> const Node*
    {
        if (!cur)
            return nullptr;

        if (less(key, cur->key))
            return find(cur->left, key);
        if (greater(key, cur->key))
            return find(cur->right, key);
        // equal
        return cur;
    }

private:
    template <typename Operation>
    void preorder_recurse(Node* cur, Operation op)
    {
        if (!cur)
            return;

        op(cur->key, cur->value);
        preorder_recurse(cur->left, op);
        preorder_recurse(cur->right, op);
    }

    template <typename Operation>
    void preorder_recurse(Node* cur, Operation op) const
    {
        if (!cur)
            return;

        op(cur->key, cur->value);
        preorder_recurse(cur->left, op);
        preorder_recurse(cur->right, op);
    }

    template <typename Operation>
    void inorder_recurse(Node* cur, Operation op)
    {
        if (!cur)
            return;

        inorder_recurse(cur->left, op);
        op(cur->key, cur->value);
        inorder_recurse(cur->right, op);
    }

    template <typename Operation>
    void inorder_recurse(Node* cur, Operation op) const
    {
        if (!cur)
            return;

        inorder_recurse(cur->left, op);
        op(cur->key, cur->value);
        inorder_recurse(cur->right, op);
    }

    template <typename Operation>
    void postorder_recurse(Node* cur, Operation op)
    {
        if (!cur)
            return;

        postorder_recurse(cur->left, op);
        postorder_recurse(cur->right, op);
        op(cur->key, cur->value);
    }

    template <typename Operation>
    void postorder_recurse(Node* cur, Operation op) const
    {
        if (!cur)
            return;

        postorder_recurse(cur->left, op);
        postorder_recurse(cur->right, op);
        op(cur->key, cur->value);
    }

    void clear_recurse(Node* cur)
    {
        if (!cur)
            return;

        clear_recurse(cur->left);
        clear_recurse(cur->right);
        delete cur;
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
    size_t _size = 0;

    Node* _root = nullptr;
};

} // namespace bs

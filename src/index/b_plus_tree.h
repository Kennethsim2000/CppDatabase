#pragma once

#include <algorithm>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <vector>

#include "table_heap.h"

namespace db
{

    class BPlusTree
    {
    private:
        struct Node
        {
            explicit Node(bool leaf)
                : is_leaf(leaf), parent(nullptr), next(nullptr)
            {
            }

            bool is_leaf;

            // Internal node: an internal node always has one more child than keys
            // keys[i] is the minimum key in children[i + 1].
            //
            // Leaf node:
            // keys[i] corresponds to values[i].
            std::vector<int> keys;

            // For leaf nodes:
            // Each key maps to one or more RIDs.
            std::vector<std::vector<RID>> values;

            // Used only by internal nodes.
            std::vector<Node *> children;

            Node *parent;
            Node *next; // link lift nodes together so we can scan keys in a sorted efficient order
        };

    public:
        explicit BPlusTree(std::size_t max_keys = 4)
            : root_(nullptr), max_keys_(max_keys)
        {
            if (max_keys < 3)
            {
                throw std::invalid_argument(
                    "B+ tree max_keys must be at least 3");
            }
        }

        // Returns true if a new distinct key was inserted.
        // Returns false if the key already exists.
        // For duplicate keys, the RID is still appended to the existing key.
        bool insert(int key, const RID &rid)
        {
            // Case 1: The tree is empty.
            if (root_ == nullptr)
            {
                root_ = create_node(true);
                root_->keys.push_back(key);
                root_->values.push_back(std::vector<RID>{rid});
                return true;
            }

            Node *leaf = find_leaf(key);

            auto it = std::lower_bound(
                leaf->keys.begin(),
                leaf->keys.end(),
                key);

            std::size_t index =
                static_cast<std::size_t>(
                    it - leaf->keys.begin());

            // Case 2: The key already exists.
            if (it != leaf->keys.end() && *it == key)
            {
                leaf->values[index].push_back(rid);
                return false;
            }

            // Case 3: Insert a new distinct key.
            leaf->keys.insert(it, key);

            leaf->values.insert(
                leaf->values.begin() + index,
                std::vector<RID>{rid});

            // Split the leaf if it exceeds its capacity.
            if (leaf->keys.size() > max_keys_)
            {
                split_leaf(leaf);
            }

            return true;
        }

        bool search_all(
            int key,
            std::vector<RID> &results) const
        {
            results.clear();

            Node *leaf = find_leaf(key);

            // The tree is empty.
            if (leaf == nullptr)
            {
                return false;
            }

            auto it = std::lower_bound(
                leaf->keys.begin(),
                leaf->keys.end(),
                key);

            if (it == leaf->keys.end() || *it != key)
            {
                return false;
            }

            std::size_t index =
                static_cast<std::size_t>(it - leaf->keys.begin());

            results = leaf->values[index];

            return !results.empty();
        }

        bool search(int key, RID &result) const
        {
            std::vector<RID> results;

            if (!search_all(key, results))
            {
                return false;
            }

            // Backwards-compatible behavior:
            // return the first RID associated with this key.
            result = results.front();

            return true;
        }

    private:
        // Own all nodes separately so Node* pointers remain stable.
        std::vector<std::unique_ptr<Node>> nodes_;

        Node *root_;
        std::size_t max_keys_;

        Node *create_node(bool is_leaf)
        {
            nodes_.push_back(
                std::make_unique<Node>(is_leaf));

            return nodes_.back().get();
        }

        // finds the correct leaf to insert/search the key in
        Node *find_leaf(int key) const
        {
            Node *current = root_;

            // An empty tree has no leaf to search.
            if (current == nullptr)
            {
                return nullptr;
            }

            while (!current->is_leaf)
            {
                // Separator keys represent the minimum key
                // of each child to their right
                // we use upper bound to find the first separator key larger than it,
                // the index for the key would be actually index, cause index + 1 would be the children
                // node for the separator key, so index(the one before) would actually be
                // where we place key
                auto it = std::upper_bound(
                    current->keys.begin(),
                    current->keys.end(),
                    key);

                std::size_t index = std::distance(
                    current->keys.begin(),
                    it);

                current = current->children[index];
            }

            return current;
        }

        // returns the smallest key in the subtree rooted at the given node,
        int min_key(Node *node) const
        {
            while (!node->is_leaf)
            {
                node = node->children.front();
            }

            return node->keys.front();
        }

        // recalculates the separator keys of an internal node based on its current children.
        void rebuild_keys(Node *node)
        {
            node->keys.clear();

            if (node->is_leaf)
            {
                return;
            }

            // For each child after the first, store its
            // smallest key as the separator.
            for (std::size_t i = 1;
                 i < node->children.size();
                 ++i)
            {
                node->keys.push_back(
                    min_key(node->children[i]));
            }
        }

        void split_leaf(Node *leaf)
        {
            Node *right = create_node(true);

            std::size_t middle = leaf->keys.size() / 2;

            right->keys.assign(
                leaf->keys.begin() + middle,
                leaf->keys.end());

            right->values.assign(
                leaf->values.begin() + middle,
                leaf->values.end());

            leaf->keys.resize(middle);
            leaf->values.resize(middle);

            // Maintain the linked list of leaf nodes.
            right->next = leaf->next;
            leaf->next = right;

            right->parent = leaf->parent;

            insert_into_parent(leaf, right);
        }

        // when a node splits into two nodes, the parent must be updated to know about the new node.
        // handles what happens after a B+ tree node splits. It inserts the newly created right node into the parent of the original node.
        void insert_into_parent(Node *left, Node *right)
        {
            if (left->parent == nullptr)
            {
                // The node being split is the root.
                Node *new_root = create_node(false);

                new_root->children.push_back(left);
                new_root->children.push_back(right);

                left->parent = new_root;
                right->parent = new_root;

                rebuild_keys(new_root);
                root_ = new_root;
                return;
            }

            Node *parent = left->parent;

            auto it = std::find(
                parent->children.begin(),
                parent->children.end(),
                left);

            std::size_t index =
                static_cast<std::size_t>(
                    std::distance(parent->children.begin(), it));

            parent->children.insert(
                parent->children.begin() + index + 1,
                right);

            right->parent = parent;

            rebuild_keys(parent);

            if (parent->keys.size() > max_keys_)
            {
                split_internal(parent);
            }
        }

        // used to split the internal into left and right nodes
        void split_internal(Node *node)
        {
            Node *right = create_node(false);

            std::size_t middle =
                node->children.size() / 2;

            right->children.assign(
                node->children.begin() + middle,
                node->children.end());

            node->children.resize(middle);

            for (Node *child : right->children)
            {
                child->parent = right;
            }

            right->parent = node->parent;

            rebuild_keys(node);
            rebuild_keys(right);

            // Insert the new right sibling into the parent.
            // This can cause another split up the tree.
            insert_into_parent(node, right);
        }
    };

}

// the actual records' RIDs live in the leaves. Internal nodes only direct the search
// toward the correct child. When a leaf splits, the first key of the new right leaf
// becomes a separator in its parent, but the key/RID entry stays in the leaf.
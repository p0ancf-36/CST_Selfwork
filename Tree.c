#include <stddef.h>
#include <stdlib.h>

#include "Tree.h"

#include "IdIndex.h"
#include "Item.h"

void *get_data(TreeNode *node) { return 1 + node; }

TreeNode *create_id_index_tree_node(const size_t i, const Item *item) {
    TreeNode *result = aligned_alloc(__alignof(TreeNode), sizeof(TreeNode) + sizeof(IdIndex));
    *(IdIndex *) get_data(result) = create_id_index(i, item);

    result->height = 1;
    result->left = result->right = nullptr;

    return result;
}

TreeNode *create_name_index_tree_node(const size_t i, const Item *item) {
    TreeNode *result = aligned_alloc(__alignof(TreeNode), sizeof(TreeNode) + sizeof(NameIndex));
    *(NameIndex *) get_data(result) = create_name_index(i, item);

    result->height = 1;
    result->left = result->right = nullptr;

    return result;
}

void destroy_tree(TreeNode *tree) {
    if (tree == nullptr)
        return;

    destroy_tree(tree->left);
    destroy_tree(tree->right);

    free(tree);
}

uint8_t height(const TreeNode *node) {
    return node != nullptr ? node->height : 0;
}

uint8_t bfactor(const TreeNode *node) {
    return height(node->right) - height(node->left);
}

void fix_height(TreeNode *node) {
    const uint8_t left_height = height(node->left);
    const uint8_t right_height = height(node->right);
    node->height = (left_height > right_height ? left_height : right_height) + 1;
}


TreeNode * rotate_right(TreeNode *p) {
    TreeNode* q = p->left;
    p->left = q->right;
    q->right = p;
    fix_height(p);
    fix_height(q);
    return q;
}

TreeNode * rotate_left(TreeNode *q) {
    TreeNode* p = q->right;
    q->right = p->left;
    p->left = q;
    fix_height(q);
    fix_height(p);
    return p;
}

TreeNode* balance(TreeNode* p)
{
    fix_height(p);
    if(bfactor(p) == 2)
    {
        if(bfactor(p->right) < 0)
            p->right = rotate_right(p->right);

        return rotate_left(p);
    }
    if(bfactor(p) == -2)
    {
        if(bfactor(p->left) > 0)
            p->left = rotate_left(p->left);

        return rotate_right(p);
    }
    return p;
}

TreeNode* insert(TreeNode* p, TreeNode *k, const Comparator comparator)
{
    if(p == nullptr) return k;

    if(comparator(get_data(k), get_data(p)) == Less)
        p->left = insert(p->left, k, comparator);
    else
        p->right = insert(p->right, k, comparator);

    return balance(p);
}

TreeNode *find_in_tree(TreeNode *node, void *object, const Comparator comparator) {
    if (node == nullptr)
        return nullptr;

    const CmpRes cmp_res = comparator(get_data(node), object);

    if (cmp_res == Equals)
        return node;

    if (cmp_res == Less)
        return find_in_tree(node->right, object, comparator);

    return find_in_tree(node->left, object, comparator);
}

TreeNode* find_min(TreeNode* p)
{
    return p->left?find_min(p->left):p;
}

TreeNode* remove_min(TreeNode* p)
{
    if(p->left == nullptr)
        return p->right;

    p->left = remove_min(p->left);
    return balance(p);
}

TreeNode* remove_in_tree(TreeNode* node, void *object, const Comparator comparator)
{
    if(node == nullptr) return nullptr;

    const CmpRes cmp_res = comparator(get_data(node), object);

    if (cmp_res == Less)
        node->left = remove_in_tree(node->left, object, comparator);
    else if (cmp_res == Greater)
        node->right = remove_in_tree(node->right, object, comparator);
    else if (cmp_res == Equals) {
        TreeNode* q = node->left;
        TreeNode* r = node->right;
        free(node);

        if(r == nullptr) return q;

        TreeNode* min = find_min(r);
        min->right = remove_min(r);
        min->left = q;

        return balance(min);
    }

    return balance(node);
}

#define GETI (*(size_t *)get_data(node))

void rebase_tree_indexes(TreeNode *node, const size_t index) {
    if (node == nullptr)
        return;

    if (GETI >= index)
        GETI -= 1;

    rebase_tree_indexes(node->left, index);
    rebase_tree_indexes(node->right, index);
}

void tree_print_items(Array *source, TreeNode *tree) {
    if (tree == nullptr)
        return;

    tree_print_items(source, tree->left);
    print_item(GET(source, *(size_t *)get_data(tree)));
    tree_print_items(source, tree->right);
}

#undef GETI

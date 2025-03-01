#include <stddef.h>
#include <stdlib.h>

#include "Tree.h"

#include "IdIndex.h"
#include "Item.h"

TreeNode *create_id_index_tree_node(const size_t i, const Item *item) {
    TreeNode *result = aligned_alloc(__alignof(TreeNode), sizeof(TreeNode) + sizeof(IdIndex));
    *(IdIndex *) TREE_GET(result) = create_id_index(i, item);

    result->height = 1;
    result->left = result->right = nullptr;

    return result;
}

TreeNode *create_name_index_tree_node(const size_t i, const Item *item) {
    TreeNode *result = aligned_alloc(__alignof(TreeNode), sizeof(TreeNode) + sizeof(NameIndex));
    *(NameIndex *) TREE_GET(result) = create_name_index(i, item);

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

int16_t bfactor(const TreeNode *node) {
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

TreeNode* tree_insert(TreeNode* p, TreeNode *k, const Comparator comparator)
{
    if(p == nullptr) return k;

    if(comparator(TREE_GET(k), TREE_GET(p)) == Less)
        p->left = tree_insert(p->left, k, comparator);
    else
        p->right = tree_insert(p->right, k, comparator);

    return balance(p);
}

TreeNode *find_in_tree(const TreeNode *node, const void *object, const Comparator comparator) {
    if (node == nullptr)
        return nullptr;

    const CmpRes cmp_res = comparator(TREE_GET(node), object);

    if (cmp_res == Equals)
        return node;

    if (cmp_res == Less)
        return find_in_tree(node->right, object, comparator);

    return find_in_tree(node->left, object, comparator);
}

TreeNode * find_i_in_tree(const TreeNode *node, const void *object, const Comparator comparator) {
    TreeNode *n = node;

    while (n != nullptr) {
        const CmpRes cmp_res = comparator(TREE_GET(n), object);

        if (cmp_res == Equals)
            break;

        if (cmp_res == Less)
            n = n->right;
        else
            n = n->left;
    }

    return n;
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

    const CmpRes cmp_res = comparator(TREE_GET(node), object);

    if (cmp_res == Greater)
        node->left = remove_in_tree(node->left, object, comparator);
    else if (cmp_res == Less)
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

void rebase_tree_indexes(TreeNode *node, const size_t index) {
    if (node == nullptr)
        return;

    if (*TREE_GET_T(node, size_t) >= index)
        *TREE_GET_T(node, size_t) -= 1;

    rebase_tree_indexes(node->left, index);
    rebase_tree_indexes(node->right, index);
}

void remove_in_tree_by_id(Array *source, TreeNode **id_tree, TreeNode **name_tree, const uint64_t id) {
    IdIndex index = create_search_id_index(id);

    TreeNode *to_remove = find_in_tree(*id_tree, &index, id_index_comparator);
    size_t remove_index = *TREE_GET_T(to_remove, size_t);

    if (to_remove == nullptr)
        return;

    *id_tree = remove_in_tree(*id_tree, &index, id_index_comparator);
    rebase_tree_indexes(*id_tree, remove_index);

    Item *item = GET_T(source, remove_index, Item);

    NameIndex sindex = create_search_name_index(item->name);
    *name_tree = remove_in_tree(*id_tree, &sindex, name_index_comparator);
    remove_on(source, remove_index);
}

void remove_in_tree_by_name(Array *source, TreeNode **id_tree, TreeNode **name_tree, const char *name) {
    NameIndex index = create_search_name_index(name);

    TreeNode *to_remove = find_in_tree(*name_tree, &index, name_index_comparator);
    size_t remove_index = *TREE_GET_T(to_remove, size_t);

    if (to_remove == nullptr)
        return;

    *name_tree = remove_in_tree(*name_tree, &index, name_index_comparator);
    rebase_tree_indexes(*name_tree, remove_index);

    Item *item = GET_T(source, remove_index, Item);

    IdIndex sindex = create_search_id_index(item->id);
    *id_tree = remove_in_tree(*id_tree, &sindex, id_index_comparator);
    remove_on(source, remove_index);
}

void tree_print_items(Array *source, TreeNode *tree) {
    if (tree == nullptr)
        return;

    tree_print_items(source, tree->left);
    print_item(GET(source, *(size_t *)TREE_GET(tree)));
    tree_print_items(source, tree->right);
}

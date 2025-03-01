//
// Created by p0ancf-36 on 21.02.25.
//

#ifndef TREE_H
#define TREE_H
#include <stdint.h>

#include "Array.h"
#include "NameIndex.h"

typedef struct TreeNode TreeNode;

typedef struct TreeNode {
    uint8_t height;
    TreeNode *left, *right;
} TreeNode;

TreeNode *create_id_index_tree_node(size_t i, const Item *item);
TreeNode *create_name_index_tree_node(size_t i, const Item *item);
void destroy_tree(TreeNode *tree);

#define TREE_GET(node) ((void *)(((TreeNode *) node) + 1))
#define TREE_GET_T(node, T) ((T *)(((TreeNode *) node) + 1))

TreeNode *tree_insert(TreeNode* p, TreeNode *k, Comparator comparator);

TreeNode *find_in_tree(const TreeNode *node, const void *object, Comparator comparator);
TreeNode *find_i_in_tree(const TreeNode *node, const void *object, Comparator comparator);

TreeNode *remove_in_tree(TreeNode* node, void *object, Comparator comparator);

void remove_in_tree_by_id(Array *source, TreeNode **id_tree, TreeNode **name_tree, const uint64_t id);
void remove_in_tree_by_name(Array *source, TreeNode **id_tree, TreeNode **name_tree, const char *name);

void tree_print_items(Array *source, TreeNode *tree);

#endif //TREE_H

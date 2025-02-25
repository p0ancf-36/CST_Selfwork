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

void *get_data(TreeNode *node);

uint8_t height(const TreeNode *);
uint8_t bfactor(const TreeNode *);
void fix_height(TreeNode *);

TreeNode *rotate_right(TreeNode *);
TreeNode *rotate_left(TreeNode *);
TreeNode* balance(TreeNode* p);

TreeNode *insert(TreeNode* p, TreeNode *k, Comparator comparator);
TreeNode *find_in_tree(TreeNode *node, void *object, Comparator comparator);

TreeNode *remove_in_tree(TreeNode* node, void *object, Comparator comparator);
void rebase_tree_indexes(TreeNode *node, size_t index);

void tree_print_items(Array *source, TreeNode *tree);

#endif //TREE_H

#include <stdio.h>
#include <stdlib.h>

#include "Item.h"
#include "Array.h"
#include "IdIndex.h"
#include "IndexArray.h"
#include "NameIndex.h"

#include "types.h"

typedef struct TreeNode TreeNode;

typedef struct TreeNode {
    uint8_t height;
    TreeNode *left, *right;
} TreeNode;

typedef struct {
    TreeNode node;
    IdIndex index;
} IdIndexTreeNode;

IdIndexTreeNode *create_id_index_tree_node(size_t i, const Item *item) {
    IdIndexTreeNode *node = aligned_alloc(__alignof(IdIndexTreeNode), sizeof(IdIndexTreeNode));
    node->index = create_id_index(i, item);
    node->node.left = node->node.right = nullptr;
    node->node.height = 1;
    return node;
}

typedef struct {
    TreeNode node;
    NameIndex index;
} NameIndexTreeNode;

NameIndexTreeNode *create_name_index_tree_node(size_t i, const Item *item) {
    NameIndexTreeNode *node = aligned_alloc(__alignof(NameIndexTreeNode), sizeof(NameIndexTreeNode));
    node->index = create_name_index(i, item);
    node->node.left = node->node.right = nullptr;
    node->node.height = 1;
    return node;
}

uint8_t height(TreeNode *);
uint8_t height(TreeNode *node) {
    return node != NULL ? node->height : 0;
}

uint8_t bfactor(TreeNode *);
uint8_t bfactor(TreeNode *node) {
    return height(node->right) - height(node->left);
}

void fixheight(TreeNode *);
void fixheight(TreeNode *node) {
    uint8_t hl = height(node->left);
    uint8_t hr = height(node->right);
    node->height = (hl > hr ? hl : hr) + 1;
}

TreeNode * rotate_right(TreeNode *);
TreeNode * rotate_left(TreeNode *);

TreeNode * rotate_right(TreeNode *p) {
    TreeNode* q = p->left;
    p->left = q->right;
    q->right = p;
    fixheight(p);
    fixheight(q);
    return q;
}

TreeNode * rotate_left(TreeNode *q) {
    TreeNode* p = q->right;
    q->right = p->left;
    p->left = q;
    fixheight(q);
    fixheight(p);
    return p;
}

TreeNode* balance(TreeNode* p)
{
    fixheight(p);
    if( bfactor(p)==2 )
    {
        if( bfactor(p->right) < 0 )
            p->right = rotate_right(p->right);
        return rotate_left(p);
    }
    if( bfactor(p)==-2 )
    {
        if( bfactor(p->left) > 0  )
            p->left = rotate_left(p->left);
        return rotate_right(p);
    }
    return p;
}

void *get_data(TreeNode *p) {
    return p + 1;
}

TreeNode* insert(TreeNode* p, TreeNode *k, Comparator comparator)
{
    if( !p ) return k;
    if( comparator(get_data(k), get_data(p)) == Less )
        p->left = insert(p->left, k, comparator);
    else
        p->right = insert(p->right, k, comparator);
    return balance(p);
}

void print(Array *source, TreeNode *tree) {
    if (tree == NULL)
        return;

    print(source, tree->left);
    print_item(get(source, *(size_t *)get_data(tree)));
    print(source, tree->right);
}

TreeNode *find_tree(TreeNode *node, void *object, Comparator comparator) {
    if (node == nullptr)
        return nullptr;

    CmpRes cmp_res = comparator(get_data(node), object);

    if (cmp_res == Equals)
        return node;

    if (cmp_res == Less)
        return find_tree(node->right, object, comparator);

    return find_tree(node->left, object, comparator);
}

TreeNode* findmin(TreeNode* p) // поиск узла с минимальным ключом в дереве p
{
    return p->left?findmin(p->left):p;
}

TreeNode* removemin(TreeNode* p) // удаление узла с минимальным ключом из дерева p
{
    if( p->left==nullptr )
        return p->right;
    p->left = removemin(p->left);
    return balance(p);
}

TreeNode* remove_tree(TreeNode* node, void *object, Comparator comparator)
{
    if(node == nullptr) return nullptr;

    CmpRes cmp_res = comparator(get_data(node), object);

    if (cmp_res == Less)
        node->left = remove_tree(node->left, object, comparator);
    else if (cmp_res == Greater)
        node->right = remove_tree(node->right, object, comparator);
    else if (cmp_res == Equals) {
        TreeNode* q = node->left;
        TreeNode* r = node->right;
        free(node);

        if(r == nullptr) return q;

        TreeNode* min = findmin(r);
        min->right = removemin(r);
        min->left = q;
        return balance(min);
    }

    return balance(node);
}

#define GETI (*(size_t *)get_data(node))

void rebase_tree_indexes(TreeNode *node, size_t index) {
    if (node == nullptr)
        return;

    if (GETI >= index)
        GETI -= 1;

    rebase_tree_indexes(node->left, index);
    rebase_tree_indexes(node->right, index);
}

#undef GETI

void destroy_tree(TreeNode *tree) {
    if (tree == nullptr)
        return;

    destroy_tree(tree->left);
    destroy_tree(tree->right);

    free(tree);
}

typedef struct {
    Item item;
    size_t prev_id_index, next_id_index;
    size_t prev_name_index, next_name_index;
} ListRecord;

void index_array() {
    FILE *file = fopen("default.txt", "r");

    if (file == NULL) {
        printf("File doesn't exists!\n");
        return;
    }

    int n;
    fscanf(file, "%d", &n);

    Array array = create_array(__alignof(Item), sizeof(Item));
    Array id_index = create_array(__alignof(IdIndex), sizeof(IdIndex));
    Array name_index = create_array(__alignof(NameIndex), sizeof(NameIndex));

    for (int i = 0; i < n; i++) {
        Item item = fscan_item(file);
        IdIndex ii = create_id_index(i, &item);
        NameIndex ni = create_name_index(i, &item);

        append(&array, &item);
        append(&id_index, &ii);
        append(&name_index, &ni);
        swap_sorted(&id_index, i, id_index_comparator);
        swap_sorted(&name_index, i, name_index_comparator);
    }

    print_by_name(&array, &name_index);
    edit_by_id(&array, &id_index, &name_index, 3);
    printf("\n");
    print_by_name(&array, &name_index);

    destroy_array(&id_index);
    destroy_array(&name_index);
    destroy_array(&array);
}

int main(void) {
    FILE *file = fopen("default.txt", "r");

    if (file == NULL) {
        printf("File doesn't exists!\n");
        return -1;
    }

    int n;
    fscanf(file, "%d", &n);

    Array array = create_array(__alignof(ListRecord), sizeof(ListRecord));
    IdIndexTreeNode *id_tree = nullptr;
    NameIndexTreeNode *name_tree = nullptr;

    for (int i = 0; i < n; i++) {
        Item item = fscan_item(file);
        append(&array, &item);

        IdIndexTreeNode *id_node = create_id_index_tree_node(i, &item);
        id_tree = (IdIndexTreeNode *) insert(&id_tree->node, &id_node->node, id_index_comparator);

        NameIndexTreeNode *name_node = create_name_index_tree_node(i, &item);
        name_tree = (NameIndexTreeNode *) insert(&name_tree->node, &name_node->node, name_index_comparator);
    }

    print(&array, &id_tree->node);
    printf("\n");
    print(&array, &name_tree->node);

    IdIndex mock_id_index = { 0, 11 };
    TreeNode *search_result = find_tree(&id_tree->node, &mock_id_index, id_index_comparator);

    if (search_result == nullptr) {
        printf("Not found\n");
    } else {
        size_t remove_index = ((IdIndex *)get_data(search_result))->index;
        remove_on(&array, remove_index);
        id_tree = (IdIndexTreeNode *) remove_tree(&id_tree->node, &mock_id_index, id_index_comparator);
        rebase_tree_indexes((TreeNode *)id_tree, remove_index);
    }

    printf("\n");
    print(&array, &id_tree->node);

    destroy_tree((TreeNode *) id_tree);
    destroy_tree((TreeNode *) name_tree);
    destroy_array(&array);

    return 0;
}
#include <stdio.h>
#include <stdlib.h>

#include "Item.h"
#include "Array.h"
#include "IdIndex.h"
#include "IndexArray.h"
#include "NameIndex.h"
#include "Tree.h"

#include "types.h"

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
    TreeNode *id_tree = nullptr;
    TreeNode *name_tree = nullptr;

    for (int i = 0; i < n; i++) {
        Item item = fscan_item(file);
        append(&array, &item);

        TreeNode *id_node = create_id_index_tree_node(i, &item);
        id_tree = insert(id_tree, id_node, id_index_comparator);

        TreeNode *name_node = create_name_index_tree_node(i, &item);
        name_tree = insert(name_tree, name_node, name_index_comparator);
    }

    print(&array, id_tree);
    printf("\n");
    print(&array, name_tree);

    IdIndex search_id_index = create_search_id_index(11);
    TreeNode *search_result = find_in_tree(id_tree, &search_id_index, id_index_comparator);

    if (search_result == nullptr) {
        printf("Not found\n");
    } else {
        size_t remove_index = ((IdIndex *)get_data(search_result))->index;
        Item *item = get(&array, remove_index);
        NameIndex search_name_index = create_search_name_index(item->name);

        id_tree = remove_in_tree(id_tree, &search_id_index, id_index_comparator);
        rebase_tree_indexes(id_tree, remove_index);

        name_tree = remove_in_tree(name_tree, &search_name_index, name_index_comparator);
        rebase_tree_indexes(name_tree, remove_index);

        remove_on(&array, remove_index);
    }

    printf("\n");
    print(&array, name_tree);

    destroy_tree(id_tree);
    destroy_tree(name_tree);
    destroy_array(&array);

    return 0;
}
#include <stdio.h>
#include <stdlib.h>

#include "Item.h"
#include "Array.h"
#include "IdIndex.h"
#include "IndexArray.h"
#include "List.h"
#include "NameIndex.h"
#include "Tree.h"

#include "types.h"

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

void index_tree() {
    FILE *file = fopen("default.txt", "r");

    if (file == NULL) {
        printf("File doesn't exists!\n");
        return;
    }

    int n;
    fscanf(file, "%d", &n);

    Array array = create_array(__alignof(Item), sizeof(Item));
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

    tree_print_items(&array, id_tree);
    printf("\n");
    tree_print_items(&array, name_tree);

    IdIndex search_id_index = create_search_id_index(11);
    TreeNode *search_result = find_in_tree(id_tree, &search_id_index, id_index_comparator);

    if (search_result == nullptr) {
        printf("Not found\n");
    } else {
        size_t remove_index = ((IdIndex *)get_data(search_result))->index;
        Item *item = GET(&array, remove_index);
        NameIndex search_name_index = create_search_name_index(item->name);

        id_tree = remove_in_tree(id_tree, &search_id_index, id_index_comparator);
        rebase_tree_indexes(id_tree, remove_index);

        name_tree = remove_in_tree(name_tree, &search_name_index, name_index_comparator);
        rebase_tree_indexes(name_tree, remove_index);

        remove_on(&array, remove_index);
    }

    printf("\n");
    tree_print_items(&array, name_tree);

    destroy_tree(id_tree);
    destroy_tree(name_tree);
    destroy_array(&array);

    return;
}

typedef enum { NewItem = 1, PrintAsAdded, PrintByIndex, Search, Remove } ListChoice;
typedef enum { Id = 1, Name } Attribute;

typedef struct {
    Array array;
    List *id_list, *name_list;

    bool is_running;
} ListTaskData;

void print_list_menu() {
    printf("1) new item\n");
    printf("2) print items in added order\n");
    printf("3) print by index attribute\n");
    printf("4) search by index attribute\n");
    printf("5) remove by index attribute\n");
    printf("6) exit\n");
}

void print_item_array(const Array array) {
    for (size_t i = 0; i < array.size; i++) {
        print_item(GET(&array, i));
    }
}

Attribute scan_attribute() {
    Attribute result = 0;

    do {
        printf("Enter attribute type:\n1) id\n2) name\n");
        scanf("%d", &result);

        if (result < 1 || result > 2) {
            printf("Attribute with number `%d` not exists! Try again\n", result);
        }

    } while (result < 1 || result > 2);

    return result;
}

void list_task_new_item(ListTaskData *data) {
    FILE *file = fopen("default.txt", "r");

    if (file == NULL) {
        printf("File doesn't exists!\n");
        return;
    }

    int n;
    fscanf(file, "%d", &n);

    data->array = create_array(__alignof(Item), sizeof(Item));
    data->id_list = nullptr;
    data->name_list = nullptr;

    for (int i = 0; i < n; i++) {
        Item item = fscan_item(file);
        append(&data->array, &item);

        List *id_index = create_id_index_list(i, &item);
        list_append(&data->id_list, id_index, id_index_comparator);

        List *name_index = create_name_index_list(i, &item);
        list_append(&data->name_list, name_index, name_index_comparator);
    }
}

void print_by_list(Array array, List *list) {
    List *curr = list;

    while (curr) {
        size_t index = *LIST_GET_T(curr, size_t);
        print_item(GET(&array, index));
        curr = curr->next;
    }
}

void list_task_remove(ListTaskData *data) {
    Attribute attribute = scan_attribute();
    List *list;
    Item *item;
    size_t remove_index;

    switch (attribute) {
        case Id:
            uint64_t id = SIZE_MAX;
            scanf("%lu", &id);
            list = list_cut_by_id(&data->id_list, id);
            remove_index = *LIST_GET_T(list, size_t);
            if (list == nullptr || remove_index == SIZE_MAX) {
                printf("Item not found\n");
                break;
            }
            decrease_indexes(data->id_list, remove_index);
            free(list);

            item = GET_T(&data->array, remove_index, Item);
            list = list_cut_by_name(&data->name_list, item->name);
            free(list);
            decrease_indexes(data->name_list, remove_index);

            remove_on(&data->array, remove_index);
            break;
        case Name:
            char name[STRING_LENGTH];
            scanf("\n%[^\n]", &name);
            list = list_cut_by_name(&data->name_list, name);
            remove_index = *LIST_GET_T(list, size_t);
            if (list == nullptr || remove_index == SIZE_MAX) {
                printf("Item not found\n");
                break;
            }
            decrease_indexes(data->name_list, remove_index);
            free(list);

            item = GET_T(&data->array, remove_index, Item);
            list = list_cut_by_name(&data->name_list, item->name);
            free(list);
            decrease_indexes(data->name_list, remove_index);

            remove_on(&data->array, index_of(&data->array, item));
            break;
    }
}

int main(void) {
    ListTaskData data;
    data.is_running = true;
    data.array = create_array(__alignof(Item), sizeof(Item));
    data.id_list = nullptr;
    data.name_list = nullptr;

    Attribute attribute;
    List *list;
    Item *item;

    while (data.is_running) {
        print_list_menu();

        int choice = 0;
        scanf("%d", &choice);

        switch (choice) {
            case NewItem: // ввод новых записей
                list_task_new_item(&data);
                break;
            case PrintAsAdded: // просмотр элементов в порядке ввода
                print_item_array(data.array);
                break;
            case PrintByIndex: // просмотр в порядке возрастания атрибута
                attribute = scan_attribute();

                switch (attribute) {
                    case Id:
                        print_by_list(data.array, data.id_list);
                        break;
                    case Name:
                        print_by_list(data.array, data.name_list);
                        break;
                }
                break;
            case Search: // поиск
                attribute = scan_attribute();

                switch (attribute) {
                    case Id:
                        uint64_t id = SIZE_MAX;
                        scanf("%lu", &id);
                        list = list_find_by_id(data.id_list, id);
                    break;
                    case Name:
                        char name[STRING_LENGTH];
                        scanf("\n%[^\n]", &name);
                        list = list_find_by_name(data.name_list, name);
                    break;
                }

                if (list == nullptr || *LIST_GET_T(list, size_t) == SIZE_MAX)
                    printf("Item not found\n");
                else
                    print_item(GET(&data.array, *LIST_GET_T(list, size_t)));

                break;
            case Remove: // удаление
                list_task_remove(&data);
                break;
            case 6:
                data.is_running = false;
                break;
            default:
                printf("There is not option with number `%d`\n", choice);
        }
    }

    return 0;
}
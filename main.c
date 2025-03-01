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

    for (int i = 0; i < array.size; i++) {
        print_item(GET_T(&array, GET_T(&name_index, i, NameIndex)->index, Item));
    }

    while (true) {
        printf("Enter name to delete: ");
        char name[STRING_LENGTH];
        scanf("\n%[^\n]", &name);

        NameIndex id_i = create_search_name_index(name);
        remove_by_name(&array, &id_index, &name_index, name);

        // if (name_index_comparator(search_result, &name_i) == Equals) {
        //     print_item(GET(&array, search_result->index));
        // } else {
        //     printf("Item does not exists\n");
        // }
        // printf("\n");

        for (int i = 0; i < array.size; i++) {
            size_t j = GET_T(&name_index, i, IdIndex)->index;
            Item *item = GET_T(&array, j, Item);
            print_item(item);
        }

        fflush(stdin);
        fflush(stdout);
    }
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
        id_tree = tree_insert(id_tree, id_node, id_index_comparator);

        TreeNode *name_node = create_name_index_tree_node(i, &item);
        name_tree = tree_insert(name_tree, name_node, name_index_comparator);
    }

    tree_print_items(&array, name_tree);
    // while (true) {
    //     printf("Enter name to remove: ");
    //     char name[STRING_LENGTH];
    //     scanf("\n%[^\n]", &name);
    //
    //     remove_in_tree_by_name(&array, &id_tree, &name_tree, name);
    //
    //     tree_print_items(&array, name_tree);
    //
    //     fflush(stdin);
    //     fflush(stdout);
    // }

    while (true) {
        printf("Enter id to remove: ");
        size_t id;
        scanf("%lu", &id);

        remove_in_tree_by_id(&array, &id_tree, &name_tree, id);

        tree_print_items(&array, id_tree);

        fflush(stdin);
        fflush(stdout);
    }
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

void list_task_remove(ListTaskData *data) {
    Attribute attribute = scan_attribute();
    List *list;
    Item *item;
    size_t remove_index;

    switch (attribute) {
        case Id:
            uint64_t id = SIZE_MAX;
            scanf("%lu", &id);
            list_delete_by_id(&data->array, &data->id_list, &data->name_list, id);
            break;
        case Name:
            char name[STRING_LENGTH];
            scanf("\n%[^\n]", &name);
            list_delete_by_name(&data->array, &data->id_list, &data->name_list, name);
            break;
    }
}

int main(void) {
    // index_array();
    index_tree();
    // ListTaskData data;
    // data.is_running = true;
    // data.array = create_array(__alignof(Item), sizeof(Item));
    // data.id_list = nullptr;
    // data.name_list = nullptr;
    //
    // Attribute attribute;
    // List *list;
    // Item *item;
    //
    // while (data.is_running) {
    //     print_list_menu();
    //
    //     int choice = 0;
    //     scanf("%d", &choice);
    //
    //     switch (choice) {
    //         case NewItem: // ввод новых записей
    //             list_task_new_item(&data);
    //             break;
    //         case PrintAsAdded: // просмотр элементов в порядке ввода
    //             print_item_array(data.array);
    //             break;
    //         case PrintByIndex: // просмотр в порядке возрастания атрибута
    //             attribute = scan_attribute();
    //
    //             switch (attribute) {
    //                 case Id:
    //                     print_by_list(&data.array, data.id_list);
    //                     break;
    //                 case Name:
    //                     print_by_list(&data.array, data.name_list);
    //                     break;
    //             }
    //             break;
    //         case Search: // поиск
    //             attribute = scan_attribute();
    //
    //             switch (attribute) {
    //                 case Id:
    //                     uint64_t id = SIZE_MAX;
    //                     scanf("%lu", &id);
    //                     list = list_find_by_id(data.id_list, id);
    //                 break;
    //                 case Name:
    //                     char name[STRING_LENGTH];
    //                     scanf("\n%[^\n]", &name);
    //                     list = list_find_by_name(data.name_list, name);
    //                 break;
    //             }
    //
    //             if (list == nullptr || *LIST_GET_T(list, size_t) == SIZE_MAX)
    //                 printf("Item not found\n");
    //             else
    //                 print_item(GET(&data.array, *LIST_GET_T(list, size_t)));
    //
    //             break;
    //         case Remove: // удаление
    //             list_task_remove(&data);
    //             break;
    //         case 6:
    //             data.is_running = false;
    //             break;
    //         default:
    //             printf("There is not option with number `%d`\n", choice);
    //     }
    // }
    //
    // return 0;
}
//
// Created by p0ancf-36 on 21.02.25.
//

#ifndef LIST_H
#define LIST_H
#include <stddef.h>

#include "Item.h"
#include "Array.h"
#include "types.h"

typedef struct List List;

typedef struct List {
    List *next;
} List;

#define LIST_GET(list) ((List *)(list) + 1)
#define LIST_GET_T(list, T) ((T *)((List *)(list) + 1))

List *create_id_index_list(size_t i, const Item *item);
List *create_name_index_list(size_t i, const Item *item);
void destroy_list(List *);

void list_append(List **, List *, Comparator);

List *list_find_by_id(List *, uint64_t);
List *list_find_by_name(List *, const char *);

void list_delete_by_id(Array *, List **, List **, uint64_t);
void list_delete_by_name(Array *, List **, List **, const char *);

void print_by_list(const Array *array, List *list);

#endif //LIST_H

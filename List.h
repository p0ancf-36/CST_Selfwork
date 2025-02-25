//
// Created by p0ancf-36 on 21.02.25.
//

#ifndef LIST_H
#define LIST_H
#include <stddef.h>

#include "Item.h"
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

List *list_find(List *, void *, Comparator);
List *list_find_by_id(List *, uint64_t);
List *list_find_by_name(List *, const char *);

List *list_cut(List **, void *, Comparator);
List *list_cut_by_id(List **, uint64_t);
List *list_cut_by_name(List **, const char *);
void decrease_indexes(List *list, size_t index);

#endif //LIST_H

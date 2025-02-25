#include "List.h"

#include <stdlib.h>
#include <string.h>

#include "Array.h"
#include "IdIndex.h"
#include "NameIndex.h"
#include "types.h"

List * create_id_index_list(size_t i, const Item *item) {
    List *result = aligned_alloc(__alignof(List), sizeof(List) + sizeof(IdIndex));
    IdIndex index = create_id_index(i, item);
    *LIST_GET_T(result, IdIndex) = index;
    result->next = nullptr;

    return result;
}

List * create_name_index_list(size_t i, const Item *item) {
    List *result = aligned_alloc(__alignof(List), sizeof(List) + sizeof(NameIndex));
    NameIndex index = create_name_index(i, item);
    *LIST_GET_T(result, NameIndex) = index;
    result->next = nullptr;

    return result;
}

void destroy_list(List *list) {
    List *curr = list;

    while (curr->next) {
        void *tmp = curr;
        curr = curr->next;
        free(curr);
    }

    free(curr);
}

void list_append(List **list, List *object, Comparator comparator) {
    List *curr = *list;
    List *prev = nullptr;

    while (curr != nullptr && comparator(LIST_GET(object), LIST_GET(curr)) == Greater) {
        prev = curr;
        curr = curr->next;
    }

    if (prev == nullptr) {
        *list = object;
        object->next = curr;
    } else if (curr == *list) {
        prev->next = object;
    } else {
        prev->next = object;
        object->next = curr;
    }
}

List *list_find(List *list, void *object, Comparator comparator) {
    List *curr = list;

    while (curr != nullptr && comparator(object, LIST_GET(curr)) == Greater) {
        curr = curr->next;
    }

    if (curr == nullptr)
        return nullptr;

    if (comparator(object, LIST_GET(curr)) != Equals)
        return nullptr;

    return curr;
}

List *list_find_by_id(List *list, uint64_t id) {
    IdIndex search_index = create_search_id_index(id);
    return list_find(list, &search_index, id_index_comparator);
}

List *list_find_by_name(List *list, const char *name) {
    NameIndex search_index = create_search_name_index(name);
    return list_find(list, &search_index, name_index_comparator);
}

List *list_cut(List **list, void *object, Comparator comparator) {
    List *curr = *list;
    List *prev = nullptr;

    while (curr != nullptr && comparator(object, LIST_GET(curr)) == Greater) {
        prev = curr;
        curr = curr->next;
    }

    if (curr == nullptr)
        return nullptr;

    if (comparator(object, LIST_GET(curr)) != Equals)
        return nullptr;

    if (prev) {
        prev->next = curr->next;
    } else {
        *list = curr->next;
    }

    return curr;
}

List *list_cut_by_id(List **list, uint64_t id) {
    IdIndex search_index = create_search_id_index(id);
    return list_cut(list, &search_index, id_index_comparator);
}

List *list_cut_by_name(List **list, const char *name) {
    NameIndex search_index = create_search_name_index(name);
    return list_cut(list, &search_index, name_index_comparator);
}

void decrease_indexes(List *list, size_t index) {
    List *curr = list;

    while (curr) {
        if (*LIST_GET_T(curr, size_t) > index) {
            *LIST_GET_T(curr, size_t) -= 1;
        }

        curr = curr->next;
    }
}
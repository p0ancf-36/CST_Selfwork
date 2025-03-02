#include "IdIndex.h"

IdIndex create_id_index(size_t index, const Item *item) {
    IdIndex result;
    result.index = index;
    result.id = item->id;
    return result;
}

IdIndex create_search_id_index(uint64_t id) {
    IdIndex result;
    result.index = SIZE_MAX;
    result.id = id;

    return result;
}

CmpRes compare_id_indexes(const IdIndex *a, const IdIndex *b) {
    if (a->id > b->id)
        return Greater;
    if (a->id < b->id)
        return Less;

    return Equals;
}

CmpRes id_index_comparator(const void *a, const void *b) {
    return compare_id_indexes(a, b);
}
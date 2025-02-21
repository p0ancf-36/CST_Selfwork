#include "NameIndex.h"

#include <string.h>

NameIndex create_name_index(size_t index, const Item *item) {
    NameIndex result;
    result.index = index;
    strcpy(result.name, item->name);
    return result;
}

CmpRes compare_name_indexes(const NameIndex *a, const NameIndex *b) {
    int cmpres = strcmp(a->name, b->name);

    if (cmpres > 0)
        return Greater;
    if (cmpres < 0)
        return Less;

    return Equals;
}

CmpRes name_index_comparator(const void *a, const void *b) {
    return compare_name_indexes(a, b);
}
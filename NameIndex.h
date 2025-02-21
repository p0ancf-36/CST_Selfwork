//
// Created by p0ancf-36 on 08.02.25.
//

#ifndef NAMEINDEX_H
#define NAMEINDEX_H
#include <stdint.h>
#include <stddef.h>
#include "types.h"
#include "Item.h"

typedef struct {
    size_t index;
    char name[STRING_LENGTH];
} NameIndex; // Индекс по названию

NameIndex create_name_index(size_t, const Item *);
NameIndex create_search_name_index(char *);
CmpRes compare_name_indexes(const NameIndex *, const NameIndex *);
CmpRes name_index_comparator(const void *, const void *);

#endif //NAMEINDEX_H

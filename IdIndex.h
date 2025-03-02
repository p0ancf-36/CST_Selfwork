//
// Created by p0ancf-36 on 08.02.25.
//

#ifndef IDINDEX_H
#define IDINDEX_H
#include <stdint.h>
#include <stddef.h>
#include "types.h"
#include "Item.h"

typedef struct {
    size_t index;
    uint64_t id;
} IdIndex; // Индекс по идентификатору

IdIndex create_id_index(size_t, const Item *);
IdIndex create_search_id_index(uint64_t);
CmpRes compare_id_indexes(const IdIndex *, const IdIndex *);
CmpRes id_index_comparator(const void *, const void *);

#endif //IDINDEX_H

#include "IndexArray.h"

#include <stdlib.h>
#include <string.h>

#include "IdIndex.h"
#include "Item.h"
#include "NameIndex.h"

void* find(Array *source, void *object, const Comparator comparator) {
    return find_r(source, object, comparator, 0, source->size-1);
}

void* find_r(Array *source, void *object, const Comparator comparator, const size_t l, const size_t r) {
    if (l >= r)
        return GET(source, l);

    const size_t m = l + (r - l + 1) / 2;

    if (comparator(object, GET(source, m)) >= Equals)
        return find_r(source, object, comparator, m, r);

    return find_r(source, object, comparator, l, m - 1);
}

void* find_i(const Array *source, const void *object, const Comparator comparator) {
    size_t l = 0, r = source->size - 1;

    while (l < r) {
        const size_t m = l + (r - l + 1) / 2;

        if (comparator(object, GET(source, m)) >= Equals)
            l = m;
        else
            r = m - 1;
    }

    return GET(source, l);
}

void swap_sorted(Array *source, const size_t index, const Comparator comparator) {
    if (source->size < 2)
        return;

    size_t l = 0, r = source->size - 1;
    void *object = GET(source, index);
    void *tmp = malloc(source->struct_size);
    memcpy(tmp, object, source->struct_size);

    if (index == l) {
        l = 1;
    } else if (index == r || comparator(object, GET(source, index - 1)) == Less) {
        r = index - 1;
    } else {
        l = index + 1;
    }

    void *place = find_r(source, object, comparator, l, r);

    if (comparator(tmp, place) == Greater) {
        place += source->struct_size;
    }

    if (place == object) {

    } else if (place < object) {
        memcpy(place + source->struct_size, place, object - place);
        memcpy(place, tmp, source->struct_size);
    } else {
        memcpy(object, object + source->struct_size, place - object);
        memcpy(place, tmp, source->struct_size);
    }

    free(tmp);
}

typedef struct {
    size_t source, id, name;
} tri_index;

tri_index find_indexes_by_id(const Array *source, const Array *id_index, const Array *name_index, const uint64_t id) {
    const IdIndex search_id_index = create_search_id_index(id);
    const IdIndex *id_i = find_i(id_index, &search_id_index, id_index_comparator);

    if (id_index_comparator(id_i, &search_id_index) != Equals) {
        return (tri_index)  {SIZE_MAX, 0, 0};
    }

    const size_t source_index = id_i->index;
    const Item *item = GET(source, source_index);

    const NameIndex search_name_index = create_search_name_index(item->name);
    const NameIndex *name_i = find_i(name_index, &search_name_index, name_index_comparator);

    return (tri_index) {source_index, index_of(id_index, id_i), index_of(name_index, name_i)};
}

tri_index find_indexes_by_name(const Array *source, const Array *id_index, const Array *name_index, const char *name) {
    const NameIndex search_name_index = create_search_name_index(name);
    const NameIndex *name_i = find_i(name_index, &search_name_index, name_index_comparator);

    if (name_index_comparator(name_i, &search_name_index) != Equals) {
        return (tri_index) {SIZE_MAX, 0, 0};
    }

    const size_t source_index = name_i->index;
    const Item *item = GET(source, source_index);

    const IdIndex search_id_index = create_search_id_index(item->id);
    const IdIndex *id_i = find_i(id_index, &search_id_index, id_index_comparator);

    return (tri_index) {source_index, index_of(id_index, id_i), index_of(name_index, name_i)};
}

void remove_by_tri_indexes(Array *source, Array *id_index, Array *name_index, const tri_index indexes) {
    remove_on(name_index, indexes.name);
    remove_on(id_index, indexes.id);
    remove_on(source, indexes.source);

    for (size_t i = 0; i < source->size; i++) {
        IdIndex *id_i = GET(id_index, i);
        NameIndex *name_i = GET(name_index, i);

        if (id_i->index > indexes.source) {
            id_i->index--;
        }
        if (name_i->index > indexes.source) {
            name_i->index--;
        }
    }
}

void remove_by_id(Array *source, Array *id_index, Array *name_index, const uint64_t id) {
    const tri_index indexes = find_indexes_by_id(source, id_index, name_index, id);

    if (indexes.source == SIZE_MAX)
        return;

    remove_by_tri_indexes(source, id_index, name_index, indexes);
}

void remove_by_name(Array *source, Array *id_index, Array *name_index, const char *name) {
    const tri_index indexes = find_indexes_by_name(source, id_index, name_index, name);

    if (indexes.source == SIZE_MAX)
        return;

    remove_by_tri_indexes(source, id_index, name_index, indexes);
}

void edit_by_tri_indexes(const Array *source, Array *id_index, Array *name_index, const tri_index indexes) {
    Item *item = GET(source, indexes.source);
    IdIndex *id_i = GET(id_index, indexes.id);
    NameIndex *name_i = GET(name_index, indexes.name);

    edit_item(item);
    id_i->id = item->id;
    strcpy(name_i->name, item->name);

    swap_sorted(id_index, indexes.id, id_index_comparator);
    swap_sorted(name_index, indexes.name, name_index_comparator);
}

void edit_by_id(const Array *source, Array *id_index, Array *name_index, const uint64_t id) {
    const tri_index indexes = find_indexes_by_id(source, id_index, name_index, id);

    if (indexes.source == SIZE_MAX)
        return;

    edit_by_tri_indexes(source, id_index, name_index, indexes);
}

void edit_by_name(const Array *source, Array *id_index, Array *name_index, const char *name) {
    const tri_index indexes = find_indexes_by_name(source, id_index, name_index, name);

    if (indexes.source == SIZE_MAX)
        return;

    edit_by_tri_indexes(source, id_index, name_index, indexes);
}

void print_by_id(const Array *source, const Array *id_index) {
    for (int i = 0; i < id_index->size; i++) {
        print_item(GET(source, ((IdIndex *) GET(id_index, i))->index));
    }
}
void print_by_name(const Array *source, const Array *name_index) {
    for (int i = 0; i < name_index->size; i++) {
        print_item(GET(source, ((NameIndex *) GET(name_index, i))->index));
    }
}
#include "Array.h"

#include <stdlib.h>
#include <string.h>

Array create_array(size_t alignment, size_t struct_size) {
    Array result;
    result.capacity = 1;
    result.size = 0;
    result.struct_size = struct_size;
    result.alignment = alignment;
    result.data = aligned_alloc(alignment, struct_size * result.capacity);

    return result;
}

void destroy_array(Array *array) {
    free(array->data);
    array->data = NULL;
}

void append(Array *dest, void *object) {
    if (dest->size == dest->capacity)
        expand(dest);

    set(dest, dest->size, object);
    dest->size += 1;
}

void expand(Array *array) {
    array->capacity <<= 1;
    void *ndata = aligned_alloc(array->alignment, array->capacity * array->struct_size);
    memcpy(ndata, array->data, array->size * array->struct_size);
    free(array->data);
    array->data = ndata;
}

void set(Array *array, size_t index, void *object) {
    memcpy(get(array, index), object, array->struct_size);
}

inline void *get(Array *array, size_t index) {
    return array->data + index * array->struct_size;
}

inline size_t index_of(Array *array, void *object) {
    return (object - array->data) / array->struct_size;
}

void remove_on(Array *array, size_t index) {
    array->size -= 1;
    if (index == array->size) {
        return;
    }
    memcpy(get(array, index), get(array, index + 1), (array->size - index) * array->struct_size);
}
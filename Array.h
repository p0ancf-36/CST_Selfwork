//
// Created by p0ancf-36 on 08.02.25.
//

#ifndef ARRAY_H
#define ARRAY_H
#include <stddef.h>

typedef struct {
    void* data;
    size_t size;
    size_t capacity;
    size_t struct_size;
    size_t alignment;
} Array;

Array create_array(size_t, size_t);
void destroy_array(Array *);

void append(Array *, const void *);
void expand(Array *);

void set(const Array *, size_t, const void *);

#define GET(array, index) ((array)->data + (index) * (array)->struct_size)
#define GET_T(array, index, type) ((type *)((array)->data + (index) * (array)->struct_size))

size_t index_of(const Array *array, const void *object);
void remove_on(Array *, size_t);

#endif //ARRAY_H

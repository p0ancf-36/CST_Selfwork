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

void append(Array *, void *);
void expand(Array *);

void set(Array *, size_t, void *);
void *get(Array *, size_t);

size_t index_of(Array *array, void *object);
void remove_on(Array *, size_t);

#endif //ARRAY_H

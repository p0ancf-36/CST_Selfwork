//
// Created by p0ancf-36 on 08.02.25.
//

#ifndef INDEXARRAY_H
#define INDEXARRAY_H
#include <stdint.h>
#include "types.h"
#include "defines.h"
#include "Array.h"

void* find(const Array *, const void *, Comparator);

void* find_r(const Array *, const void *, Comparator, size_t l, size_t r);
void* find_i(const Array *, const void *, Comparator);

void swap_sorted(const Array *, size_t, Comparator);

void remove_by_id(Array *, Array *, Array *, uint64_t);
void remove_by_name(Array *, Array *, Array *, const char *);

void edit_by_id(const Array *, Array *, Array *, uint64_t);
void edit_by_name(const Array *, Array *, Array *, const char *);

void print_by_id(const Array *, const Array *);
void print_by_name(const Array *, const Array *);

#endif //INDEXARRAY_H

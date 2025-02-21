//
// Created by p0ancf-36 on 08.02.25.
//

#ifndef INDEXARRAY_H
#define INDEXARRAY_H
#include <stdint.h>
#include "types.h"
#include "defines.h"
#include "Array.h"

void* find(Array *, void *, Comparator);

void* find_r(Array *, void *, Comparator, size_t l, size_t r);
void* find_i(Array *, void *, Comparator);

void swap_sorted(Array *, size_t, Comparator);

void remove_by_id(Array *, Array *, Array *, uint64_t);
void remove_by_name(Array *, Array *, Array *, char *);

void edit_by_id(Array *, Array *, Array *, uint64_t);
void edit_by_name(Array *, Array *, Array *, char *);

void print_by_id(Array *, Array *);
void print_by_name(Array *, Array *);

#endif //INDEXARRAY_H

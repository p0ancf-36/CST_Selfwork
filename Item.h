//
// Created by p0ancf-36 on 08.02.25.
//

#ifndef ITEM_H
#define ITEM_H
#include <stdint.h>
#include <stdio.h>
#include "defines.h"

typedef struct {
    uint64_t id;
    uint32_t in_stock;
    uint32_t count;
    uint32_t av_rent_count;
    uint32_t av_rent_time;

    char name[STRING_LENGTH];
} Item;

Item scan_item();
Item fscan_item(FILE *);
void print_item(const Item *);
void edit_item(Item *);

#endif //ITEM_H

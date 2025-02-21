//
// Created by p0ancf-36 on 08.02.25.
//

#ifndef TYPES_H
#define TYPES_H

typedef enum {
    Less = -1, Equals, Greater
} CmpRes; // Результат сравнения

typedef CmpRes (*Comparator)(const void *, const void *);

#endif //TYPES_H

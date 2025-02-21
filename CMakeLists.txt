cmake_minimum_required(VERSION 3.30)
project(selfwork C)

set(CMAKE_C_STANDARD 23)

add_executable(selfwork main.c
        types.h
        defines.h
        Item.h
        Array.h
        IdIndex.h
        NameIndex.h
        IndexArray.h
        Item.c
        Array.c
        IdIndex.c
        NameIndex.c
        IndexArray.c)

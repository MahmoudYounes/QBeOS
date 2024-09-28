#ifndef VECTOR_H
#define VECTOR_H

#include "include/common.h"
#include "arch/include/vmm.h"

/**
* TODO and Note to self:
* before continuing working on this class and the rest of the stdlib, your
* vmm should support allocating small chunks of memory rather than the 4kb
* chunk with the allocation of every new;
*/

template <typename T> class Vector{
private: 
    uint64_t length;
    uint64_t capacity;
    void expandStore();
    T store[];
public:
    Vector();
    Vector(T arr[]);
    void Push(T item);
    T Pop();
};

#endif


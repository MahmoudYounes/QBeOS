#ifndef VECTOR_H
#define VECTOR_H

#include "include/common.h"
#include "arch/include/vmm.h"

/**
* TODO and Note to self:
* before continuing working on this class and the rest of the stdlib, your
* vmm should support allocating small chunks of memory rather than the 4kb
* chunk with the allocation of every new;
*
* a very good resource to read about is in the golang memory allocator
*
* this is needed to better manage memory. however, this doesn't come for free.
* my concern is having to implement a garbage collector in the OS which is
* apparently a bad idea as it requires a stop the world event.
*
*/

template <typename T> class Vector{
private: 
    uint64_t length;
    uint64_t capacity;
    T *store;
    void expandStore(){
      capacity *= 2;
      T *newStore = new T[capacity];
      for (uint64_t i = 0; i < length; i++) {
        newStore[i] = store[i];
      }

      delete[] store;
      store = newStore;
    }

public:
    Vector(){
      capacity = 2;
      length = 0;
      store = new T[capacity];
    };

    void Push(T item){
      if (length == capacity){
        expandStore();
      }
      store[length] = item;
      length++;
    };


    T Pop(){
      // TODO: consider dynamically shrinking the vector
      if (length == 0){
        return 0;
      }
      length--;
      return store[length];
    };
};

#endif


#include "kstdlib/include/vector.h"


template <typename T> Vector<T>::Vector() {
    capacity = 2;
    length = 0;
    store = new T[capacity];
}

template <typename T> void Vector<T>::Push(T item) { 
    if (length == capacity){
        expandStore();
    }
    store[length] = item;
    length++;
}

template <typename T> void Vector<T>::expandStore() {
    capacity *= 2;
    T newStore[] = new T[capacity];
    for (uint64_t i = 0; i < length; i++) {
        newStore[i] = store[i];
    }

    delete[] store;
    store = newStore;
}


template <typename T> T Vector<T>::Pop() {
    // TODO: consider dynamically shrinking the vector
    if (length == 0){
        return 0;
    }
    length--;
    return store[length + 1];
}

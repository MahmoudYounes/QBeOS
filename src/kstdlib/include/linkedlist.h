#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <include/common.h>

// Generic intrusive linked lists
template <typename T> class LinkedList{
public:
  T data;
  LinkedList<T> *next;
  LinkedList(){};
  LinkedList(T item){
    LinkedList(item, NULL);
  };
  LinkedList(T item, LinkedList *ll){
    data = item;
    next = ll;
  };

  void AddNext(LinkedList *ll){
    next = ll;
  }
}; 

#endif

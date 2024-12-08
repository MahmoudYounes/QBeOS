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
    this->data = item;
    this->next = NULL;
  };
  LinkedList(T item, LinkedList *ll){
    this->data = item;
    this->next = ll;
  };

  void AddNext(LinkedList *ll){
    next = ll;
  }
}; 

#endif

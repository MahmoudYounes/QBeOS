#ifndef HASHTABLE_H
#define HASHTABLE_H

// Generic Hashtable
#include "kstdlib/include/hash.h"
#include "kstdlib/include/linkedlist.h"
#include "kstdlib/include/simplehasher.h"

template <typename K, typename V> class HashTable{
private:
  Hasher *h;
  uint32_t capacity;
  uintptr_t *store;
  
  class Node{
  public:
    K key;
    V val;
    Node (K k, V v){
      key = k;
      val = v;
    } 
  };

public:
  HashTable(uint32_t cap){
    capacity = cap;
    h = new SimpleHasher(capacity); 
    store = new uintptr_t[capacity]; 
  };

  void Insert(K key, V val){
    uint32_t hash = h->Hash(key);
    Node node(key, val);

    LinkedList<Node> *ptr, *pptr = ptr = store[hash];
    if (ptr == NULL){
      // first insertion
      store[hash] = &LinkedList<Node>(node);
    } else {
      while (ptr != NULL){
        if (ptr->data.key == node.key){
          // if we found the same key, we override it
          ptr->data = node;
          return;
        }

        pptr = ptr;
        ptr = ptr->next; 
      }

      // if we found same hash different values
      pptr->AddNext(&LinkedList<Node>(Node(key, val)));
    } 
  };

  V Get(K key){
    uint32_t hash = h->Hash(key);

    LinkedList<Node> *ptr = store[hash];
    while (ptr->next != NULL){
      if (ptr->data.key == key){
        return ptr->data.val;
      }

      ptr = ptr->next;
    }
    return 0;
  }
};

#endif

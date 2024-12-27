#ifndef HASHTABLE_H
#define HASHTABLE_H

// Generic Hashtable
#include "kstdlib/include/hash.h"
#include "kstdlib/include/linkedlist.h"
#include "kstdlib/include/simplehasher.h"

template <typename K, typename V> class HashTable{
private:  
  class Node{
  public:
    K key;
    V val;
    Node (K k, V v){
      key = k;
      val = v;
    } 
  };

  Hasher *h;
  uint32_t capacity;
  LinkedList<Node*> **store;

public:
  HashTable(uint32_t cap){
    capacity = cap;
    h = new SimpleHasher(capacity); 
    store = new LinkedList<Node*>*[capacity]; 
  };

  void Insert(K key, V val){
    uint64_t hash = *(uint64_t *)h->Hash((uint8_t*)&key, sizeof(key));
    Node *node = new Node(key, val);

    LinkedList<Node*> *ptr, *pptr = ptr = store[hash];
    if (ptr == NULL){
      // first insertion
      store[hash] = new LinkedList<Node*>(node);
    } else {
      while (ptr != NULL){
        if (ptr->data->key == node->key){
          // if we found the same key, we override it
          ptr->data = node;
          return;
        }

        pptr = ptr;
        ptr = ptr->next; 
      }

      // if we found same hash different values
      pptr->AddNext(new LinkedList<Node*>(node));
    } 
  };

  V Get(K key){
    uint64_t hash = *(uint64_t *)h->Hash((uint8_t*)&key, sizeof(key));

    LinkedList<Node*> *ptr = store[hash];
    while (ptr != NULL){
      if (ptr->data->key == key){
        return ptr->data->val;
      }

      ptr = ptr->next;
    }
    return 0;
  }
};

#endif

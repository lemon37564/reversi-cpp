#include <cstddef>
#include <cstdint>

#ifndef __node_h__
#define __node_h__

class Node {
   public:
    int16_t loc;
    int16_t value;
    Node();
    Node(int16_t loc, int16_t value);
};

class Nodes {
   public:
    Node* nodes;
    size_t len;

    void shuffle();
    void sort_desc();
    void sort_asc();
    void push(Node node);

   private:
    void swap(int i, int j);
};

Nodes pool_get_clear_one();
void pool_pseudo_get();
void pool_free_one();

#endif
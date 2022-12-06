#include "node.hpp"
#include <cstdio>
#include <cstdlib>

static Node pool[32][32];
int curr = 0;

Node::Node() {
    this->loc = 0;
    this->value = 0;
}

Node::Node(int16_t loc, int16_t value) {
    this->loc = loc;
    this->value = value;
}

void Nodes::swap(int i, int j) {
    Node tmp = (this->nodes)[i];
    (this->nodes)[i] = (this->nodes)[j];
    (this->nodes)[j] = tmp;
}

void Nodes::shuffle() {
    size_t len = this->len;
    for (size_t i = 0; i < len; i++) {
        size_t r1 = rand() % len, r2 = rand() % len;
        this->swap(r1, r2);
    }
}

void Nodes::sort_desc() {
    size_t len = this->len;
    if (len > 1) {
        for (size_t i = 1; i < len; i++) {
            for (size_t j = i; j > 0 && this->nodes[j].value > this->nodes[j - 1].value; j--) {
                this->swap(j, j - 1);
            }
        }
    }
}

void Nodes::sort_asc() {
    size_t len = this->len;
    if (len > 1) {
        for (size_t i = 1; i < len; i++) {
            for (size_t j = i; j > 0 && this->nodes[j].value < this->nodes[j - 1].value; j--) {
                this->swap(j, j - 1);
            }
        }
    }
}

void Nodes::push(Node node) {
    this->nodes[this->len] = node;
    this->len++;
}

Nodes pool_get_clear_one() {
    Nodes ns = {.nodes = (Node*)pool[curr], .len = 0};
    curr++;
    return ns;
}

void pool_pseudo_get() {
    curr++;
}

void pool_free_one() {
    curr--;
}

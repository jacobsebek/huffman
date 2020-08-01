#pragma once

#include <stddef.h> //size_t
#include "bitio.h" // byte

struct node;

typedef struct node {
    byte c; // If null, the node is considered as a branch node
    size_t occur; // The total occurence
    struct node *left, *right; // Left and right kids
} node;

typedef struct heap {
    node** data;
    size_t num_nodes;
    size_t max_nodes;
} heap;

void print_recursive(const node* root, unsigned base_depth);

heap* heap_create(const size_t size);
void heap_destroy(heap* heap);

node* node_create(const unsigned char c, const size_t occur, node* left, node* right);
void node_destroy(node* node); // deep destroy

node* heap_top(const heap* heap);
node* heap_pop(heap* heap);
void heap_add(node* new, heap* heap);
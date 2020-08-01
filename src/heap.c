#include "heap.h"
#include "exitflags.h"

#include <stdlib.h> //malloc
#include <stddef.h> //size_t
#include <stdio.h> //printf

#include <stdbool.h>

void print_recursive(const node* root, unsigned base_depth) // Prints node tree recursively
{
    if (root == NULL) return;

    print_recursive(root->left, base_depth+1);
    print_recursive(root->right, base_depth+1);

    for (register unsigned i = 0; i < base_depth; i++) putchar('.');

    if (root->left == NULL && root->right == NULL)
        if (root->occur == 0)
            printf("(%c%c[%d])\n", root->c == '\n' ? '\\' : root->c, root->c == '\n' ? 'n' : ' ', root->c);
        else
            printf("(%c%c[%d], %ld)\n", root->c == '\n' ? '\\' : root->c, root->c == '\n' ? 'n' : ' ', root->c, root->occur);
    else 
        printf("(--%ld--)\n", root->occur);
}

heap* heap_create(const size_t length)
{
    heap* new = malloc(sizeof(heap));
    if (new == NULL) {
        fprintf(stderr, EXMSG_BADALLOC);
        exit(EXIT_MEMO_ALFAULT);
    }

    new->max_nodes = length;
    new->num_nodes = 0;
    new->data = calloc(length, sizeof(node*));

    if (new->data == NULL) {
        fprintf(stderr, EXMSG_BADALLOC);
        exit(EXIT_MEMO_ALFAULT);
    }

    return new;
}

void heap_destroy(heap* heap) // Does not destroy the nodes
{
    free(heap->data);
    free(heap);
}

node* node_create(const unsigned char c, const size_t occur, node* left, node* right)
{
    node* new = malloc(sizeof(node));
    if (new == NULL) {
        fprintf(stderr, EXMSG_BADALLOC);
        exit(EXIT_MEMO_ALFAULT);
    }

    new->c = c;
    new->occur = occur;
    new->left = left;
    new->right = right;

    return new;
}

void node_destroy(node* node) // recursively destroys the whole node tree
{
    if (node == NULL) return;

    node_destroy(node->left);
    node_destroy(node->right);

    free(node);
}

node* heap_top(const heap* heap)
{
    return heap->data[0];
}

node* heap_pop(heap* heap)
{
    node* pop = heap_top(heap);
    if (pop == NULL) return pop;

    node* tmp = heap->data[--heap->num_nodes]; // No need to swap first and last, I can use the last node as tmp straight away

	for (int j = 0; j < heap->num_nodes;) {

		register const int li = j*2+1, ri = li+1;

		if (ri < heap->num_nodes && (heap->data[ri]->occur) < (tmp->occur) && (heap->data[li]->occur) > (heap->data[ri]->occur)) {
			heap->data[j] = heap->data[ri];
			j = ri;
		} else if (li < heap->num_nodes && (heap->data[li]->occur) < (tmp->occur)) {
			heap->data[j] = heap->data[li];
			j = li;
		} else {
			heap->data[j] = tmp;
			break;
		}
	}

    return pop;
}

void heap_add(node* new, heap* heap)
{
    if (heap->num_nodes >= heap->max_nodes) {
        fprintf(stderr, EXMSG_INDEX);
        exit(EXIT_MEMO_IXFAULT);
    }

    register unsigned j = heap->num_nodes++;
    while (j != 0) {
        unsigned parent = (j-1)/2;

        if ((heap->data[parent]->occur) > (new->occur)) {
            heap->data[j] = heap->data[parent];
            j = parent;
        } else break;

    }

    heap->data[j] = new;

}
#include "exitflags.h"
#include "heap.h"
#include "bitio.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
//#include <string.h>

/*
Compression :
    1. Find occurence of all characters in file
    2. Sort them by occurence in a priority queue (lowest occurence have the highest priority)
    3. Create a tree-like structure by getting the two last nodes from the queue and creating
       an internal node having these two as children and their sum of occurences as occurence
    4. Goto step 3 if the priority queue is not empty
    5. Write the structure in a file acoordingly using this method : (https://stackoverflow.com/a/759766/7767689)
       with the difference that all the letters corresponding to the 1 bits are stored next to each other after
       the bit sequence (easier to write)
       The file format (.huf) shall be :"HUF"-
                                        [number of characters to be read (32bit)]
                                        [mapping bit sequence (leaves*2-1)]
                                        [corresponding glyphs (leaves)]
                                        [data]
*/

/*

*/
static void com_tree_map(const node* root, FILE* out)
{
    if (root == NULL) return;
    // Write the map bit sequence

    if (root->left == NULL && root->right == NULL) {
        bit_write(1, out);
    } else { 
        bit_write(0, out);
        com_tree_map(root->left, out);
        com_tree_map(root->right, out);
    }

}

/*

*/
static void com_tree_chars(const node* root, FILE* out)
{
    if (root->left == NULL && root->right == NULL) {
        byte_write(root->c, out);
    } else {
        com_tree_chars(root->left, out);
        com_tree_chars(root->right, out);
    }
}

/*
Hash all the codes to an array from the specified huffman tree
*/
static void com_tree_hash(const node* root, bitstring codes[256], bitstring current)
{
    if (root == NULL) return;

    if (root->left == NULL && root->right == NULL) {
        codes[root->c] = current;
        return;
    } else {
        current.size++;

        bitstring_setbit(&current, 0, current.size-1);
        com_tree_hash(root->left,  codes, current);

        bitstring_setbit(&current, 1, current.size-1);
        com_tree_hash(root->right, codes, current);
    }

}

void compress(FILE* in, FILE* out, const _Bool debug)
{
    heap* heap = heap_create(256);
    size_t counts[256] = { 0 };

    uint32_t chars_total = 0;
    // Count occurence of each letter
    for(register byte c; c = fgetc(in), !feof(in) && !ferror(in);) {
        counts[c]++;

        if (++chars_total == UINT32_MAX) {
            fprintf(stderr, EXMSG_TOOLARGE);
            exit(EXIT_MEMO_IXFAULT);
        }

    }

    if (ferror(in)) {
        fprintf(stderr, EXMSG_READ);
        exit(EXIT_FILE_READERR);
    }

    unsigned leaves = 0;
    // Populate the heap with maintaining the heap property
    for (register unsigned i = 0; i < 256; i++) {
        if (counts[i] == 0) continue;

        heap_add(node_create(i, counts[i], NULL, NULL), heap);
        leaves++;
    }

    // Gradually build the tree until only one node is left
    while (heap->num_nodes >= 2) {
        node *l = heap_pop(heap),
             *r = heap_pop(heap);
        heap_add(node_create('\0', (l->occur)+(r->occur), l, r), heap);
    }

    // SERIALIZE (https://stackoverflow.com/a/759766/7767689)

    fwrite("HUF", sizeof(byte), 3, out);
    fwrite(&chars_total, sizeof(chars_total), 1, out);

    if (ferror(out)) {
        fprintf(stderr, EXMSG_WRITE);
        exit(EXIT_FILE_WRITERR);
    }

    com_tree_map(heap_top(heap), out);
    bit_wflush(out); // The buffer might not be flushed

    com_tree_chars(heap_top(heap), out); // No need to flush since this writes whole bytes

    bitstring codes[256] = { {0}, 0 };
    com_tree_hash(heap_top(heap), codes, (bitstring){ {0} , 0});

    if (debug) {
        printf("\nThe whole tree : \n");
        print_recursive(heap_top(heap), 0); // Print tree to console
        printf("\nIndividual Huffman codes : \n");

        for (register unsigned i = 0; i < 256; i++) {
            if (codes[i].size == 0) continue; 

            printf("%c%c[%3d]: ", i == '\n' ? '\\' : i < 32 ? ' ' : i, i == '\n' ? 'n' : ' ', i);

            for (register unsigned b = 0; b < codes[i].size; b++)
                printf("%d", bitstring_getbit(&codes[i], b));
            putchar('\n');
        }

        printf("\nFirst 16 characters : \n");
    }

    // Rewind the input file back to the beggining
    if (fseek(in, 0, SEEK_SET) != 0) {
        fprintf(stderr, EXMSG_READ);
        exit(EXIT_FILE_LOADERR);
    }

    size_t count_chs = 0, print_chs = 16; //debug mode
    size_t count_out = 0; // Numebr of outputed bits
    for(register byte c; c = fgetc(in), !feof(in) && !ferror(in); count_chs++, count_out += codes[c].size) {

        if (debug && count_chs < print_chs)
            printf("%c%c[%3d]: ", c == '\n' ? '\\' : c < 32 ? ' ' : c, c == '\n' ? 'n' : ' ', c);

        for (register unsigned b = 0; b < codes[c].size; b++) {

            if (debug && count_chs < print_chs)
                putchar(bitstring_getbit(&codes[c], b) ? '1' : '0');

            bit_write(bitstring_getbit(&codes[c], b), out);
        }

        if (debug && count_chs+1 < print_chs) {
            printf(", ");
            if (count_chs != 0 && count_chs % 6 == 0) putchar('\n');
        }
    }
    if (debug) printf(" ...(%ld out of %ld characters total printed) \n", count_chs < print_chs ? count_chs : print_chs, count_chs);

    bit_wflush(out);

    printf("\nCompressed to %.1f%% of the base size.", (3+((leaves*2-1)/8.0)+leaves+(count_out/8.0))/count_chs*100.0);

    node_destroy(heap_top(heap)); // Destroy the whole tree recursively
    heap_destroy(heap);
}
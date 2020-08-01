#include "exitflags.h"
#include "heap.h"
#include "bitio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strcmp

/*
Decompression:
    1. Make sure the file strats with "HUF"
    2. Read the total number of characters (32bit)
    3. Create a huffman tree out of the following bits until
       [leaves] 1's is visited
    4. Move to the next byte if necessary
    5. Assign all the characters ([leaves] bytes) to the corresponding leaves
    6. Recursively decode the following sequence until a set amount of characetrs has been visited

*/
node* dec_tree_build(FILE* in) // Create an empty tree from the map
{
    static unsigned nodes = 0;

    if (nodes > 256) {
        fprintf(stderr, EXMSG_CORRUPT);
        exit(EXIT_FILE_READERR);
    }

    if (bit_read(in) == 1) {

        if (feof(in)) {
            fprintf(stderr, EXMSG_CORRUPT);
            exit(EXIT_FILE_READERR);
        }

        return node_create('\0', 0, NULL, NULL);
        nodes++;
    } else {

        //printf("%d, \n", *num_leaves);
        node *l = dec_tree_build(in),
             *r = dec_tree_build(in);
        node* new = node_create('\0', 0, l, r); // occur = 0 because we do not need it at all when decoding
        nodes++;
        
        return new;
    }
}

void dec_tree_fill(node* root, FILE* in) // Fill the characters to the tree
{
    if (root == NULL) return;

    if (root->left == NULL && root->right == NULL) { // Leaf node

        if (feof(in)) {
            fprintf(stderr, EXMSG_CORRUPT);
            exit(EXIT_FILE_READERR);
        }

        root->c = byte_read(in);
    } else {
        dec_tree_fill(root->left, in);
        dec_tree_fill(root->right, in);
    }

}

byte dec_tree_decode(node* root, FILE* in) // returns next decoded character from stream
{
    //static bitstring bs = { 0, 0};
    if (root == NULL) return 0;

    if (root->left == NULL && root->right == NULL) {
        /*printf("%c - ", root->c);
        for (register byte b = 0; b < bs.size; b++) putchar(bitstring_getbit(&bs, b) ? '1' : '0');
        putchar('\n');
        bs = (bitstring){{0},0};*/
        return root->c;
    } else {

        if (feof(in)) {
            fprintf(stderr, EXMSG_CORRUPT);
            exit(EXIT_FILE_READERR);
        }

        if (bit_read(in) == 0) {
            //bitstring_setbit(&bs, 0, bs.size++);
            return dec_tree_decode(root->left, in);
        } else {
            //bitstring_setbit(&bs, 1, bs.size++);
            return dec_tree_decode(root->right, in);
        }
    }

}

void decompress(FILE* in, FILE* out, const _Bool debug)
{
    {
        byte head[3+1] = { 0 };
        fgets(head, 3+1, in);
        if (ferror(in) || feof(in)) {
            fprintf(stderr, EXMSG_READ);
            exit(EXIT_FILE_READERR);
        }

        if (strcmp(head, "HUF") != 0) {
            fprintf(stderr, EXMSG_CORRUPT);
            exit(EXIT_FILE_READERR);
        }
    }

    uint32_t chars_left;
    fread(&chars_left, sizeof(chars_left), 1, in);
    if (ferror(in) || feof(in)) {
        fprintf(stderr, EXMSG_READ);
        exit(EXIT_FILE_READERR);
    }

    node* top = dec_tree_build(in);
    bit_rflush(in); // Get to next byte
    dec_tree_fill(top, in);

    if (debug) {
        printf("\nThe whole tree : \n");
        print_recursive(top, 0);
    }

    while (!feof(in) && !ferror(in) && chars_left-- > 0)
        byte_write( dec_tree_decode(top, in) , out);

    node_destroy(top);
}
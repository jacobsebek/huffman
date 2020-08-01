#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "exitflags.h"

#define LINE_MAX 128

#define OPER_COMPRESS 'c'
#define OPER_DECOMPRESS 'd'

int main(const int argc, const char *argv[])
{
    printf("This software was created by github.com/jacobsebek and is distributed under the MIT license.\n");

    // ARGUMENT CHECKS
    if (argc-1 != 3)
    {
        printf("Invalid argument count, check out the tutorial document.\n");
        exit(EXIT_ARGS_BADSIZE);
    }

    if (argv[1][0] != '-')
    {
        printf("Operation not specified, check out the tutorial document.\n");
        exit(EXIT_OPER_MISSING);
    }

    if (argv[1][1] != OPER_COMPRESS &&
        argv[1][1] != OPER_DECOMPRESS) {

        printf("Invalid operation, check out the tutorial document.\n");
        exit(EXIT_OPER_INVALID);
    }

    _Bool debug = 0;
    if (argv[1][2] == 'i')
        debug = 1;

    extern void decompress(FILE* in, FILE* out, const _Bool debug);
    extern void compress(FILE* in, FILE* out, const _Bool debug);

    // LOAD THE INPUT AND OUTPUT FILES

    FILE *in = fopen(argv[2], (argv[1][1] == OPER_COMPRESS) ? "r" : "rb");
    if (in == NULL)
    {
        printf("An error occured whilst opening the input file, check out the tutorial document.\n");
        exit(EXIT_FILE_LOADERR);
    }

    FILE *out = fopen(argv[3], (argv[1][1] == OPER_COMPRESS) ? "wb" : "w");
    if (out == NULL) {
        printf("An error occured whilst creating the output file, check out the tutorial document.\n");
        exit(EXIT_FILE_LOADERR);
    }

    // PERFORM OPERATION

    clock_t start = clock();

    if (argv[1][1] == OPER_COMPRESS) {
        printf("Running the Huffman coding compression algorithm %s the debug mode.\nInput file : %s; Output file : %s;\n", debug ? "with" : "without", argv[2], argv[3]);
        compress(in, out, debug);
    } else {
        printf("Running the Huffman coding decompression algorithm %s the debug mode.\nInput file : %s; Output file : %s;\n", debug ? "with" : "without", argv[2], argv[3]);
        decompress(in, out, debug);
    }

    double duration = (double)(clock()-start)/CLOCKS_PER_SEC;
    printf("\nThis %s took %.2lf seconds.", argv[1][1] == OPER_COMPRESS ? "compression " : "decompression", duration);

    printf("\n%s successful!\n", argv[1][1] == OPER_COMPRESS ? "Compression " : "Decompression");

    fclose(out);
    fclose(in);

    exit(EXIT_OK);
}
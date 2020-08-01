#pragma once

#include <stdio.h>
#include <stdint.h>

typedef uint8_t bit;
typedef uint8_t byte;

typedef struct bitstring {
    uint64_t bits[4]; // 256 bits
    unsigned size; // 0 to 256
} bitstring;

void bitstring_setbit(bitstring* bs, const bit b, const unsigned index);
bit bitstring_getbit(const bitstring* bs, const unsigned index);

void bit_write(const bit b, FILE* out);
const bit bit_read(FILE* in);

const byte bit_wflush(FILE* out); // Returns the number of bits flushed
const byte bit_rflush(FILE* in);

void bits_write(const bitstring bs, FILE* out);
const bitstring bits_read(FILE* in, const unsigned num);

void byte_write(const byte b, FILE* out);
const byte byte_read(FILE* in);
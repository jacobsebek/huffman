#include "exitflags.h"
#include "bitio.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

static uint8_t wbbuffer = 0;
static size_t wbufsize = 0;

static uint8_t rbbuffer = 0;
static size_t rbufsize = 0;

void bitstring_setbit(bitstring* bs, const bit b, const unsigned index)
{
    if (b != 0)
        bs->bits[index/64] |= (1 << index%64);
    else
        bs->bits[index/64] &= ~(1 << index%64);
     
}

bit bitstring_getbit(const bitstring* bs, const unsigned index)
{
    return (bs->bits[index/64] & (1 << index%64)) != 0 ? 1 : 0;
}

void bit_write(const bit b, FILE* out)
{
    wbbuffer |= ((b != 0 ? 1 : 0) << wbufsize++);

    if (wbufsize == sizeof(wbbuffer)*8)
        bit_wflush(out);

}

const byte bit_wflush(FILE* out)
{
    if (wbufsize == 0) return 0;

    fwrite(&wbbuffer, sizeof(wbbuffer), 1, out);
    if (ferror(out)) {
        fprintf(stderr, EXMSG_WRITE);
        exit(EXIT_FILE_WRITERR);
    }

    register byte tmp = wbufsize;

    wbufsize = 0;
    wbbuffer = 0; // Empty the buffer

    return tmp;
}

const bit bit_read(FILE* in)
{
    if (rbufsize == 0)
        bit_rflush(in);

    return (rbbuffer & ( 1 << (sizeof(rbbuffer)*8-(rbufsize--)) )) != 0;
}

const byte bit_rflush(FILE* in)
{
    rbbuffer = 0;
    
    fread(&rbbuffer, sizeof(rbbuffer), 1, in);
    if (ferror(in)) {
        fprintf(stderr, EXMSG_READ);
        exit(EXIT_FILE_READERR);
    }

    rbufsize = sizeof(rbbuffer)*8;
}

void bits_write(const bitstring bs, FILE* out)
{
    for (register unsigned b = 0; b < bs.size; b++)
        bit_write(bitstring_getbit(&bs, b), out);
}

const bitstring bits_read(FILE* in, const unsigned num)
{
    bitstring bs;
    for (register unsigned b = 0; b < num; b++)
        bitstring_setbit(&bs, bit_read(in), b);

    bs.size = num;
    return bs;
}

void byte_write(const byte b, FILE* out)
{
    bits_write((bitstring){ {b, 0}, 8}, out);
}

const byte byte_read(FILE* in)
{
    return (bits_read(in, 8).bits[0] & 0xFF);
}
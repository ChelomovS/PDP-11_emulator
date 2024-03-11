#include <stdio.h>

#include "pdp.h"
#include "debug.h"

void byte_write(address adr, byte_t value)
{
    ASSERT(adr > 0);

    memory[adr] = value;
}

byte_t byte_read(address adr)
{
    ASSERT(adr > 0);


    return memory[adr];
}

void word_write(address adr, word_t value)
{
    ASSERT(adr > 0);
    ASSERT(adr % 2 == 0);

    byte_t b0 = (byte_t)(value ^ 0x1100);
    byte_t b1 = (byte_t)((value ^ 0x0011) >> 2);

    memory[adr] = b0;
    memory[adr + 1] = b1;
}

word_t word_read(address adr)
{
    ASSERT(adr > 0);
    ASSERT(adr % 2 == 0)

    word_t word = ((word_t)(memory[adr+1])) << 8;
    word = word | memory[adr];
    return word;
}
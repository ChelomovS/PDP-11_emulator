#include <stdio.h>

#include "pdp.h"
#include "debug.h"

byte_t memory[MEMORY_SIZE];

void byte_write(address adr, byte_t value)
{
    memory[adr] = value;
}

byte_t byte_read(address adr)
{
    return memory[adr];
}

void word_write(address adr, word_t value)
{
    ASSERT(adr % 2 == 0);

    byte_t b0 = (byte_t)(value & 0x00FF);
    byte_t b1 = (byte_t)((value & 0xFF00) >> 8);

    memory[adr] = b0;
    memory[adr + 1] = b1;
}

word_t word_read(address adr)
{
    ASSERT(adr % 2 == 0)

    word_t word = ((word_t)(memory[adr+1])) << 8;
    word = word | memory[adr];
    return word;
}


void memory_dump(address adr, size_t dump_size)
{
    for (size_t adr_move = 0; adr_move < dump_size; adr_move ++)
    {
        printf(GREEN "%06o: %06o %04x \n" RESET, adr + adr_move, memory[adr + adr_move], memory[adr + adr_move]);
    }
}

void load_data(const char* file_name)
{
    ASSERT(file_name != NULL);

    FILE* filein = fopen(file_name, "r");
    ASSERT(filein != NULL);

    address adr = 0;
    fscanf(filein, "%hd", &adr);

    uint32_t number_of_bytes = 0;
    fscanf(filein, "%x", &number_of_bytes);
    byte_t byte = 0;

    for (size_t pass = 0; pass < number_of_bytes; pass++)
    {
        fscanf(filein, "%hhx", &byte);
        byte_write(adr, byte);
        adr++;
    }
}
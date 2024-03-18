#include <stdio.h>

#include "mem.h"
#include "debug.h"
#include "errno.h"

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
    ASSERT(adr % 2 == 0);

    FILE* file_dump = fopen("dump.txt", "w");
    for (size_t adr_move = 0; adr_move < dump_size; adr_move += 2)
    {
        fprintf(file_dump, "%06o: %06o %04x \n", adr + adr_move, word_read(adr + adr_move), word_read(adr + adr_move));
    }
}

int load_data(const char* file_name)
{
    FILE* filein = fopen(file_name, "r");
    if (filein == NULL)
    {
        printf("errno: %d \n", errno);
        perror(file_name);
        return 1;
    }

    address adr = 0;
    while (fscanf(filein, "%hd", &adr) != EOF)
    {
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
    
    return 0;
}
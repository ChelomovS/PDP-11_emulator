#include <stdio.h>

#include "errors.h"
#include "mem.h"
#include "debug.h"

byte_t memory[MEMORY_SIZE];

word_t reg[NUMBER_OF_REG]; // регистры R0 ... R7

void byte_write(address adr, byte_t value)
{
    ASSERT((int)adr <= MEMORY_SIZE);
    memory[adr] = value;
}

byte_t byte_read(address adr)
{
    ASSERT((int)adr <= MEMORY_SIZE)
    return memory[adr];
}

void word_write(address adr, word_t value)
{
    ASSERT((int)adr < MEMORY_SIZE);
    
    if (adr < 8)
    {
        fprintf(stderr, "Пишу в регистр\n");
        reg[adr] = value;
        return;
    }

    ASSERT(adr % 2 == 0);
    byte_t b0 = (byte_t)(value & 0x00FF);
    byte_t b1 = (byte_t)((value & 0xFF00) >> 8);

    memory[adr] = b0;
    memory[adr + 1] = b1;
}

word_t word_read(address adr)
{
    ASSERT((int)adr < MEMORY_SIZE);
    if (adr < 8)
    {
        fprintf(stderr, "Читаю из регистра\n");
        return reg[adr];
    }
    ASSERT((int)adr % 2 == 0)
    word_t word = (word_t)(((word_t)(memory[adr+1])) << 8);
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

enum pdp_errors load_data(const int argc, const char* file_name)
{
    if (file_name == NULL || argc < 2)
        return pdp_no_such_file;

    FILE* filein = fopen(file_name, "r");
    if (filein == NULL)
        return pdp_bad_file_for_open;
    address adr;
    while (fscanf(filein, "%hx", &adr) != EOF)
    {
        word_t number_of_bytes = 0;
        fscanf(filein, "%hx", &number_of_bytes);

        byte_t byte = 0;
        for (size_t pass = 0; pass < number_of_bytes; pass++)
        {
            fscanf(filein, "%hhx", &byte);
            byte_write(adr, byte);
            adr++;
        }
    }
    
    return pdp_ok;
}
#include <stdlib.h>
#include "commands.h"
#include "test_memory.h"
#include "errors.h"

Arg dd = {};
Arg ss = {};


byte_t memory[MEMORY_SIZE] = {};

word_t reg[NUMBER_OF_REG] = {}; // регистры R0 ... R7

static const Command cmd[] = 
{ // MASK     OPCODE   NAME       COMMAND     PARAMS
    {0170000, 0010000, "mov",     do_mov,     HAS_SS},
    {0170000, 0060000, "add",     do_add,     HAS_SS},
    {0177000, 0077000, "sob",     do_sob,     },
    {0177700, 0005200, "inc",     do_inc,     HAS_DD},
    {0177777, 0000000, "halt",    do_halt,    },
    {0000000, 0000000, "unknown", do_nothing, }
};

void run()
{
    pc = 01000;
    word_t word = 0;

    while(1)
    {
        word = word_read(pc);
        fprintf(stderr, "%06o %06o: ", pc, word);
        Command command = parse_cmd(word);
        pc += 2;
        command.do_command();
    }
}

Command parse_cmd(word_t word)
{
    Command parsed_command = {};
    for (size_t pass = 0; ; pass++)
    {
        if ((word & cmd[pass].mask) == cmd[pass].opcode)
        {
            if (cmd[pass].params == HAS_SS)
            {
                ss = get_mr(word >> 6); 
                dd = get_mr(word);                               
            }
            else 
            if (cmd[pass].params == HAS_DD)
            {
                dd = get_mr(word);
            }

            parsed_command = cmd[pass];
            break; 
        }
    }
    return parsed_command;
}

void do_mov()
{
    // значение аргумента ss пишем по адресу аргумента dd
    word_write(dd.adr, ss.value);
    fprintf(stderr, "mov \n");
}

void do_add()
{
    fprintf(stderr, "add \n");
    // сумму значений аргументов ss и dd пишем по адресу аргумента dd
    word_write(dd.adr, ss.value + dd.value);
}


void do_sob()
{
    fprintf(stdout, "sob \n");
}

void do_inc()
{
    fprintf(stdout, "inc \n");
}

void do_halt()
{    
    fprintf(stdout, "halt \n");
    reg_dump();
    exit(0);
}

void do_nothing()
{
    fprintf(stdout, "unknown \n");
}

Arg get_mr(word_t word)
{
    Arg res = {};

    word_t reg_number = word & 7;      // номер регистра, 7 - бинарная маска 111

    word_t mod = (word >> 3) & 7;      // определение моды
    switch (mod)
    {
        // мода 0, R1
        case 0:
            res.adr = (address)reg_number;        // адрес - номер регистра
            res.value = reg[reg_number];          // значение - число в регистре
            fprintf(stderr, "R%d ", reg_number);  // печать номера регистра
            break;

        // мода 1, (R1)
        case 1:
            res.adr = reg[reg_number];               // в регистре адрес
            res.value = word_read(res.adr);          // по адресу - значение
            fprintf(stderr,"(R%d) ", reg_number);    // печать обращения к регистру по адресу 
            break;

        // мода 2, (R1)+ или #3
        case 2:
            reg[reg_number] += 2;        
            res.adr = reg[reg_number];       // в регистре адрес
            res.value = word_read(res.adr);  // по адресу - значение

            // печать разной мнемоники для PC и других регистров
            if (reg_number == 7)
                fprintf(stderr, "#%ho ", word_read(reg[reg_number]));
            else
                fprintf(stderr, "(R%d) ", reg_number);
            break;
        // мода 3
        case 3:
            res.adr = reg[reg_number];
            res.value = word_read(res.adr);      // добавилось еще одно разыменование
            reg[reg_number] += 2;
            break;

        // мода 4
        case 4:
            if(0 <= reg_number && reg_number <= 5)  
                reg[reg_number] -= 1;
            if(6 <= reg_number && reg_number <= 7)
                reg[reg_number] -= 2;

            res.adr = reg[reg_number];
            res.value = word_read(res.adr);
            break;
        // мода 5
        case 5:
            reg[reg_number] -= 2;
            res.adr = reg[reg_number];
            res.adr = word_read(res.adr);
            res.value = word_read(res.adr);      
            break;
        default:
            fprintf(stderr, "Mode %d not implemented yet!\n", mod);
            exit(1);
    }

    return res;
}

void reg_dump()
{
    for (size_t reg_number = 0; reg_number < NUMBER_OF_REG; reg_number++)
    {
        printf("reg[%zu] = %.6ho \n", reg_number, reg[reg_number]);
    }
}

void test_mode1_toreg()
{
    // setup
    reg[3] = 12;    // dd
    reg[5] = 0200;  // ss
    word_write(0200, 34);
    reg_dump();
    Command cmd = parse_cmd(0011503);

    word_t  expected_ss_value = 34;
    address expected_ss_adr   = 0200;
    word_t  expected_dd_value = 12;
    address expected_dd_adr   = 3;
    
    cmd.do_command();
    reg_dump();
    fprintf(stderr, "Test mode 1: reg \n");
    fprintf(stderr, "expected ss.value = %hx, result ss.value = %hx \n", 
                                                expected_ss_value, ss.value);
    fprintf(stderr, "expected ss.adr = %hx, result ss.adr = %hx \n", 
                                                expected_ss_adr, ss.adr);
    fprintf(stderr, "expected dd.value = %hx, result dd.value = %hx \n", 
                                                expected_dd_value, dd.value);
    fprintf(stderr, "expected dd.adr = %hx, result dd.adr = %hx \n", 
                                                expected_dd_adr, dd.value);

    ASSERT(expected_ss_value == ss.value);
    ASSERT(expected_ss_adr   == ss.adr);
    ASSERT(expected_dd_value == dd.value);
    ASSERT(expected_dd_adr   == dd.adr);

    word_t expected_reg_3 = 34;
    word_t expected_reg_5 = 0200;

    fprintf(stderr, "expected reg[3] = %hx, result reg[3] = %hx \n", 
                                                expected_reg_3, reg[3]);
    fprintf(stderr, "expected reg[5] = %hx, result ss.value = %hx \n", 
                                                expected_reg_5, reg[5]);

    ASSERT(expected_reg_3 == reg[3]);

    // проверяем, что значение регистра не изменилось
    ASSERT(expected_reg_5 == reg[5]);

    reg[3] = 0;
    reg[5] = 0;
    printf("Test mode 1: reg - OK \n");
}

void test_mode0()
{
    fprintf(stderr, "Test mode 0: \n");
    reg[3] = 12;    // dd
    reg[5] = 34;    // ss
    Command cmd = parse_cmd(0010503);
    cmd.do_command();
    fprintf(stderr, "%hx\n", dd.value);
    fprintf(stderr, "%hx\n", dd.adr);
    fprintf(stderr, "%hx\n", ss.value);
    fprintf(stderr, "%hx\n", ss.adr);   
    
    ASSERT(ss.value == 34);
    ASSERT(ss.adr == 5);
    ASSERT(dd.value == 12);
    ASSERT(dd.adr == 3);
    printf(">>>>Test mode 0 - OK<<<<\n");

    reg[3] = 0;
    reg[5] = 0;
}

void test_mov()
{
    fprintf(stderr, "Test mov\n");
    reg[3] = 12;    // dd
    reg[5] = 34;    // ss
    Command cmd = parse_cmd(0010503);
    cmd.do_command();
    ASSERT(reg[3] = 34);
    ASSERT(reg[5] = 34);
    printf("Test mov - OK\n");
    reg[3] = 0;
    reg[5] = 0;
}


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
        reg[adr] = value;
        return;
    }

    ASSERT(adr % 2 == 0);
    byte_t b0 = (byte_t)((value));
    byte_t b1 = (byte_t)((value) >> 8);

    memory[adr] = b0;
    memory[adr + 1] = b1;
}

word_t word_read(address adr)
{
    ASSERT((int)adr < MEMORY_SIZE);
    if (adr < 8)
    {
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
        fprintf(file_dump, "%06ho: %06ho %04hx \n", adr + adr_move, word_read(adr + adr_move), word_read(adr + adr_move));
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
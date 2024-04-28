#include <stdlib.h>
#include "commands.h"
#include "mem.h"
#include "test_memory.h"

Arg dd = {};
Arg ss = {};

word_t reg[NUMBER_OF_REG];

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
    fprintf(stderr, "mov \n");
    // значение аргумента ss пишем по адресу аргумента dd
    word_write(dd.adr, ss.value);
}

void do_add()
{
    fprintf(stderr, "add \n");
    // сумму значений аргументов ss и dd пишем по адресу аргумента dd
    word_write(dd.adr, ss.value + dd.value);
}


void do_sob()
{
    fprintf(stderr, "sob \n");
}

void do_inc()
{
    fprintf(stderr, "inc \n");
}

void do_halt()
{
    memory_dump(0, 2000);
    reg_dump();
    fprintf(stderr, "halt \n");
    exit(0);
}

void do_nothing()
{
    fprintf(stderr, "unknown \n");
}

Arg get_mr(word_t word)
{
    Arg res = {};

    word_t reg_number = word & 7;      // номер регистра, 7 - бинарная маска 111
    fprintf(stderr, "%d", reg_number);
    word_t mod = (word >> 3) & 7;      // определение моды
    switch (mod)
    {
        // мода 0, R1
        case 0:
            res.adr = (address)reg_number; // адрес - номер регистра
            res.value = reg[res.adr];   // значение - число в регистре
            printf("R%d \n", res.adr);  // печать номера регистра
            break;

        // мода 1, (R1)
        case 1:
            res.adr = (address)reg[reg_number];  // в регистре адрес
            res.value = word_read(res.adr);      // по адресу - значение
            printf("(R%d) \n", reg_number);      // печать обращения к регистру по адресу 
            break;

        // мода 2, (R1)+ или #3
        case 2:
            res.adr = (address)reg[reg_number];  // в регистре адрес
            res.value = word_read(res.adr);      // по адресу - значение
            reg[reg_number] += 2;            
            // печать разной мнемоники для PC и других регистров
            if (reg_number == 7)
                printf("#%o \n", res.value);
            else
                printf("(R%d) \n", reg_number);
            break;
        // мода 3
        case 3:
            res.adr = word_read(reg[reg_number]);
            res.value = word_read(res.adr);   // добавилось еще одно разыменование
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
        printf("reg[%zu] = %.6o \n", reg_number, reg[reg_number]);
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
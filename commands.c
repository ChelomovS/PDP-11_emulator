#include "commands.h"

#include <stdlib.h>
#include <stdbool.h>

#include "test.h"
#include "errors.h"

Arg dd = {};
Arg ss = {};
Arg nn = {};

Arg r  = {};
Arg b  = {};

char xx = 0;

short flag_N = 0;
short flag_Z = 0;
short flag_V = 0;
short flag_C = 0;

byte_t memory[MEMORY_SIZE] = {};

word_t reg[NUMBER_OF_REG] = {}; // регистры R0 ... R7

static const Command cmd[] = 
{ // MASK     OPCODE   NAME       COMMAND     PARAMS
    {0070000, 0010000, "mov",     do_mov,     HAS_SS | HAS_DD | HAS_B},
    {0170000, 0060000, "add",     do_add,     HAS_SS | HAS_DD},
    {0177000, 0077000, "sob",     do_sob,     HAS_NN | HAS_R},
    {0177000, 0004000, "jsr",     do_jsr,     HAS_R | HAS_DD},
    {0177770, 0000200, "rts",     do_rts,     HAS_RL},
    {0177700, 0005000, "clr",     do_clr,     HAS_DD},
    {0177700, 0001400, "beq",     do_beq,     HAS_XX},
    {0177000, 0100000, "bpl",     do_bpl,     HAS_XX},
    {0017700, 0005700, "tst",     do_tst,     HAS_DD | HAS_B},
    {0177777, 0000000, "halt",    do_halt,    NO_PARAMS},
    {0177400, 0000400, "br",      do_br,      HAS_XX},
    {0000000, 0000000, "unknown", do_nothing, NO_PARAMS}
};

enum pdp_errors run()
{
    pc = 01000;
    word_t word = 0;

    while(1)
    {
        word = word_read(pc);
  
        if (pc < 0)
            return programm_counter_negative;

        fprintf(stderr, "%06o %06o: ", pc, word);
        pc += 2;           
        Command command = parse_cmd(word);
  
        command.do_command();
        fprintf(stderr, "\n");
    }
}

Command parse_cmd(word_t word)
{
    Command parsed_command = {};
    for (size_t pass = 0; ; pass++)
    {
        if ((word & cmd[pass].mask) == cmd[pass].opcode)
        {
            fprintf(stderr,"%s ", cmd[pass].name);
            if ((cmd[pass].params & HAS_B) == HAS_B)
            {
                b = get_b(word >> 15);
            }
            if ((cmd[pass].params & HAS_NN) == HAS_NN)
            {
                nn = get_nn(word);
            }
            if ((cmd[pass].params & HAS_R) == HAS_R)
            {
                r = get_r(word >> 6);
            }
            if ((cmd[pass].params & HAS_SS) == HAS_SS)
            {
                ss = get_mr(word >> 6);                       
            }
            if ((cmd[pass].params & HAS_DD) == HAS_DD)
            {               
                dd = get_mr(word);
            }
            if ((cmd[pass].params & HAS_XX) == HAS_XX)
            {
                xx = (char)get_xx(word);
            }
            parsed_command = cmd[pass];
            break; 
        }
    }
    return parsed_command;
}

void do_mov()
{    
    if (b.value == 0)
    {
        // значение аргумента ss пишем по адресу аргумента dd
        word_write(dd.adr, ss.value);
    }
    else if (b.value == 1)
    {
        byte_write(dd.adr, (byte_t)ss.value);
    }

    if (dd.adr == odata)
    {
        printf("%c", ss.value);
    }

    set_NZVC(ss.value);
    flag_V = 0;
}

void do_add()
{
    // сумму значений аргументов ss и dd пишем по адресу аргумента dd
    word_write(dd.adr, ss.value + dd.value);
    set_NZVC(ss.value + dd.value);
}

void do_clr()
{
    word_write(dd.adr, 0);
}

void do_br()
{
    pc = (adress)(pc + 2 * xx);
    fprintf(stderr, "br %ho", pc);
}

void do_beq()
{
    if (flag_Z == 1)
        pc = (adress)(pc + 2 * xx);
    fprintf(stderr, "%ho", pc);
}

void do_bpl()
{
    if (flag_N == 0)
        pc = (adress)(pc + 2 * xx);
    fprintf(stderr, "%ho", pc);
}

void do_tst()
{
    set_NZVC(dd.value);
    flag_V = 0;
    flag_C = 0;
}

void do_sob()
{
    reg[r.value]--; 
    if (reg[r.value] > 0)
        pc = pc - 2 * nn.value;
       
    fprintf(stderr, "sob R%d %ho\n", r.value, pc);
}

void do_jsr()
{
    sp -= 2;
    word_write(sp, reg[r.value]);
    reg[r.value] = pc;
    pc = dd.adr;
    fprintf(stderr, "R%d %ho", r.value, pc);
}

void do_rts()
{
    pc = reg[r.value];
    reg[r.value] = word_read(sp);
    sp += 2;
    fprintf(stderr, "%ho", pc);
}

void do_halt()
{    
    reg_dump();
    exit(0);
}

void do_nothing()
{
    ASSERT(0 && ":(");
}

Arg get_nn(word_t word)
{
    Arg res = {};
    res.value = word & 7;
    return res;
}

Arg get_r(word_t word)
{
    Arg res = {};
    res.value = word & 7;
    return res;
}

Arg get_b(word_t word)
{
    Arg res = {};
    res.value = word & 7;
    return res;
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
            res.adr = (adress)reg_number;        // адрес - номер регистра
            if (b.value == 1)
                res.value = byte_read(res.adr);
            else if (b.value == 0)
                res.value = word_read(res.adr);    // значение - число в регистре
            fprintf(stderr, "R%d ", reg_number);      // печать номера регистра
            break;

        // мода 1, (R1)
        case 1:
            res.adr = reg[reg_number];               // в регистре адрес
            if (b.value == 1)
                res.value = byte_read(res.adr);  
            else if (b.value == 0)
                res.value = word_read(res.adr);    // по адресу - значение

            fprintf(stderr,"(R%d) ", reg_number);    // печать обращения к регистру по адресу 
            break;

        // мода 2, (R1)+ или #3
        case 2:
            res.adr = reg[reg_number];       // в регистре адрес
            if (b.value == 1)
                res.value = byte_read(res.adr);
            else if (b.value == 0)
                res.value = word_read(res.adr);  // по адресу - значение
            
            // печать разной мнемоники для PC и других регистров
            if (reg_number == 7)
                fprintf(stderr, "#%ho ", res.value);
            else
                fprintf(stderr, "(R%d)+ ", reg_number);
        
            if (b.value == 0)
                reg[reg_number] += 2;
            else if ((b.value == 1) && (reg_number >= 0) && (reg_number <= 5))
                    reg[reg_number] += 1;
            else reg[reg_number] += 2;

            break;
        // мода 3
        case 3:
            res.adr = word_read(reg[reg_number]);            
            if (b.value == 1)
                res.value = byte_read(res.adr);
            else if (b.value == 0)
                res.value = word_read(res.adr); // добавилось еще одно разыменование
            reg[reg_number] += 2;
            break;
        // мода 4
        case 4:
            if(0 <= reg_number && reg_number <= 5)
            {   
                if (b.value == 0)
                    reg[reg_number] -= 2;
                else 
                    reg[reg_number] -= 1;
            }
            else if (6 <= reg_number && reg_number <= 7)
                reg[reg_number] -= 2;

            res.adr = reg[reg_number];
            res.value = word_read(res.adr);
            fprintf(stderr, " -(R%d)", reg_number);
            break;
        // мода 5
        case 5:
            reg[reg_number] -= 2;
            res.adr = reg[reg_number];
            res.adr = word_read(res.adr);
            res.value = word_read(res.adr);      
            break;
        // мода 6
        case 6:
        {
            word_t move = word_read(pc);
            pc += 2;
            res.adr = reg[reg_number];
            res.adr += move;
            if (b.value == 0)
                res.value = word_read(res.adr);
            else if (b.value == 1)
                res.value = byte_read(res.adr);
            break;
        }
        // мода 7
        case 7:
        {
            word_t move = word_read(pc);
            pc += 2;
            res.adr = reg[reg_number];
            res.adr += move;
            res.adr = word_read(res.adr);
            if (b.value == 0)
                res.value = word_read(res.adr);
            else if (b.value == 1)
                res.value = byte_read(res.adr);
            break;
        }
        default:
            fprintf(stderr, "Mode %d not implemented yet!\n", mod);
            exit(1);
    }

    return res;
}

char get_xx(word_t word)
{
    return (char)(word & 255);
}

void set_NZVC(word_t word)
{
    if (word == 0)
        flag_Z = 1;
    if (b.value == 1)
    {
        flag_N = (word >> 7) == 1;
        flag_C = (word >> 8) == 1;
    }
    else if (b.value == 0)
    {
        flag_N = (word >> 15) == 1;
        flag_C = (word >> 16) == 1;
    }
}

void reg_dump()
{
    printf("\n\n");
    for (size_t reg_number = 0; reg_number < NUMBER_OF_REG; reg_number++)
    {
        printf("reg[%zu] = %.6ho \n", reg_number, reg[reg_number]);
    }
}

void byte_write(adress adr, byte_t value)
{
    if (adr < 8)
    {
        if (value < 0)
            reg[adr] = (word_t)(((word_t)value) | 0xFF00);
        if (value >= 0)
            reg[adr] = (word_t)(((word_t)value) & 0x00FF);
    }
    else
    {
        ASSERT((int)adr <= MEMORY_SIZE);
        memory[adr] = value;
    }
}

byte_t byte_read(adress adr)
{
    ASSERT((int)adr <= MEMORY_SIZE);    
    if ((int)adr < 8)
        return (byte_t)reg[adr];

    return memory[adr];
}

void word_write(adress adr, word_t value)
{
    ASSERT((int)adr <= MEMORY_SIZE);

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

word_t word_read(adress adr)
{
    ASSERT((int)adr <= MEMORY_SIZE);
    if (adr < 8)
    {
        return reg[adr];
    }

    ASSERT((int)adr % 2 == 0)
    word_t word = (word_t)(((word_t)(memory[adr+1])) << 8);
    word = word | memory[adr];
    return word;
}

void memory_dump(adress adr, size_t dump_size)
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
    adress adr;
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

void print_NZVC()
{
    fprintf(stderr, "N: %d", flag_N);
    fprintf(stderr, "Z: %d", flag_Z);
    fprintf(stderr, "V: %d", flag_V);
    fprintf(stderr, "C: %d", flag_C);
}
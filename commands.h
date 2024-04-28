#ifndef COMMANDS_H
#define COMMANDS_H

#include "mem.h" 

#define pc reg[7] // programm counter

#define NO_PARAMS 0
#define HAS_DD    1
#define HAS_SS    2

typedef struct 
{
    word_t mask;
    word_t opcode;
    char* name;
    void (*do_command)(void);
    char params;
} Command;

typedef struct {
    word_t value; // значение аргумента
    address adr;  // адрес аргумента
} Arg;


void run();

void do_mov();
void do_add();
void do_sob();
void do_inc();
void do_halt();
void do_nothing();
void reg_dump();
Command parse_cmd(word_t word); 
Arg get_mr(word_t word);


void test_mode1_toreg();
void test_mode0();
void test_mov();


#endif // COMMANDS_H
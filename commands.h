#ifndef COMMANDS_H
#define COMMANDS_H

#include "stdint.h"
#include <stdlib.h>

typedef uint8_t byte_t;
typedef uint16_t word_t;
typedef uint16_t adress;

#define MEMORY_SIZE (64 * 1024)
#define NUMBER_OF_REG 8

extern byte_t memory[MEMORY_SIZE];
extern word_t reg[NUMBER_OF_REG];

#define sp reg[6]
#define pc reg[7] // programm counter

#define ostat 0177564
#define odata 0177566

#define NO_PARAMS 0
#define HAS_DD    1
#define HAS_SS    2
#define HAS_NN    4
#define HAS_XX    8
#define HAS_R     16
#define HAS_B     32
#define HAS_RL    64

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
    adress adr;  // адрес аргумента
} Arg;

extern Arg ss, dd, nn, r, b;
extern short flag_N, flag_Z, flag_V, flag_C;
enum pdp_errors run();

void do_mov();
void do_add();
void do_clr();
void do_br();
void do_beq();
void do_bpl();
void do_tst();
void do_sob();
void do_inc();
void do_jsr();
void do_rts();
void do_halt();
void do_nothing();
void set_NZVC(word_t word);
void reg_dump();

Command parse_cmd(word_t word); 
Arg get_nn(word_t word);
Arg get_r(word_t word);
Arg get_b(word_t word);
Arg get_mr(word_t word);
char get_xx(word_t word);

void byte_write(adress adr, byte_t value); // Запись байта value по адресу address
byte_t byte_read(adress adr); // Чтение байта по адресу address

void word_write(adress adr, word_t value); // Запись слова word по адресу address
word_t word_read(adress adr); // Чтение слова по адресу address

enum pdp_errors load_data(const int argc, const char* file_name); // Загрузка данных из файла в память программы
void memory_dump(adress adr, size_t dump_size); // Дамп массива памяти с адреса adr размером size

void print_NZVC();


#endif // COMMANDS_H
#ifndef COMMANDS_H
#define COMMANDS_H
#include "stdint.h"
typedef uint8_t byte_t;
typedef uint16_t word_t;
typedef uint16_t address;

#define MEMORY_SIZE (64 * 1024)
#define NUMBER_OF_REG 8

extern byte_t memory[MEMORY_SIZE];
extern word_t reg[NUMBER_OF_REG];

// #include "mem.h" 

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

extern Arg ss, dd;

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

void byte_write(address adr, byte_t value); // Запись байта value по адресу address
byte_t byte_read(address adr); // Чтение байта по адресу address

void word_write(address adr, word_t value); // Запись слова word по адресу address
word_t word_read(address adr); // Чтение слова по адресу address

enum pdp_errors load_data(const int argc, const char* file_name); // Загрузка данных из файла в память программы
void memory_dump(address adr, size_t dump_size); // Дамп массива памяти с адреса adr размером size

void test_mode1_toreg();
void test_mode0();
void test_mov();


#endif // COMMANDS_H
#ifndef MEM_H
#define MEM_H

#include <stdio.h>
#include <stdint.h>

#include "errors.h"

#define MEMORY_SIZE (64 * 1024)
#define NUMBER_OF_REG 8

typedef uint8_t byte_t;
typedef uint16_t word_t;
typedef uint16_t address;

extern byte_t memory[MEMORY_SIZE];
extern word_t reg[NUMBER_OF_REG];

void byte_write(address adr, byte_t value); // Запись байта value по адресу address
byte_t byte_read(address adr); // Чтение байта по адресу address

void word_write(address adr, word_t value); // Запись слова word по адресу address
word_t word_read(address adr); // Чтение слова по адресу address

enum pdp_errors load_data(const int argc, const char* file_name); // Загрузка данных из файла в память программы
void memory_dump(address adr, size_t dump_size); // Дамп массива памяти с адреса adr размером size

#endif // MEM_H
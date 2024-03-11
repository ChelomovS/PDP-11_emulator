#ifndef PDP_H
#define PDP_H

#include <stdio.h>
#include <stdint.h>

#define MEMORY_SIZE (64 * 1024)

typedef uint8_t byte_t;
typedef uint16_t word_t;
typedef uint16_t address;

byte_t memory[MEMORY_SIZE];//FIXME - extern

void byte_write(address adr, byte_t value); // Запись байта value по адресу address
byte_t byte_read(address adr); // Чтение байта по адресу address

void word_write(address adr, word_t value); // Запись слова word по адресу address
word_t word_read(address adr); // Чтение слова по адресу address

#endif // PDP_H
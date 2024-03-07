#include <stdio.h>

#include "test.h"
#include "debug.h"

void test1() // Записать байт, прочитать байт
{
    address adr = 2; 
    byte_t b0 = 0x0a;

    byte_write(adr, b0);

    byte_t byte_res = byte_read(adr);

    fprintf(stderr, "%02hhx = %02hhx \n", b0, byte_res);

    ASSERT(b0 == byte_res);
}

void test2() // Записать два байта, прочитать одно слово
{
    address adr = 4;
    byte_t b1 = 0x0b;
    byte_t b0 = 0x0a;
    word_t word_exp = 0x0b0a;

    byte_write(adr, b0);
    byte_write(adr + 1, b1);

    word_t word_res = word_read(adr);
    fprintf(stderr, "%04hx = %04hx \n", word_res, word_exp);

    ASSERT(word_res == word_exp);
}


void test_all()
{
    test1();
    test2();
}
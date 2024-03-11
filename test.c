#include <stdio.h>

#include "test.h"
#include "debug.h"

void test1() // Записать байт, прочитать байт
{
    address adr = 2; 
    byte_t byte_exp = 0x0a;

    byte_write(adr, byte_exp);

    byte_t byte_res = byte_read(adr);

    fprintf(stderr, "-------------------------------------------------------------- \n");
    fprintf(stderr, "Test 1: write one bytes, read one byte \n");
    fprintf(stderr, ">>> byte expected: %02hhx, byte result: %02hhx \n",
                                                                byte_exp, byte_res);

    ASSERT(byte_exp == byte_res);
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

    fprintf(stderr, "-------------------------------------------------------------- \n");
    fprintf(stderr, "Test 2: write two bytes, read one word \n");
    fprintf(stderr, ">>> word expected %04hx, word result: %04hx \n",
                                                                word_exp, word_res);

    ASSERT(word_exp == word_res);
}

void test3() // Записать слово, прочитать слово 
{
    address adr = 6;
    word_t word_exp = 0xaabb;

    word_write(adr, word_exp);

    word_t word_res = word_read(adr);

    fprintf(stderr, "-------------------------------------------------------------- \n");
    fprintf(stderr, "Test 3: write one word, read one word \n");
    fprintf(stderr, ">>> word expected %04hx, word result %04hx \n", 
                                                                word_exp, word_res);

    ASSERT(word_res == word_exp);
}

void test4() // Записать слово, прочитать два байта
{
    address adr = 8; 
    word_t word_exp = 0xaabb;
    word_write(adr, word_exp);

    byte_t first_byte_exp = 0xbb;
    byte_t second_byte_exp = 0xaa;

    byte_t first_byte_res = byte_read(adr);
    byte_t second_byte_res = byte_read(adr + 1);

    fprintf(stderr, "-------------------------------------------------------------- \n");
    fprintf(stderr, "Test 4: write one word, read one word \n");
    fprintf(stderr, ">>> first byte expected %04hhx, first byte result %04hhx \n",
                                                    first_byte_exp, first_byte_res);
    fprintf(stderr, ">>> first byte expected %04hhx, first byte result %04hhx \n",
                                                    second_byte_exp, second_byte_res);

    ASSERT(first_byte_exp == first_byte_res);
    ASSERT(second_byte_exp == second_byte_res);

}

void test_memory()
{
    test1();
    test2();
    test3();
    test4();
}
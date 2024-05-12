#include <stdio.h>

#include "test.h"
#include "string.h"
#include "debug.h"

void test_memory_1() // Записать байт, прочитать байт
{
    adress adr = 10; 
    byte_t byte_exp = 0xba;

    byte_write(adr, byte_exp);

    byte_t byte_res = byte_read(adr);

    fprintf(stderr, "-------------------------------------------------------------- \n");
    fprintf(stderr, "Test 1: write one bytes, read one byte \n");
    fprintf(stderr, ">>> byte expected: %02hx, byte result: %02hx \n",
                                                                byte_exp, byte_res);

    ASSERT(byte_exp == byte_res);
}

void test_memory_2() // Записать два байта, прочитать одно слово
{
    adress adr = 10;
    byte_t b1 = 0x0b;
    byte_t b0 = 0x0a;
    word_t word_exp = 0x0b0a;

    byte_write(adr, b0);
    byte_write(adr + 1, b1);

    word_t word_res = word_read(adr);

    fprintf(stderr, "-------------------------------------------------------------- \n");
    fprintf(stderr, "Test 2: write two bytes, read one word \n");
    fprintf(stderr, ">>> word expected %04x, word result: %04x \n",
                                                                word_exp, word_res);

    ASSERT(word_exp == word_res);
}

void test_memory_3() // Записать слово, прочитать слово 
{
    adress adr = 10;
    word_t word_exp = 0xaabb;

    word_write(adr, word_exp);

    word_t word_res = word_read(adr);

    fprintf(stderr, "-------------------------------------------------------------- \n");
    fprintf(stderr, "Test 3: write one word, read one word \n");
    fprintf(stderr, ">>> word expected %04x, word result %04x \n", 
                                                                word_exp, word_res);

    ASSERT(word_res == word_exp);
}

void test_memory_4() // Записать слово, прочитать два байта
{
    adress adr = 10; 
    word_t word_exp = 0xffba;
    word_write(adr, word_exp);

    byte_t first_byte_exp = 0xba;
    byte_t second_byte_exp = 0xff;

    byte_t first_byte_res = byte_read(adr);
    byte_t second_byte_res = byte_read(adr + 1);

    fprintf(stderr, "-------------------------------------------------------------- \n");
    fprintf(stderr, "Test 4: write one word, read one word \n");
    fprintf(stderr, ">>> first byte expected %02hx, first byte result %02hx \n",
                                                    first_byte_exp, first_byte_res);
    fprintf(stderr, ">>> first byte expected %02hx, first byte result %02hx \n",
                                                    second_byte_exp, second_byte_res);

    ASSERT(first_byte_exp == first_byte_res);
    ASSERT(second_byte_exp == second_byte_res);
}

void test_memory()
{
    test_memory_1();
    test_memory_2();
    test_memory_3();
    test_memory_4();
    printf("Test memory - OK\n");
}


void test_mode1_toreg()
{
    // setup
    reg[3] = 12;    // dd
    reg[5] = 0200;  // ss
    word_write(0200, 34);
    Command cmd = parse_cmd(0011503);

    word_t expected_ss_value = 34;
    adress expected_ss_adr   = 0200;
    word_t expected_dd_value = 12;
    adress expected_dd_adr   = 3;
    
    cmd.do_command();
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
    printf("Test mode 1 - OK\n");
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
    printf("Test mode 0 - OK\n");

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
    printf("\n\n");
}

#include <stdio.h>

#include "commands.h"
#include "test.h"
#include "errors.h"

int main(const int argc, const char** argv)
{
    test_memory();
    test_mode1_toreg();
    test_mode0();
    test_mov();

    enum pdp_errors error = pdp_ok;
    error = load_data(argc, argv[1]);
    if (error != pdp_ok)
    {
        errors_processing(error);
        return 1;
    }
    word_write(ostat, 0xFFFF);
    error = run();
    if (error != pdp_ok)
    {
        errors_processing(error);
        return 1;
    }
    return 0;
}
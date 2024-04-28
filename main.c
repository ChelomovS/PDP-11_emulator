#include <stdio.h>

#include "mem.h"
#include "commands.h"
#include "test_memory.h"
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
    run();

    memory_dump(0, 2000);
    run();
    return 0;
}
#include <stdio.h>

#include "mem.h"
#include "test.h"

int main(const int argc, const char** argv)
{
    load_data(argv[1]);

    memory_dump(100, 400);
    return 0;
}
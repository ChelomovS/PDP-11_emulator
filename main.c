#include <stdio.h>

#include "pdp.h"
#include "test.h"

int main(const int argc, const char** argv)
{
    load_data(argv[1]);

    memory_dump(0, 100);
    return 0;
}
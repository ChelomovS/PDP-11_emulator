#include <stdio.h>

#include "pdp.h"
#include "test.h"

int main(const int argc, const char** argv)
{
    test_memory();
    
    memory_dump(0, 100);
    return 0;
}
#include <stdio.h>
#include "errors.h"
#include "debug.h"

void errors_processing(enum pdp_errors error)
{
    switch (error)
    {
        case pdp_ok:
            fprintf(stderr, "PDP-11 is ok\n");
            return;
        case pdp_no_such_file:
            fprintf(stderr, "No such file\n");
            return;
        case pdp_bad_file_for_open:
            fprintf(stderr, "Can't open this file\n");
            return;
        case pdp_syntax_error:
            fprintf(stderr, "Syntax error\n");
            return;
        case programm_counter_negative:
            fprintf(stderr, "Negative PC\n");
            return;
        default:
            ASSERT(0 && ":(");
    }
}
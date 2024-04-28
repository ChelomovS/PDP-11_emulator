#ifndef ERRORS_H
#define ERRORS_H

#include <stdio.h>

enum pdp_errors
{
    pdp_ok                = 0,
    pdp_no_such_file      = 1,
    pdp_bad_file_for_open = 2,
    pdp_syntax_error      = 3,
};

void errors_processing(enum pdp_errors error);

#endif
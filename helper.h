#ifndef _HELPER_H
#define _HELPER_H

#include <stdio.h>

#define ASSERT(assertion, call_description)                    \
    {                                                          \
        if (!(assertion)) {                                    \
            fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__); \
            perror(call_description);                          \
            exit(1);                                           \
        }                                                      \
    }

#endif
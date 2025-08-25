/* Boolean value support in Hexlet */

#ifndef HEXLET_BOOLS_H
#define HEXLET_BOOLS_H

#if __STDC_VERSION__ < 199901L

typedef unsigned char bool;

#define TRUE 1
#define FALSE 0

#else

#include <stdbool.h>

#define TRUE true
#define FALSE false

#endif

#endif
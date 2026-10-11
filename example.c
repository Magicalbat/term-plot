
#define TERM_PLOT_IMPLEMENTATION
#include "term_plot.h"

#include <stdio.h>

int main(void) {
    _tp_termsize s0 = _tp_get_termsize();

    printf("%u x %u\n", s0.width, s0.height);

    getchar();

    _tp_termsize s1 = _tp_get_termsize();
    printf("%u x %u\n", s1.width, s1.height);

    return 0;
}


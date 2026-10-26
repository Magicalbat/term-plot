
#define TERM_PLOT_IMPLEMENTATION
#include "term_plot.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

int main(void) {
    tp_figure* fig = tp_begin(&(tp_figure_desc){
        .subplot_rows = 1,
        .subplot_cols = 2,

        .title = TP_STR8("Example Figure"),

        .char_height_offset = -1,
    });

    tp_string8 rendered = tp_render_utf8(fig, TP_NEWLINE_LF, NULL);

    tp_enable_utf8_output();
    printf("%.*s", (int)rendered.size, rendered.str);
    
    tp_end(fig);

    return 0;
}


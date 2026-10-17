
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
        .subplot_rows = 2,
        .subplot_cols = 2,

        .char_width = 120,
        .char_height = 35,

        .override_padding = true,
        .custom_padding = 2,

        .width_ratios = { 1, 2 }
    });

    tp_string8 rendered = tp_render_utf8(fig, TP_NEWLINE_LF, NULL);

    tp_enable_utf8_output();
    printf("%.*s\n", (int)rendered.size, rendered.str);
    
    tp_end(fig);

    return 0;
}


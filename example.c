
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

#define PI 3.14159265358979323

int main(void) {
    tp_figure* fig = tp_begin(&(tp_figure_desc){
        .subplot_rows = 1,
        .subplot_cols = 1,

        .title = TP_STR8("Example Figure"),

        .char_height_offset = -1,
    });

    u32 N = 64;

    {
        f64* x0s = malloc(sizeof(f64) * N);
        f64* y0s = malloc(sizeof(f64) * N);
        f64* y1s = malloc(sizeof(f64) * N);

        for (u32 i = 0; i < N; i++) {
            x0s[i] = ((f64)i / (N-1)) * 2.0 * PI;
            y0s[i] = sin(x0s[i]);
            y1s[i] = cos(x0s[i]);
        }

        tp_plot_scatter(fig, &(tp_points_desc){
            .axes_row = 0,
            .axes_col = 0,

            .num_points = N,
            .xs = x0s,
            .ys = y0s,
        });

        tp_plot_scatter(fig, &(tp_points_desc){
            .axes_row = 0,
            .axes_col = 0,

            .num_points = N,
            .xs = x0s,
            .ys = y1s,
        });

        free(x0s);
        free(y0s);
        free(y1s);
    }

    tp_string8 rendered = tp_render_utf8(fig, TP_NEWLINE_LF, NULL);

    tp_enable_utf8_output();
    printf("%.*s", (int)rendered.size, rendered.str);
    
    tp_end(fig);

    return 0;
}


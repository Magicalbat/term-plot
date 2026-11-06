
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

#define ARR_LEN(arr) (sizeof(arr) / sizeof(arr[0]))

int main(void) {
    tp_figure* fig = tp_begin(&(tp_figure_desc){
        .subplot_rows = 2,
        .subplot_cols = 1,

        .title = TP_STR8("Example Figure"),

        .char_height_offset = -1,

        //.raster_type = TP_RASTER_ASCII,
    });

    #define N 32

    f64 x0s[N] = { 0 };
    f64 y0s[N] = { 0 };

    // Top subplot
    {
        for (u32 i = 0; i < N; i++) {
            x0s[i] = ((f64)i / (N-1)) * 2.0 * PI;
        }

        for (u32 i = 0; i < N; i++) {
            y0s[i] = sin(x0s[i]);
        }

        tp_plot_lines(fig, &(tp_points_desc){
            .axes_row = 0,
            .axes_col = 0,

            .num_points = N,
            .xs = x0s,
            .ys = y0s,
        });

        for (u32 i = 0; i < N; i++) {
            y0s[i] = cos(x0s[i]);
        }

        tp_plot_lines(fig, &(tp_points_desc){
            .axes_row = 0,
            .axes_col = 0,

            .num_points = N,
            .xs = x0s,
            .ys = y0s,
        });
    }

    // Bottom subplot
    {
        tp_f64 off = 0;
        tp_f64 scale = 3.2e-7;

        for (u32 i = 0; i < 12; i++) {
            for (u32 j = 0; j < N; j++) {
                x0s[j] = (tp_f64)((i * (N + 4)) + j) * scale + off;
                y0s[j] = (tp_f64)(j);
            }

            tp_plot_lines(fig, &(tp_points_desc){
                .axes_row = 1,
                .axes_col = 0,

                .num_points = N,
                .xs = x0s,
                .ys = y0s,
            });
        }
    }

    tp_f64 xticks[] = {
        0, PI / 2.0, PI, 3.0 * PI / 2.0, 2.0 * PI,
        123456,
    };

    tp_f64 yticks[] = {
        -50, -1.0, -0.5, 0.0, 0.5, 1.0, 50.0
    };

    tp_setup_axes(fig, 0, 0, &(tp_axes_desc){
        .title = TP_STR8("Here is a subplot"),

        .flags = 0,

        .xlabel = TP_STR8("Time"),
        .ylabel = TP_STR8("Ians"),

        //.num_xticks = ARR_LEN(xticks),
        //.xticks = xticks,
        //.num_yticks = ARR_LEN(yticks),
        //.yticks = yticks,

        .desired_ytick_gap = 1,
        .desired_xtick_gap = 20,
    });

    tp_setup_axes(fig, 1, 0, &(tp_axes_desc){
        .title = TP_STR8("Another one"),

        .xlabel = TP_STR8("this is the x axis"),
        .ylabel = TP_STR8("Colors"),
    });

    tp_string8 rendered = tp_render_utf8(fig, TP_NEWLINE_LF, NULL);

    tp_enable_utf8_output();
    printf("%.*s", (int)rendered.size, rendered.str);
    
    tp_end(fig);

    return 0;
}


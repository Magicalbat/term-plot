
#define TERM_PLOT_IMPLEMENTATION
#include "term_plot.h"

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
        .subplot_cols = 2,

        .title = TP_STR8("Example Figure"),

        .char_height_offset = -1,
    });

    #define N 64

    f64 xs[N] = { 0 };
    f64 ys[N] = { 0 };

    // Top left subplot
    {
        for (u32 i = 0; i < N; i++) {
            xs[i] = ((f64)i / (N-1)) * 2.0 * PI;
            ys[i] = sin(xs[i]);
        }

        tp_plot_lines(fig, 0, 0, &(tp_points_desc){
            .num_points = N,
            .xs = xs,
            .ys = ys,

            .col = { 0, 255, 0 },

            .label = TP_STR8("sin"),
        });

        for (u32 i = 0; i < N; i++) {
            ys[i] = cos(xs[i]);
        }

        tp_plot_scatter(fig, 0, 0, &(tp_points_desc){
            .num_points = N,
            .xs = xs,
            .ys = ys,

            .label = TP_STR8("cos"),
        });

        tp_f64 xticks[] = {
            0, PI / 2.0, PI, 3.0 * PI / 2.0, 2.0 * PI,
            123456,
        };

        tp_setup_axes(fig, 0, 0, &(tp_axes_desc){
            .title = TP_STR8("Here is a subplot"),

            .flags = TP_AXES_LEGEND,

            .legend_pos_x = 0.0,
            .legend_pos_y = 1.0,

            .xlabel = TP_STR8("Time"),
            .ylabel = TP_STR8("Ians"),

            .num_xticks = ARR_LEN(xticks),
            .xticks = xticks,

            .bg_col = { 25, 25, 25 },
        });
    }

    // Bottom left subplot
    {
        tp_f64 off = 0;
        tp_f64 scale = 3.2e-7;

        for (u32 i = 0; i < 12; i++) {
            for (u32 j = 0; j < N; j++) {
                xs[j] = (tp_f64)((i * (N + 4)) + j) * scale + off;
                ys[j] = (tp_f64)(j);
            }

            tp_plot_lines(fig, 1, 0, &(tp_points_desc){
                .num_points = N,
                .xs = xs,
                .ys = ys,
            });
        }

        tp_setup_axes(fig, 1, 0, &(tp_axes_desc){
            .title = TP_STR8("Another one"),

            .xlabel = TP_STR8("this is the x axis"),
            .ylabel = TP_STR8("Colors"),
        });
    }

    // Top right subplot
    {
        for (u32 i = 0; i < N; i++) {
            xs[i] = (f64)i * (1 + (f64)rand() / (f64)RAND_MAX);
            ys[i] = (f64)i * (1 + (f64)rand() / (f64)RAND_MAX);
        }

        tp_plot_scatter(fig, 0, 1, &(tp_points_desc){
            .num_points = N,
            .xs = xs,
            .ys = ys,
        });
    }
    
    // Bottom right subplot
    {
        for (u32 i = 0; i < N; i++) {
            xs[i] = ((f64)i / (N-1)) * 2.0 * PI - PI;
            ys[i] = sinh(xs[i]);
        }

        tp_plot_lines(fig, 1, 1, &(tp_points_desc){
            .num_points = N,
            .xs = xs,
            .ys = ys,
            .label = TP_STR8("sinh"),
        });

        for (u32 i = 0; i < N; i++) {
            ys[i] = cosh(xs[i]);
        }

        tp_plot_lines(fig, 1, 1, &(tp_points_desc){
            .num_points = N,
            .xs = xs,
            .ys = ys,
            .label = TP_STR8("cosh"),
        });

        for (u32 i = 0; i < N; i++) {
            ys[i] = tanh(xs[i]);
        }

        tp_plot_lines(fig, 1, 1, &(tp_points_desc){
            .num_points = N,
            .xs = xs,
            .ys = ys,
            .label = TP_STR8("tanh"),
        });

        tp_setup_axes(fig, 1, 1, &(tp_axes_desc){
            .title = TP_STR8("Hyperbolic trig"),
            .flags = TP_AXES_LEGEND,
        });
    }
    
    tp_show_interactive_and_end(fig);

    return 0;
}


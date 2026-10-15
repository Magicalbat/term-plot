
#define TERM_PLOT_IMPLEMENTATION
#include "term_plot.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;

#define ROWS 1
#define COLS 2

#define WIDTH 80
#define HEIGHT 40

int main(void) {
    tp_figure* fig = tp_begin(&(tp_figure_desc){
        .subplot_rows = ROWS,
        .subplot_cols = COLS,

        .override_padding = true,
        .custom_padding = 2,

        .width_ratios = { 1, 3 }
    });

    tp_setup_axes(fig, 0, 0, &(tp_axes_desc){
        .flags = TP_AXES_NO_YNUMS | TP_AXES_NO_XNUMS,
    });

    _tp_fig_layout layout = { 0 };
    _tp_layout_axes(fig, &layout, (_tp_rect_u32){
        .x = 5, .y = 5,
        .w = WIDTH - 5, .h = HEIGHT - 5,
    });

    u8 raster[WIDTH * HEIGHT];
    memset(raster, '.', WIDTH * HEIGHT);

    for (u32 col = 0; col < COLS; col++) {
        for (u32 row = 0; row < ROWS; row++) {
            u32 idx = col + row * COLS;

            _tp_rect_u32 drawable = layout.drawable[idx];
            for (u32 y = drawable.y; y < drawable.y + drawable.h; y++) {
                for (u32 x = drawable.x; x < drawable.x + drawable.w; x++) {
                    raster[x + y * WIDTH] = '#';
                }
            }

            _tp_rect_u32 graphable = layout.graphable[idx];
            for (u32 y = graphable.y; y < graphable.y + graphable.h; y++) {
                for (u32 x = graphable.x; x < graphable.x + graphable.w; x++) {
                    raster[x + y * WIDTH] = '-';
                }
            }
        }
    }

    for (u32 y = 0; y < HEIGHT; y++) {
        printf("%.*s\n", WIDTH, raster + y * WIDTH);
    }

    tp_end(fig);

    return 0;
}


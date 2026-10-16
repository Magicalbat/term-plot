
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

#define ROWS 1
#define COLS 2

#define WIDTH 60
#define HEIGHT 20

int main(void) {
    tp_figure* fig = tp_begin(&(tp_figure_desc){
        .subplot_rows = ROWS,
        .subplot_cols = COLS,

        .override_padding = true,
        .custom_padding = 2,

        .width_ratios = { 1, 3 }
    });

    _tp_drawtarget dt = _tp_drawtarget_create(
        fig->arena, (_tp_dim_u32){ WIDTH, HEIGHT }
    );

    tp_enable_utf8_output();

    for (u32 y = 0; y < HEIGHT; y++) {
        for (u32 x = 0; x < WIDTH; x++) {
            u32 index = x + y * WIDTH;

            if ((index % 4) <= 1) {
                dt.codepoints[index] = (u32)'A' + index/4;
                dt.bg_cols[index] = (tp_rgb8){
                    0,
                    (u8)((f32)x / WIDTH * 255.0f),
                    (u8)((f32)y / HEIGHT * 255.0f),
                };
            } else {
                dt.codepoints[index] = 0x2500 + index/4;
                dt.fg_cols[index] = (tp_rgb8){
                    0,
                    (u8)((f32)x / WIDTH * 255.0f),
                    (u8)((f32)y / HEIGHT * 255.0f),
                };
            }
        }
    }

    u64 size = _tp_drawtarget_encode_size(&dt, TP_NEWLINE_LF, true);
    u8* encoded = _TP_PUSH_ARRAY_NZ(fig->arena, u8, size);
    _tp_drawtarget_encode(&dt, TP_NEWLINE_LF, true, encoded);
    printf("%s\n", encoded);

    // Layout testing
    #if 0
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
    #endif

    tp_end(fig);

    return 0;
}


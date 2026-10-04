
#ifndef TERM_PLOT_H_INCLUDE
#define TERM_PLOT_H_INCLUDE

#include <stdint.h>

typedef int8_t tp_i8;
typedef int16_t tp_i16;
typedef int32_t tp_i32;
typedef int64_t tp_i64;
typedef uint8_t tp_u8;
typedef uint16_t tp_u16;
typedef uint32_t tp_u32;
typedef uint64_t tp_u64;

typedef tp_i8 tp_b8;
typedef tp_i32 tp_b32;

typedef float tp_f32;
typedef double tp_f64;

typedef struct {
    tp_u8* str;

    // Size in bytes, **NOT** codepoints
    tp_u64 size;
} tp_string8;

// Creates a `tp_string8` from a literal cstring
#define TP_STR8(s) (tp_string8){ (tp_u8*)(s), sizeof((s)) - 1 }

tp_string8 tp_str8_from_cstr(tp_u8* cstr);

typedef struct {
    tp_f64 left, right, top, bottom;
} tp_rect64;

typedef enum {
    TP_RASTER_BRAILLE = 0,
    TP_RASTER_ASCII,
} tp_raster_type;

typedef enum {
    TP_AXES_DISPLAY_LEGEND     = 0b0000001,
    TP_AXES_DISPLAY_ORIGIN     = 0b0000010,
    TP_AXES_DISPLAY_NO_XTICKS  = 0b0000100,
    TP_AXES_DISPLAY_NO_YTICKS  = 0b0001000,
    TP_AXES_DISPLAY_NO_XNUMS   = 0b0010000,
    TP_AXES_DISPLAY_NO_YNUMS   = 0b0100000,
} tp_axes_display_flags;

typedef struct {
    // Number of characters wide
    // 0 will default to the available terminal width
    tp_u32 char_width;

    // Number of characters high
    // 0 will default to the available terminal height
    tp_u32 char_height;

    tp_raster_type raster_type;
    tp_axes_display_flags display_flags;

    tp_string8 xlabel;
    tp_string8 ylabel;
    tp_string8 title;

    // Display bounds of the axes
    // { 0, 0, 0, 0 } will default to fit the data
    tp_rect64 bounds;
} tp_axes_desc;

typedef struct {
    tp_u32 num_points;

    // `xs` can be NULL. If it is, the plot will use 0...`num_points`-1
    tp_f64* xs;
    tp_f64* ys;

    tp_string8 label;
} tp_plot_desc;

typedef struct tp_axes tp_axes;

tp_axes* tp_axes_create(const tp_axes_desc* axes_desc);
void tp_axes_destroy(tp_axes* axes);

void tp_axes_set_char_width(tp_axes* axes, tp_u32 char_width);
void tp_axes_set_char_height(tp_axes* axes, tp_u32 char_height);
void tp_axes_set_raster(tp_axes* axes, tp_raster_type raster_type);
void tp_axes_set_display_flags(tp_axes* axes, tp_axes_display_flags flags);
void tp_axes_set_xlabel(tp_axes* axes, tp_string8 xlabel);
void tp_axes_set_ylabel(tp_axes* axes, tp_string8 xlabel);
void tp_axes_set_title(tp_axes* axes, tp_string8 title);
void tp_axes_set_xlim(tp_axes* axes, tp_f64 xmin, tp_f64 xmax);
void tp_axes_set_ylim(tp_axes* axes, tp_f64 ymin, tp_f64 ymax);

void tp_axes_plot(tp_axes* axes, const tp_plot_desc* plot_desc);

void tp_axes_show(tp_axes* axes, tp_b8 interactive);
void tp_axes_show_and_destroy(tp_axes* axes, tp_b8 interactive);

#endif // TERM_PLOT_H_INCLUDE

#ifdef TERM_PLOT_IMPLEMENTATION

tp_string8 tp_str8_from_cstr(tp_u8* cstr) {
    tp_u8* start = cstr;

    for (; *cstr != 0; cstr += 1);

    return (tp_string8) {
        .str = start,
        .size = (tp_u64)(cstr - start)
    };
}

#endif // TERM_PLOT_IMPLEMENTATION


/*
MIT License

Copyright (c) 2026 Ian Dvorin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

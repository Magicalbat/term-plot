
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

typedef enum {
    TP_RASTER_DEFAULT = 0,
    TP_RASTER_BRAILLE,
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
    // Number of rows of subplots
    // If `subplot_rows` is 0, the figure will default to 1 subplot row
    tp_u32 subplot_rows;

    // Number of columns of subplots
    // If `subplot_cols` is 0, the figure will default to 1 subplot column
    tp_u32 subplot_cols;

    // Number of characters wide
    // 0 will default to the available terminal width
    // Dynamically showing the graph may result in a smaller graph if
    // `char_width` does not fit within the current terminal window
    tp_u32 char_width;

    // Number of characters high
    // 0 will default to the available terminal height
    // Dynamically showing the graph may result in a smaller graph if
    // `char_height` does not fit within the current terminal window
    tp_u32 char_height;

    tp_raster_type raster_type;

    tp_string8 title;
} tp_figure_desc;

typedef struct {
    // Number of characters wide
    // 0 will default to the available terminal width
    // Dynamically showing the graph may result in a smaller graph if
    // `char_width` does not fit within the current terminal window
    tp_u32 char_width;

    // Number of characters high
    // 0 will default to the available terminal height
    // Dynamically showing the graph may result in a smaller graph if
    // `char_height` does not fit within the current terminal window
    tp_u32 char_height;

    tp_raster_type raster_type;
    tp_axes_display_flags display_flags;

    tp_string8 xlabel;
    tp_string8 ylabel;
    tp_string8 title;

    // Minimum x bound for the axes
    // If both `xmin` and `xmax` are 0, the x bounds will fit to the data
    tp_f64 xmin;

    // Maximum x bound for the axes
    // If both `xmin` and `xmax` are 0, the x bounds will fit to the data
    tp_f64 xmax;

    // Minimum y bound for the axes
    // If both `ymin` and `ymax` are 0, the y bounds will fit to the data
    tp_f64 ymin;

    // Maximum y bound for the axes
    // If both `ymin` and `ymax` are 0, the y bounds will fit to the data
    tp_f64 ymax;
} tp_axes_desc;

typedef struct {
    // Row of the axes to plot to
    tp_u32 axes_row;
    // Column of the axes to plot to
    tp_u32 axes_col;
    
    tp_u32 num_points;

    // `xs` can be NULL. If it is, the plot will use 0...`num_points`-1
    tp_f64* xs;
    tp_f64* ys;

    tp_string8 label;
} tp_plot_desc;

typedef struct tp_figure tp_figure;

// Creates a new figure for plotting
tp_figure* tp_begin(const tp_figure_desc* fig_desc);

// Destroys any resources the figure is using
void tp_end(tp_figure* fig);

// Each of the subplot axes can only be setup once
// That is, all subsequent calls to `tp_axes_setup` of the same subplot cell
// will be ignored
void tp_axes_setup(
    tp_figure* fig, tp_u32 arow, tp_u32 acol,
    const tp_axes_desc* axes_desc
);

void tp_plot(tp_figure* fig, const tp_plot_desc* plot_desc);

// Size of the rendered utf8 stream in **bytes**
tp_u64 tp_render_utf8_size(tp_figure* fig);

// Renders the figure as a utf8 stream to the pointer `out`
// `out` must be at least the size in bytes returned by `tp_render_utf8_size`
void tp_render_utf8(tp_figure* fig, tp_u8* out);

// Prints out the current graph statically to standard out
// If `dynamic` is true, the figure will take over the console, and there will 
// be keybinds to interact with the graph
void tp_show(tp_figure* fig, tp_b8 dynamic);

// This is the same as `tp_show` followed by `tp_end`
// Shows the figure and deletes it when done
void tp_show_and_end(tp_figure* fig, tp_b8 dynamic);

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

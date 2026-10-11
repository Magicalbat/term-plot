
#ifndef TERM_PLOT_H_INCLUDE
#define TERM_PLOT_H_INCLUDE

// Note(Ian) I am not super familiar with all the C stuff for linux, so let me
// know if there is a better way to do this. I think this is required for 
// `madvise`
#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#   define _DEFAULT_SOURCE
#endif

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
    // For a figure, `TP_RASTER_DEFAULT` will default to `TP_RASTER_BRAILLE`
    // For axes, `TP_RASTER_DEFAULT` will default to the figure's setting
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

typedef enum {
    // Padding of 1 character
    TP_PADDING_NORMAL = 0,

    // No padding
    TP_PADDING_NONE,

    // Padding of 2 characters
    TP_PADDING_LARGE,
} tp_padding;

#ifndef TP_MAX_SUBPLOT_ROWS 
#   define TP_MAX_SUBPLOT_ROWS 16
#endif

#ifndef TP_MAX_SUBPLOT_COLS 
#   define TP_MAX_SUBPLOT_COLS 16
#endif

typedef struct {
    // Number of rows of subplots
    // If `subplot_rows` is 0, the figure will default to 1 subplot row
    // Must be less than or equal to `TP_MAX_SUBPLOT_ROWS`
    tp_u32 subplot_rows;

    // Number of columns of subplots
    // If `subplot_cols` is 0, the figure will default to 1 subplot column
    // Must be less than or equal to `TP_MAX_SUBPLOT_COLS`
    tp_u32 subplot_cols;

    // Number of characters wide
    // 0 will default to the available terminal width
    // Dynamically showing the graph may result in a smaller graph if
    // `char_width` does not fit within the current terminal window
    tp_u32 char_width;

    // Array of `subplot_cols` elements that give the relative proportion of 
    // each column of subplots
    // All zeros will default to uniform sizing
    // Each column gets a relative width of `width_ratios[i]/sum(width_ratios)`
    tp_f64 width_ratios[TP_MAX_SUBPLOT_COLS];

    // Array of `subplot_rows` elements that give the relative proportion of 
    // each row of subplots
    // All zeros will default to uniform sizing
    // Each row gets a relative height of `height_ratios[i]/sum(height_ratios)`
    tp_f64 height_ratios[TP_MAX_SUBPLOT_ROWS];

    // Number of characters high
    // 0 will default to the available terminal height
    // Dynamically showing the graph may result in a smaller graph if
    // `char_height` does not fit within the current terminal window
    tp_u32 char_height;

    tp_raster_type raster_type;

    // Denotes the padding between axes
    tp_padding axes_padding;

    tp_string8 title;
} tp_figure_desc;

typedef struct {
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
} tp_points_desc;

typedef struct {
    void* (*alloc_fn)(tp_u64 size, void* user_data);
    void* user_data;
} tp_allocator;

typedef enum {
    // Unix style
    TP_NEWLINE_LF = 0,

    // Windows style
    TP_NEWLINE_CRLF,
} tp_newline_style;

typedef struct tp_figure tp_figure;

// Creates a new figure for plotting
tp_figure* tp_begin(const tp_figure_desc* fig_desc);

// Destroys any resources the figure is using
void tp_end(tp_figure* fig);

// Calls made to out of bounds axes will be ignored
void tp_setup_axes(
    tp_figure* fig, tp_u32 arow, tp_u32 acol,
    const tp_axes_desc* axes_desc
);

void tp_plot_scatter(tp_figure* fig, const tp_points_desc* points);
void tp_plot_lines(tp_figure* fig, const tp_points_desc* points);

// Enables utf8 output for the current terminal 
void tp_enable_utf8_output(void);

// Renders the current figure as a utf8 stream
// If the `allocator` pointer is NULL, malloc will be used
// Only one call to the `allocator->alloc` function will be made by the 
// function, which means that you can use a linear allocator here
//
// Note: If you are printing this out to the console in any fashion, it may not
// appear correctly unless you call `tp_enable_utf8_output()`. The function
// `tp_show(...)` does this automatically. You could also of course ues the 
// ascii rasterizer to obviate the need of utf8 output support.
tp_string8 tp_render_utf8(
    tp_figure* fig, tp_newline_style newline_style,
    const tp_allocator* allocator
);

// Prints out the current graph statically to standard out
// If `dynamic` is true, the figure will take over the console, and there will 
// be keybinds to interact with the graph
void tp_show(tp_figure* fig, tp_b8 dynamic);

// This is the same as `tp_show` followed by `tp_end`
// Shows the figure and deletes it when done
void tp_show_and_end(tp_figure* fig, tp_b8 dynamic);

#endif // TERM_PLOT_H_INCLUDE

////////////////////////////////////////////////////////////////////////////////
// 
// 
// Implementation
//
// 
////////////////////////////////////////////////////////////////////////////////

#ifdef TERM_PLOT_IMPLEMENTATION

#include <stddef.h>
#include <string.h> // memset, memcpy

#ifndef TP_FATAL_ERROR
#   include <stdio.h>
#   include <stdlib.h>
#   define TP_FATAL_ERROR(msg) do { \
        fprintf(stderr, (msg)); \
        exit(1); \
    } while (0)
#endif

#ifndef TP_WARN
#   include <stdio.h>
#   define TP_WARN(msg) fprintf(stderr, (msg))
#endif

#define _TP_MIN(a, b) ((a) < (b) ? (a) : (b))
#define _TP_MAX(a, b) ((a) > (b) ? (a) : (b))
#define _TP_CLAMP(x, a, b) (_TP_MIN((b), _TP_MAX((x), (a))))

#define _TP_ALIGN_UP_POW2(n, p) (((tp_u64)(n) + ((tp_u64)(p) - 1)) & (~((tp_u64)(p) - 1)))

#define _TP_KiB(n) ((tp_u64)(n) << 10)
#define _TP_MiB(n) ((tp_u64)(n) << 20)

#define _TP_SLL_PUSH_BACK(f, l, n) ((f) == NULL ? \
    ((f) = (l) = (n)) :                       \
    ((l)->next = (n), (l) = (n)),             \
    ((n)->next = NULL))                       \

#define _TP_TRUE 1
#define _TP_FALSE 1

typedef struct {
    tp_u32 width, height;
} _tp_termsize;

////////////////////////////////////////////////////////////////////////////////
// 
// Platform dependent functionality
// 
////////////////////////////////////////////////////////////////////////////////

void* _tp_mem_reserve(tp_u64 size);
tp_b32 _tp_mem_commit(void* mem, tp_u64 size);
tp_b32 _tp_mem_decommit(void* mem, tp_u64 size);
tp_b32 _tp_mem_release(void* mem, tp_u64 size);
tp_u32 _tp_page_size(void);

_tp_termsize _tp_get_termsize(void);

static _tp_termsize _tp_def_termsize = { .width = 80, .height = 24 };

#if defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

void tp_enable_utf8_output(void) {
    SetConsoleOutputCP(CP_UTF8);
}

void* _tp_mem_reserve(tp_u64 size) {
    return VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_READWRITE);
}

tp_b32 _tp_mem_commit(void* mem, tp_u64 size) {
    void* ret = VirtualAlloc(mem, size, MEM_COMMIT, PAGE_READWRITE);
    return ret != NULL;
}

tp_b32 _tp_mem_decommit(void* mem, tp_u64 size) {
    return VirtualFree(mem, size, MEM_DECOMMIT);
}

tp_b32 _tp_mem_release(void* mem, tp_u64 size) {
    return VirtualFree(mem, size, MEM_RELEASE);
}

tp_u32 _tp_page_size(void) {
    SYSTEM_INFO si = { 0 };
    GetSystemInfo(&si);
    return si.dwPageSize;
}

_tp_termsize _tp_get_termsize(void) {
    CONSOLE_SCREEN_BUFFER_INFO console_info = { 0 };
    HANDLE stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);

    if (!GetConsoleScreenBufferInfo(
        stdout_handle, &console_info
    )) {
        TP_WARN(
            "[term-plot Warning] "
            "Failed to get terminal size, returning default"
        );

        return _tp_def_termsize;
    }

    SMALL_RECT win_rect = console_info.srWindow;

    tp_u32 width = (tp_u32)(win_rect.Right - win_rect.Left + 1);
    tp_u32 height = (tp_u32)(win_rect.Bottom - win_rect.Top + 1);

    return (_tp_termsize){
        .width = width,
        .height = height
    };
}

#elif defined(__unix__)

#include <unistd.h>
#include <locale.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

void tp_enable_utf8_output(void) {
    char* res = setlocale(LC_ALL, "C.UTF-8");

    if (res == NULL) {
        res = setlocale(LC_ALL, "en_US.UTF-8");
    }

    if (res == NULL) {
        res = setlocale(LC_ALL, "");
    }
}

void* _tp_mem_reserve(tp_u64 size) {
    void* out = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (out == MAP_FAILED) {
        return NULL;
    }
    return out;
}

tp_b32 _tp_mem_commit(void* ptr, tp_u64 size) {
    tp_i32 ret = mprotect(ptr, size, PROT_READ | PROT_WRITE);
    return ret == 0;
}

tp_b32 _tp_mem_decommit(void* ptr, tp_u64 size) {
    tp_i32 ret = mprotect(ptr, size, PROT_NONE);

    if (ret != 0) return _TP_FALSE;

    ret = madvise(ptr, size, MADV_DONTNEED);

    return ret == 0;
}

tp_b32 _tp_mem_release(void* ptr, tp_u64 size) {
    tp_i32 ret = munmap(ptr, size);
    return ret == 0;
}

tp_u32 _tp_page_size(void) {
    return (tp_u32)sysconf(_SC_PAGESIZE);
}

_tp_termsize _tp_get_termsize(void) {
    struct winsize w = { 0 };

    tp_i32 ret = ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    if (ret >= 0) {
        return (_tp_termsize) {
            .width = w.ws_col,
            .height = w.ws_row,
        };
    }

    TP_WARN(
        "[term-plot Warning] "
        "Failed to get terminal size, returning default"
    );

    return _tp_def_termsize;
}

#else

#include <stdlib.h>

void* _tp_mem_reserve(tp_u64 size) {
    return malloc(size);
}

tp_b32 _tp_mem_commit(void* mem, tp_u64 size) {
    (void)mem;
    (void)size;

    return _TP_TRUE;
}

tp_b32 _tp_mem_decommit(void* mem, tp_u64 size) {
    (void)mem;
    (void)size;

    return _TP_TRUE;
}

tp_b32 _tp_mem_release(void* mem, tp_u64 size) {
    (void)size;

    free(mem);

    return _TP_TRUE;
}

tp_u32 _tp_page_size(void) {
    return _TP_KiB(4);
}

#endif

////////////////////////////////////////////////////////////////////////////////
// 
// Arena allocator
// 
////////////////////////////////////////////////////////////////////////////////

#define _TP_ARENA_HEADER_SIZE (sizeof(_tp_arena))
#define _TP_ARENA_ALIGN (2 * sizeof(void*))

typedef struct _tp_arena {
    struct _tp_arena* current;
    struct _tp_arena* prev;

    tp_u64 reserve_size;
    tp_u64 commit_size;

    tp_u64 base_pos;
    tp_u64 pos;
    tp_u64 commit_pos;
} _tp_arena;

typedef struct {
    _tp_arena* arena;
    tp_u64 start_pos;
} _tp_arena_temp;

#define _TP_PUSH_STRUCT(arena, T) (T*)_tp_arena_push((arena), sizeof(T), _TP_FALSE)
#define _TP_PUSH_STRUCT_NZ(arena, T) (T*)_tp_arena_push((arena), sizeof(T), _TP_TRUE)
#define _TP_PUSH_ARRAY(arena, T, n) (T*)_tp_arena_push((arena), sizeof(T) * (tp_u64)(n), _TP_FALSE)
#define _TP_PUSH_ARRAY_NZ(arena, T, n) (T*)_tp_arena_push((arena), sizeof(T) * (tp_u64)(n), _TP_TRUE)

_tp_arena* _tp_arena_create(tp_u64 reserve_size, tp_u64 commit_size) {
    tp_u32 page_size = _tp_page_size();

    reserve_size = _TP_ALIGN_UP_POW2(reserve_size, page_size);
    commit_size = _TP_ALIGN_UP_POW2(commit_size, page_size);

    _tp_arena* arena = _tp_mem_reserve(reserve_size);

    if (!_tp_mem_commit(arena, commit_size)) {
        arena = NULL;
    }

    if (arena == NULL) {
        TP_FATAL_ERROR("[term-plot Error] Failed to commit memory for arena");
        return NULL;
    }

    arena->current = arena;
    arena->prev = NULL;

    arena->reserve_size = reserve_size;
    arena->commit_size = commit_size;

    arena->base_pos = 0;
    arena->pos = _TP_ARENA_HEADER_SIZE;
    arena->commit_pos = commit_size;

    return arena;
}

void _tp_arena_destroy(_tp_arena* arena) {
    _tp_arena* current = arena->current;

    while (current != NULL) {
        _tp_arena* prev = current->prev;
        _tp_mem_release(current, current->reserve_size);

        current = prev;
    }
}

tp_u64 _tp_arena_get_pos(_tp_arena* arena) {
    return arena->current->base_pos + arena->current->pos;
}

void* _tp_arena_push(_tp_arena* arena, tp_u64 size, tp_b32 non_zero) {
    void* out = NULL;

    _tp_arena* current = arena->current;

    tp_u64 pos_aligned = _TP_ALIGN_UP_POW2(current->pos, _TP_ARENA_ALIGN);
    out = (tp_u8*)current + pos_aligned;
    tp_u64 new_pos = pos_aligned + size;

    if (new_pos > current->reserve_size) {
        tp_u64 reserve_size = arena->reserve_size;
        tp_u64 commit_size = arena->commit_size;

        if (size + _TP_ARENA_HEADER_SIZE > reserve_size) {
            tp_u32 page_size = _tp_page_size();

            reserve_size = _TP_ALIGN_UP_POW2(
                size + _TP_ARENA_HEADER_SIZE, page_size
            );
        }

        _tp_arena* new_arena = _tp_arena_create(reserve_size, commit_size);
        new_arena->base_pos = current->base_pos + current->reserve_size;

        _tp_arena* prev_cur = current;
        current = new_arena;
        current->prev = prev_cur;
        arena->current = current;

        pos_aligned = _TP_ALIGN_UP_POW2(current->pos, _TP_ARENA_ALIGN);
        out = (tp_u8*)current + pos_aligned;
        new_pos = pos_aligned + size;
    }

    if (new_pos > current->commit_pos) {
        tp_u64 new_commit_pos = new_pos;
        new_commit_pos += current->commit_size - 1;
        new_commit_pos -= new_commit_pos % current->commit_size;
        new_commit_pos = _TP_MIN(new_commit_pos, current->reserve_size);

        tp_u64 commit_size = new_commit_pos - current->commit_pos;

        tp_u8* commit_pointer = (tp_u8*)current + current->commit_pos;

        if (!_tp_mem_commit(commit_pointer, commit_size)) {
            out = NULL;
        } else {
            current->commit_pos = new_commit_pos;
        }
    }

    if (out == NULL) {
        TP_FATAL_ERROR("[term-plot Error] Failed to allocate memory for arena");
        return NULL;
    }

    current->pos = new_pos;

    if (!non_zero) {
        memset(out, 0, size);
    }

    return out;
}

void _tp_arena_pop(_tp_arena* arena, tp_u64 size) {
    size = _TP_MIN(size, _tp_arena_get_pos(arena));

    _tp_arena* current = arena->current;
    while (current != NULL && size > current->pos) {
        _tp_arena* prev = current->prev;

        size -= current->pos;
        _tp_mem_release(current, current->reserve_size);

        current = prev;
    }

    arena->current = current;

    size = _TP_MIN(current->pos - _TP_ARENA_HEADER_SIZE, size);
    current->pos -= size;
}

void _tp_arena_pop_to(_tp_arena* arena, tp_u64 pos) {
    tp_u64 cur_pos = _tp_arena_get_pos(arena);
    
    pos = _TP_MIN(pos, cur_pos);

    _tp_arena_pop(arena, cur_pos - pos);
}

void _tp_arena_clear(_tp_arena* arena) {
    _tp_arena_pop_to(arena, _TP_ARENA_HEADER_SIZE);
}

_tp_arena_temp _tp_arena_temp_begin(_tp_arena* arena) {
    return (_tp_arena_temp) {
        .arena = arena,
        .start_pos = _tp_arena_get_pos(arena)
    };
}

void _tp_arena_temp_end(_tp_arena_temp temp) {
    _tp_arena_pop_to(temp.arena, temp.start_pos);
}

////////////////////////////////////////////////////////////////////////////////
// 
// String stuff
// 
////////////////////////////////////////////////////////////////////////////////

tp_string8 tp_str8_from_cstr(tp_u8* cstr) {
    tp_u8* start = cstr;

    for (; *cstr != 0; cstr += 1);

    return (tp_string8) {
        .str = start,
        .size = (tp_u64)(cstr - start)
    };
}

tp_string8 _tp_str8_copy(_tp_arena* arena, tp_string8 str) {
    if (str.size == 0) {
        return (tp_string8){ 0 };
    }

    tp_string8 out = {
        .str = _TP_PUSH_ARRAY_NZ(arena, tp_u8, str.size),
        .size = str.size,
    };

    memcpy(out.str, str.str, out.size);

    return out;
}


////////////////////////////////////////////////////////////////////////////////
// 
// Figure implementation
// 
////////////////////////////////////////////////////////////////////////////////

typedef enum {
    _TP_DRAW_NONE = 0,
    _TP_DRAW_SCATTER,
    _TP_DRAW_LINES,
} _tp_draw_cmd_kind;

typedef struct {
    tp_u32 num_points;

        tp_f64* xs;
        tp_f64* ys;

        tp_string8 label;
} _tp_draw_pointslike;

typedef struct _tp_draw_cmd {
    struct _tp_draw_cmd* next;

    _tp_draw_cmd_kind kind;

    union {
        _tp_draw_pointslike scatter;
        _tp_draw_pointslike lines;
    } info;
} _tp_draw_cmd;

typedef struct {
    tp_raster_type raster_type;
    tp_axes_display_flags display_flags;

    tp_string8 xlabel;
    tp_string8 ylabel;
    tp_string8 title;

    // If both `xmin` and `xmax` are 0, the x bounds will fit to the data
    tp_f64 xmin;
    tp_f64 xmax;

    // If both `ymin` and `ymax` are 0, the y bounds will fit to the data
    tp_f64 ymin;
    tp_f64 ymax;

    _tp_draw_cmd* first_draw;
    _tp_draw_cmd* last_draw;
} _tp_axes;

typedef struct tp_figure {
    _tp_arena* arena;

    tp_u32 subplot_rows;
    tp_u32 subplot_cols;

    tp_f64* width_fractions;
    tp_f64* height_fractions;

    // 0 will default to the available terminal width
    tp_u32 char_width;
    // 0 will default to the available terminal width
    tp_u32 char_height;

    tp_raster_type raster_type;

    tp_padding axes_padding;

    tp_string8 title;

    // Stored row-major
    _tp_axes* axes;
} tp_figure;

tp_figure* tp_begin(const tp_figure_desc* fig_desc) {
    tp_figure_desc desc = fig_desc == NULL ? (tp_figure_desc){ 0 } : *fig_desc;

    if (
        desc.subplot_rows > TP_MAX_SUBPLOT_ROWS || 
        desc.subplot_cols > TP_MAX_SUBPLOT_COLS
    ) {
        TP_WARN(
            "[term-plot Warning] "
            "Clamping figure subplot rows and columns to maximum. "
            "If this is an issue, consider defining `TP_MAX_SUBPLOT_ROWS` "
            "and/or `TP_MAX_SUBPLOT_COLS` to a higher number."
        );
    }

    _tp_arena* arena = _tp_arena_create(_TP_MiB(1), _TP_KiB(64));

    tp_figure* fig = _TP_PUSH_STRUCT(arena, tp_figure);

    *fig = (tp_figure){
        .arena = arena,

        .subplot_rows = _TP_CLAMP(desc.subplot_rows, 1, TP_MAX_SUBPLOT_ROWS),
        .subplot_cols = _TP_CLAMP(desc.subplot_cols, 1, TP_MAX_SUBPLOT_COLS),

        .char_width = desc.char_width,
        .char_height = desc.char_height,

        .raster_type = desc.raster_type == TP_RASTER_DEFAULT ? 
            TP_RASTER_BRAILLE : desc.raster_type,

        .axes_padding = desc.axes_padding,

        .title = _tp_str8_copy(arena, desc.title),
    };

    fig->axes = _TP_PUSH_ARRAY(
        arena, _tp_axes, fig->subplot_rows * fig->subplot_cols
    );

    fig->width_fractions = _TP_PUSH_ARRAY_NZ(arena, tp_f64, fig->subplot_cols);
    fig->height_fractions = _TP_PUSH_ARRAY_NZ(arena, tp_f64, fig->subplot_rows);

    tp_f64 width_ratio_sum = 0.0;
    for (tp_u32 i = 0; i < fig->subplot_cols; i++) {
        width_ratio_sum += desc.width_ratios[i];
    }

    for (tp_u32 i = 0; i < fig->subplot_cols; i++) {
        fig->width_fractions[i] = 1.0 / (
            width_ratio_sum == 0 ? (tp_f64)fig->subplot_cols : width_ratio_sum
        );
    }

    tp_f64 height_ratio_sum = 0.0;
    for (tp_u32 i = 0; i < fig->subplot_rows; i++) {
        height_ratio_sum += desc.height_ratios[i];
    }

    for (tp_u32 i = 0; i < fig->subplot_rows; i++) {
        fig->height_fractions[i] = 1.0 / (
            height_ratio_sum == 0 ? (tp_f64)fig->subplot_rows : height_ratio_sum
        );
    }

    return fig;
}

void tp_end(tp_figure* fig) {
    if (fig == NULL) { return; }

    _tp_arena_destroy(fig->arena);
}

void tp_setup_axes(
    tp_figure* fig, tp_u32 arow, tp_u32 acol,
    const tp_axes_desc* axes_desc
) {
    if (fig == NULL) { return; }
    if (arow >= fig->subplot_rows || acol >= fig->subplot_cols) {
        TP_WARN(
            "[term-plot Warning] "
            "Ignoring out of bounds call to `tp_setup_axes`."
        );

        return;
    }

    tp_axes_desc desc = axes_desc == NULL ? (tp_axes_desc){ 0 } : *axes_desc;

    _tp_axes* axes = fig->axes + (arow * fig->subplot_cols + acol);

    axes->raster_type = desc.raster_type == TP_RASTER_DEFAULT ?
        fig->raster_type : desc.raster_type;

    axes->display_flags = desc.display_flags;

    axes->xlabel = _tp_str8_copy(fig->arena, desc.xlabel);
    axes->ylabel = _tp_str8_copy(fig->arena, desc.ylabel);
    axes->title = _tp_str8_copy(fig->arena, desc.title);

    axes->xmin = desc.xmin;
    axes->xmax = desc.xmax;
    axes->ymin = desc.ymin;
    axes->ymax = desc.ymax;
}

void _tp_plot_pointslike(
    tp_figure* fig, _tp_draw_cmd_kind kind,
    const tp_points_desc* points_desc
) {
    if (fig == NULL || points_desc == NULL) { return; }
    if (
        points_desc->axes_row >= fig->subplot_rows ||
        points_desc->axes_col >= fig->subplot_cols ||
        points_desc->ys == NULL
    ) {
        TP_WARN(
            "[term-plot Warning] "
            "Ignoring invalid plotting command. Check for out of bounds axes "
            "and/or invalid data pointers"
        );

        return;
    }

    _tp_draw_pointslike pointslike = (_tp_draw_pointslike){
        .num_points = points_desc->num_points,

        .xs = _TP_PUSH_ARRAY_NZ(fig->arena, tp_f64, points_desc->num_points),
        .ys = _TP_PUSH_ARRAY_NZ(fig->arena, tp_f64, points_desc->num_points),

        .label = _tp_str8_copy(fig->arena, points_desc->label),
    };

    if (points_desc->xs == NULL) {
        for (tp_u32 i = 0; i < pointslike.num_points; i++) {
            pointslike.xs[i] = (tp_f64)i;
        }
    } else {
        memcpy(
            pointslike.xs, points_desc->xs,
            sizeof(tp_f64) * pointslike.num_points
        );
    }

    memcpy(
        pointslike.ys, points_desc->ys,
        sizeof(tp_f64) * pointslike.num_points
    );

    _tp_draw_cmd* cmd = _TP_PUSH_STRUCT_NZ(fig->arena, _tp_draw_cmd);

    *cmd = (_tp_draw_cmd){
        .kind = kind,

        // Note(Ian) this is definitely a bit of a hack, but if all the infos
        // are in the same union, than any `_tp_draw_pointslike` in that union
        // will occupy the same place in memory
        .info.scatter = pointslike
    };

    _tp_axes* axes = fig->axes + (
        points_desc->axes_row * fig->subplot_cols + points_desc->axes_col
    );

    _TP_SLL_PUSH_BACK(axes->first_draw, axes->last_draw, cmd);
}

void tp_plot_scatter(tp_figure* fig, const tp_points_desc* points) {
    _tp_plot_pointslike(fig, _TP_DRAW_SCATTER, points);
}

void tp_plot_lines(tp_figure* fig, const tp_points_desc* points) {
    _tp_plot_pointslike(fig, _TP_DRAW_LINES, points);
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

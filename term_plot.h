
#ifndef TERM_PLOT_H_INCLUDE
#define TERM_PLOT_H_INCLUDE

#ifndef TP_MAX_SUBPLOT_ROWS 
#   define TP_MAX_SUBPLOT_ROWS 8
#endif

#ifndef TP_MAX_SUBPLOT_COLS 
#   define TP_MAX_SUBPLOT_COLS 8
#endif

#define TP_MAX_SUBPLOTS (TP_MAX_SUBPLOT_ROWS * TP_MAX_SUBPLOT_COLS)

// Number of characters to display for the axis numbers
#ifndef TP_AXIS_NUM_CHARS
#   define TP_AXIS_NUM_CHARS 7
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
    tp_u8 r, g, b;
} tp_rgb8;

typedef struct {
    tp_u8* str;

    // Size in bytes, **NOT** codepoints
    tp_u64 size;
} tp_string8;

// Creates a `tp_string8` from a literal cstring
#define TP_STR8(s) (tp_string8){ (tp_u8*)(s), sizeof((s)) - 1 }

tp_string8 tp_str8_from_cstr(tp_u8* cstr);

typedef enum {
    TP_RASTER_BRAILLE = 0,
    TP_RASTER_ASCII,

    _TP_RASTER_COUNT,
} tp_raster_type;

typedef enum {
    TP_AXES_LEGEND    = 0b000001,
    TP_AXES_NO_XTICKS = 0b000010,
    TP_AXES_NO_YTICKS = 0b000100,
    TP_AXES_NO_XNUMS  = 0b001000,
    TP_AXES_NO_YNUMS  = 0b010000,
} tp_axes_flags;

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
    // Showing the graph interactively may result in a smaller graph if
    // `char_width` does not fit within the current terminal window
    tp_u32 char_width;

    // Number of characters high
    // 0 will default to the available terminal height
    // Showing the graph interactively may result in a smaller graph if
    // `char_height` does not fit within the current terminal window
    tp_u32 char_height;

    // An offset applied after calculating the base figure width
    // This is so that you can implicity graph something like (term_width - 2)
    tp_i32 char_width_offset;

    // An offset applied after calculating the base figure height
    // This is so that you can implicity graph something like (term_height - 2)
    tp_i32 char_height_offset;

    tp_string8 title;

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

    tp_raster_type raster_type;

    // If true, no color will be output
    tp_b8 monochrome;

    // Set this value to use `custom_padding`
    tp_b8 override_padding;

    // Set this value to use `custom_margin`
    tp_b8 override_margin;

    // Defines the number of characters of padding between axes
    // This will only take effect if `override_padding` is true
    // Padding by default is 1 character
    tp_u32 custom_padding;

    // Defines the number of characters around the entire figure in each 
    // direction.
    // This will only take effect if `override_margin` is true
    // Margin by default is 1 character
    tp_u32 custom_margin;
} tp_figure_desc;

typedef struct {
    tp_axes_flags flags;

    tp_string8 xlabel;
    tp_string8 ylabel;
    tp_string8 title;

    tp_rgb8 bg_col;

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

    // Number of elements in the `xticks` array
    tp_u32 num_xticks;

    // Number of elements in the `yticks` array
    tp_u32 num_yticks;

    // By default, x-axis tick marks will be drawn automatically from the axis
    // bounds, but you can override them with `xticks`. Make sure to also set
    // `num_xticks` if you are overriding this. Will be ignored if NULL
    tp_f64* xticks;

    // By default, y-axis tick marks will be drawn automatically from the axis
    // bounds, but you can override them with `yticks`. Make sure to also set
    // `num_yticks` if you are overriding this. Will be ignored if NULL
    tp_f64* yticks;

    // Desired number of characters between x ticks, defaults to 12
    // This is the *desired* gap, but the automatic ticks will prioritize using
    // round numbers over respecting your desired gap. If you need more 
    // control, just use your own ticks
    //
    // This will be ignored if `xticks` is used
    tp_u32 desired_xtick_gap;

    // Desired number of characters between y ticks, defaults to 4
    // This is the *desired* gap, but the automatic ticks will prioritize using
    // round numbers over respecting your desired gap. If you need more 
    // control, just use your own ticks
    // 
    // This will be ignored if `yticks` is used
    tp_u32 desired_ytick_gap;
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

    // If `col` is { 0, 0, 0 }, it will get overriden to the default color
    tp_rgb8 col;
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

// Clears all data associated with the figure and resets it in accordance with
// the new descriptor. For repeated figure use, this is going to be much faster
// than repeated `tp_end` and `tp_begin` calls, because `tp_reset` can reuse
// the resources from the existing figure
void tp_reset(tp_figure* fig, const tp_figure_desc* fig_desc);

// Calls made to out of bounds axes will be ignored
// This should generally only be called once per axis because subsequent calls
// technically can leak memory
void tp_setup_axes(
    tp_figure* fig, tp_u32 arow, tp_u32 acol,
    const tp_axes_desc* axes_desc
);

void tp_plot_scatter(tp_figure* fig, const tp_points_desc* points);
void tp_plot_lines(tp_figure* fig, const tp_points_desc* points);

// Enables utf8 output for the current terminal 
void tp_enable_utf8_output(void);

// Renders the current figure as a utf8 stream
// 
// If `allocator` is NULL, malloc will be used
// Only one call to the `allocator->alloc` function will be made by the 
// function, which means that you can use a linear allocator here
//
// Note: If you are printing this out to the console in any fashion, it may not
// appear correctly unless you call `tp_enable_utf8_output()`. The function
// `tp_show(...)` does this automatically. You could also of course ues the 
// ascii rasterizer to avoid utf8 as a whole
tp_string8 tp_render_utf8(
    tp_figure* fig, tp_newline_style newline_style,
    const tp_allocator* allocator
);

// Prints out the current graph to standard out
void tp_show(tp_figure* fig);

// This is the same as `tp_show` followed by `tp_end`
// Shows the figure and deletes it when done
void tp_show_and_end(tp_figure* fig);

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
#include <math.h>

#if defined(_WIN32)
#   define _TP_PLAT_WIN32
#elif defined(__unix__)
#   define _TP_PLAT_UNIX
#else
#   define _TP_PLAT_UNKNOWN
#endif

#if !defined(TP_FATAL_ERROR) || !defined(TP_WARN) || !defined(TP_SNPRINTF) || \
    defined(_TP_PLAT_UNKNOWN)
#   include <stdio.h>
#endif

#ifndef TP_FATAL_ERROR
#   include <stdlib.h>
#   define TP_FATAL_ERROR(msg) do { \
        fprintf(stderr, (msg)); \
        exit(1); \
    } while (0)
#endif

#ifndef TP_WARN
#   define TP_WARN(msg) fprintf(stderr, (msg))
#endif

#ifndef TP_SNPRINTF
#   define TP_SNPRINTF snprintf
#endif

#define _TP_ABS(x) ((x) < 0 ? -(x) : (x))
#define _TP_MIN(a, b) ((a) < (b) ? (a) : (b))
#define _TP_MAX(a, b) ((a) > (b) ? (a) : (b))
#define _TP_CLAMP(x, a, b) (_TP_MIN((b), _TP_MAX((x), (a))))

#define _TP_ALIGN_UP_POW2(n, p) (((tp_u64)(n) + ((tp_u64)(p) - 1)) & (~((tp_u64)(p) - 1)))

#define _TP_KiB(n) ((tp_u64)(n) << 10)
#define _TP_MiB(n) ((tp_u64)(n) << 20)

#define _TP_ARR_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))

#define _TP_SLL_PUSH_BACK(f, l, n) ((f) == NULL ? \
    ((f) = (l) = (n)) :                       \
    ((l)->next = (n), (l) = (n)),             \
    ((n)->next = NULL))                       \

#define _TP_TRUE 1
#define _TP_FALSE 0

typedef struct {
    tp_u32 width, height;
} _tp_dim_u32;

typedef struct {
    tp_u32 x, y, w, h;
} _tp_rect_u32;

typedef struct {
    tp_u32 start, size;
} _tp_range_u32;

static tp_u32 _tp_clz_u64(tp_u64 n) {
    if (n == 0) { return 64; }

#if defined(__clang__) || defined (__GNUC__)
    return (tp_u32)__builtin_clzll(n);
#elif defined(_MSC_VER)
    return __lzcnt64(n);
#else

    // Based on this article and stackoverflow post:
    // https://andrewlock.net/counting-the-leading-zeroes-in-a-binary-number/
    // https://stackoverflow.com/questions/10439242/count-leading-zeroes-in-an-int32/10439333#10439333
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32;

    n -= n >> 1 & 0x5555555555555555ULL;
    n = (n >> 2 & 0x3333333333333333ULL) + (n & 0x3333333333333333ULL);
    n = (n >> 4) + n & 0x0f0f0f0f0f0f0f0fULL;
    n += n >> 8;
    n += n >> 16;
    n += n >> 32;

    return 64 - (n & 0x7f);
#endif
}


////////////////////////////////////////////////////////////////////////////////
// 
// Platform dependent functionality
// 
////////////////////////////////////////////////////////////////////////////////

static void* _tp_mem_reserve(tp_u64 size);
static tp_b32 _tp_mem_commit(void* mem, tp_u64 size);
static tp_b32 _tp_mem_release(void* mem, tp_u64 size);
static tp_u32 _tp_page_size(void);

static _tp_dim_u32 _tp_get_termsize(void);

static _tp_dim_u32 _tp_def_termsize = { .width = 80, .height = 24 };

// Writes the string to stdout
static void _tp_term_write(tp_string8 str);

#if defined(_TP_PLAT_WIN32)

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

tp_b32 _tp_mem_release(void* mem, tp_u64 size) {
    return VirtualFree(mem, size, MEM_RELEASE);
}

tp_u32 _tp_page_size(void) {
    SYSTEM_INFO si = { 0 };
    GetSystemInfo(&si);
    return si.dwPageSize;
}

_tp_dim_u32 _tp_get_termsize(void) {
    CONSOLE_SCREEN_BUFFER_INFO console_info = { 0 };
    HANDLE stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);

    if (!GetConsoleScreenBufferInfo(
        stdout_handle, &console_info
    )) {
        TP_WARN(
            "[term-plot Warning] "
            "Failed to get terminal size, returning default\n"
        );

        return _tp_def_termsize;
    }

    SMALL_RECT win_rect = console_info.srWindow;

    tp_u32 width = (tp_u32)(win_rect.Right - win_rect.Left + 1);
    tp_u32 height = (tp_u32)(win_rect.Bottom - win_rect.Top + 1);

    return (_tp_dim_u32){
        .width = width,
        .height = height
    };
}

void _tp_term_write(tp_string8 str) {
    HANDLE stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);

    tp_u64 total_written = 0;

    while (total_written < str.size) {
        DWORD written = 0;

        tp_u32 to_write = (str.size - total_written) & ~((tp_u32)0);
        WriteFile(
            stdout_handle, str.str + total_written,
            to_write, &written, NULL
        );

        total_written += written;
    }
}

#elif defined(_TP_PLAT_UNIX)

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

tp_b32 _tp_mem_release(void* ptr, tp_u64 size) {
    tp_i32 ret = munmap(ptr, size);
    return ret == 0;
}

tp_u32 _tp_page_size(void) {
    return (tp_u32)sysconf(_SC_PAGESIZE);
}

_tp_dim_u32 _tp_get_termsize(void) {
    struct winsize w = { 0 };

    tp_i32 ret = ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    if (ret >= 0) {
        return (_tp_dim_u32) {
            .width = w.ws_col,
            .height = w.ws_row,
        };
    }

    TP_WARN(
        "[term-plot Warning] "
        "Failed to get terminal size, returning default\n"
    );

    return _tp_def_termsize;
}

void _tp_term_write(tp_string8 str) {
    write(STDOUT_FILENO, str.str, str.size);
}

#elif defined(_TP_PLAT_UNKNOWN)

#include <stdlib.h>
#include <locale.h>

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

_tp_dim_u32 _tp_get_termsize(void) {
    return _tp_def_termsize;
}

void _tp_term_write(tp_string8 str) {
    fwrite(str.str, 1, str.size, stdout);
}

#endif // Platform dependent

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

static _tp_arena* _tp_arena_create(tp_u64 reserve_size, tp_u64 commit_size) {
    tp_u32 page_size = _tp_page_size();

    reserve_size = _TP_ALIGN_UP_POW2(reserve_size, page_size);
    commit_size = _TP_ALIGN_UP_POW2(commit_size, page_size);

    _tp_arena* arena = _tp_mem_reserve(reserve_size);

    if (!_tp_mem_commit(arena, commit_size)) {
        arena = NULL;
    }

    if (arena == NULL) {
        TP_FATAL_ERROR("[term-plot Error] Failed to commit memory for arena\n");
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

static void _tp_arena_destroy(_tp_arena* arena) {
    _tp_arena* current = arena->current;

    while (current != NULL) {
        _tp_arena* prev = current->prev;
        _tp_mem_release(current, current->reserve_size);

        current = prev;
    }
}

static tp_u64 _tp_arena_get_pos(_tp_arena* arena) {
    return arena->current->base_pos + arena->current->pos;
}

static void* _tp_arena_push(_tp_arena* arena, tp_u64 size, tp_b32 non_zero) {
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
        TP_FATAL_ERROR("[term-plot Error] Failed to allocate memory for arena\n");
        return NULL;
    }

    current->pos = new_pos;

    if (!non_zero) {
        memset(out, 0, size);
    }

    return out;
}

static void _tp_arena_pop(_tp_arena* arena, tp_u64 size) {
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

static void _tp_arena_pop_to(_tp_arena* arena, tp_u64 pos) {
    tp_u64 cur_pos = _tp_arena_get_pos(arena);
    
    pos = _TP_MIN(pos, cur_pos);

    _tp_arena_pop(arena, cur_pos - pos);
}

static void _tp_arena_clear(_tp_arena* arena) {
    _tp_arena_pop_to(arena, _TP_ARENA_HEADER_SIZE);
}

static _tp_arena_temp _tp_arena_temp_begin(_tp_arena* arena) {
    return (_tp_arena_temp) {
        .arena = arena,
        .start_pos = _tp_arena_get_pos(arena)
    };
}

static void _tp_arena_temp_end(_tp_arena_temp temp) {
    _tp_arena_pop_to(temp.arena, temp.start_pos);
}

////////////////////////////////////////////////////////////////////////////////
// 
// Unicode Processing
// 
////////////////////////////////////////////////////////////////////////////////


static const tp_u8 _tp_utf8_lengths[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 4, 0
};

static const tp_u8 _tp_utf8_first_byte_masks[] = {
    0, 0x7f, 0x1f, 0x0f, 0x07
};

static const tp_u8 _tp_utf8_final_shifts[] = {
    0, 18, 12, 6, 0
};

// Returns the number of unicode codepoints in the utf8 string
// Will parse so long as the utf8 stream is valid
static tp_u32 _tp_utf32_from_utf8_size(tp_string8 str) {
    tp_u32 count = 0;
    tp_u32 cur_len = 1;

    // `cur_len` of 0 means an invalid utf8 stream, so just parse until then
    for (tp_u64 i = 0; i < str.size && cur_len != 0; count++, i += cur_len) {
        cur_len = _tp_utf8_lengths[str.str[i] >> 3];
    }

    return count;
}

// Will write up to `cap` codepoints to the utf32 stream in `out`
// Stops writing if it encounters an invalid portion of the utf8 stream
//
// Returns the number of codepoints written
static tp_u32 _tp_utf32_from_utf8(tp_string8 str, tp_u32* out, tp_u32 cap) {
    tp_u32 out_pos = 0;
    tp_u32 cur_len = 0;

    for (
        tp_u64 i = 0;
        i < str.size && out_pos < cap; 
        out_pos++, i += cur_len
    ) {
        tp_u8 first_byte = str.str[i];
        cur_len = _tp_utf8_lengths[first_byte >> 3];

        if (cur_len == 0 || i + cur_len > str.size) { break; }

        tp_u32 codepoint = (tp_u32)(
            first_byte & _tp_utf8_first_byte_masks[cur_len]
        );
        codepoint <<= 18;

        switch (cur_len) {
            case 4: codepoint |= ((tp_u32)str.str[i+3] & 0x3f);
            // fallthrough
            case 3: codepoint |= (((tp_u32)str.str[i+2] & 0x3f) << 6);
            // fallthrough
            case 2: codepoint |= (((tp_u32)str.str[i+1] & 0x3f) << 12);
            default: break;
        }

        codepoint >>= _tp_utf8_final_shifts[cur_len];
        out[out_pos] = codepoint;
    }

    return out_pos;
}

// Returns the size in bytes it would take to encode the utf32 stream as a 
// utf8 stream
//
// This function assumes that the input is a valid utf32 stream, which should 
// be fine because we only call this internally
static tp_u64 _tp_utf8_from_utf32_size(tp_u32* str, tp_u32 len) {
    tp_u64 size = 0;

    for (tp_u32 i = 0; i < len; i++) {
        size += 1 +
            (str[i] >= 0x80) +
            (str[i] >= 0x800) +
            (str[i] >= 0x10000);
    }

    return size;
}

// Encodes a single unicode codepoint to a utf8 stream
//
// This function assumes that the input is a valid utf32 stream, which should 
// be fine because we only call this internally
static tp_u32 _tp_utf8_encode(tp_u32 codepoint, tp_u8* out) {
    tp_u32 size = 0;

    if (codepoint <= 0x7f) {
        out[0] = (tp_u8)(codepoint & 0xff);

        size = 1;
    } else if (codepoint <= 0x7ff) {
        out[0] = (tp_u8)(0xc0 | (codepoint >> 6));
        out[1] = (tp_u8)(0x80 | (codepoint & 0x3f));

        size = 2;
    } else if (codepoint <= 0xffff) {
        out[0] = (tp_u8)(0xe0 | (codepoint >> 12));
        out[1] = (tp_u8)(0x80 | ((codepoint >> 6) & 0x3f));
        out[2] = (tp_u8)(0x80 | (codepoint & 0x3f));

        size = 3;
    } else if (codepoint <= 0x10fff) {
        out[0] = (tp_u8)(0xf0 | (codepoint >> 18));
        out[1] = (tp_u8)(0x80 | ((codepoint >> 12) & 0x3f));
        out[2] = (tp_u8)(0x80 | ((codepoint >> 6) & 0x3f));
        out[3] = (tp_u8)(0x80 | (codepoint & 0x3f));

        size = 4;
    }

    return size;
}

static tp_u32 _tp_utf8_decode(tp_u8* str, tp_u64 cap, tp_u32* out) {
    tp_u8 first_byte = str[0];
    tp_u32 len = _tp_utf8_lengths[first_byte >> 3];

    if (len == 0 || len > cap) { return 0; }

    tp_u32 codepoint = (tp_u32)(
        first_byte & _tp_utf8_first_byte_masks[len]
    );
    codepoint <<= 18;

    switch (len) {
        case 4: codepoint |= ((tp_u32)str[3] & 0x3f);
        // fallthrough
        case 3: codepoint |= (((tp_u32)str[2] & 0x3f) << 6);
        // fallthrough
        case 2: codepoint |= (((tp_u32)str[1] & 0x3f) << 12);
        default: break;
    }

    codepoint >>= _tp_utf8_final_shifts[len];
    *out = codepoint;

    return len;
}

////////////////////////////////////////////////////////////////////////////////
// 
// String functions
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

static tp_string8 _tp_str8_copy(_tp_arena* arena, tp_string8 str) {
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

// Note(Ian) I know there are a million things incorrect with assuming the
// number of codepoints is the width of a string of text, but I am not about to
// put half of HarfBuzz in the silly terminal plotting library to accurately 
// center text
static tp_u32 _tp_str8_width(tp_string8 str) {
    return _tp_utf32_from_utf8_size(str);
}

////////////////////////////////////////////////////////////////////////////////
// 
// Color functions
// 
////////////////////////////////////////////////////////////////////////////////

#define _TP_ANSI_RESET "\x1b[0m"
#define _TP_ANSI_FG_TEMP "\x1b[38;2;000;000;000m"
#define _TP_ANSI_BG_TEMP "\x1b[48;2;000;000;000m"
#define _TP_ANSI_COL_SIZE (sizeof(_TP_ANSI_FG_TEMP) - 1)

// These are from the pico-8 color palette
static const tp_rgb8 _tp_palette[] = {
    { 0x29, 0xad, 0xff },
    { 0xff, 0x00, 0x4d },
    { 0x00, 0xe4, 0x36 },
    { 0xff, 0xa3, 0x00 },
    { 0xff, 0x77, 0xa8 },
    { 0xff, 0xec, 0x27 },
    { 0x83, 0x76, 0x9c },
    { 0x00, 0x87, 0x51 },
    { 0x7e, 0x25, 0x53 },
    { 0xff, 0xf1, 0xe8 },
};

static tp_b32 _tp_rgb8_eq(tp_rgb8 a, tp_rgb8 b) {
    return a.r == b.r && a.g == b.g && a.b == b.b;
}

// Writes the ANSI escape sequence to encode a color change to `col` in `out`
// `out` must have `_TP_ANSI_COL_SIZE` bytes
static void _tp_ansi_from_rgb8(tp_rgb8 col, tp_b32 fg, tp_u8* out) {
    if (fg) {
        memcpy(out, _TP_ANSI_FG_TEMP, _TP_ANSI_COL_SIZE);
    } else {
        memcpy(out, _TP_ANSI_BG_TEMP, _TP_ANSI_COL_SIZE);
    }

    out[7] = '0' + col.r / 100;
    out[8] = '0' + (col.r / 10) % 10;
    out[9] = '0' + col.r % 10;

    out[11] = '0' + col.g / 100;
    out[12] = '0' + (col.g / 10) % 10;
    out[13] = '0' + col.g % 10;

    out[15] = '0' + col.b / 100;
    out[16] = '0' + (col.b / 10) % 10;
    out[17] = '0' + col.b % 10;
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

    tp_rgb8 col;
} _tp_info_pointslike;

typedef struct _tp_draw_cmd {
    struct _tp_draw_cmd* next;

    _tp_draw_cmd_kind kind;

    union {
        _tp_info_pointslike scatter;
        _tp_info_pointslike lines;
    } info;
} _tp_draw_cmd;

typedef struct {
    tp_axes_flags flags;

    tp_string8 xlabel;
    tp_string8 ylabel;
    tp_string8 title;

    tp_u32 palette_index;
    tp_rgb8 bg_col;

    // If both `xmin` and `xmax` are 0, the x bounds will fit to the data
    tp_f64 xmin;
    tp_f64 xmax;

    // If both `ymin` and `ymax` are 0, the y bounds will fit to the data
    tp_f64 ymin;
    tp_f64 ymax;

    tp_u32 num_xticks;
    tp_u32 num_yticks;

    tp_f64* xticks;
    tp_f64* yticks;

    tp_u32 desired_xtick_gap;
    tp_u32 desired_ytick_gap;

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

    tp_i32 width_offset;
    tp_i32 height_offset;

    tp_raster_type raster_type;

    tp_b8 monochrome;

    // Padding between axes
    tp_u32 padding;

    // Margin around entire figure
    tp_u32 margin;

    tp_string8 title;

    // Stored row-major
    _tp_axes* axes;
} tp_figure;

static tp_figure* _tp_fig_init(
    _tp_arena* arena, const tp_figure_desc* fig_desc
) {
    tp_figure_desc desc = fig_desc == NULL ? (tp_figure_desc){ 0 } : *fig_desc;

    if (
        desc.subplot_rows > TP_MAX_SUBPLOT_ROWS || 
        desc.subplot_cols > TP_MAX_SUBPLOT_COLS
    ) {
        TP_WARN(
            "[term-plot Warning] "
            "Clamping figure subplot rows and columns to maximum. "
            "If this is an issue, consider defining `TP_MAX_SUBPLOT_ROWS` "
            "and/or `TP_MAX_SUBPLOT_COLS` to a higher number.\n"
        );
    }

    tp_figure* fig = _TP_PUSH_STRUCT(arena, tp_figure);

    *fig = (tp_figure){
        .arena = arena,

        .subplot_rows = _TP_CLAMP(desc.subplot_rows, 1, TP_MAX_SUBPLOT_ROWS),
        .subplot_cols = _TP_CLAMP(desc.subplot_cols, 1, TP_MAX_SUBPLOT_COLS),

        .char_width = desc.char_width,
        .char_height = desc.char_height,

        .width_offset = desc.char_width_offset,
        .height_offset = desc.char_height_offset,

        .raster_type = desc.raster_type,

        .monochrome = desc.monochrome,

        .padding = desc.override_padding ? desc.custom_padding : 1,
        .margin = desc.override_margin ? desc.custom_margin : 1,

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

    if (width_ratio_sum == 0) {
        tp_f64 def = 1.0 / (tp_f64)fig->subplot_cols;

        for (tp_u32 i = 0; i < fig->subplot_cols; i++) {
            fig->width_fractions[i] = def; 
        }
    } else {
        for (tp_u32 i = 0; i < fig->subplot_cols; i++) {
            fig->width_fractions[i] = desc.width_ratios[i] / width_ratio_sum;
        }
    }

    tp_f64 height_ratio_sum = 0.0;
    for (tp_u32 i = 0; i < fig->subplot_rows; i++) {
        height_ratio_sum += desc.height_ratios[i];
    }

    if (height_ratio_sum == 0) {
        tp_f64 def = 1.0 / (tp_f64)fig->subplot_rows;

        for (tp_u32 i = 0; i < fig->subplot_rows; i++) {
            fig->height_fractions[i] = def; 
        }
    } else {
        for (tp_u32 i = 0; i < fig->subplot_rows; i++) {
            fig->height_fractions[i] = desc.height_ratios[i] / height_ratio_sum;
        }
    }

    return fig;
}

tp_figure* tp_begin(const tp_figure_desc* fig_desc) {
    _tp_arena* arena = _tp_arena_create(_TP_MiB(1), _TP_KiB(64));
    return _tp_fig_init(arena, fig_desc);
}

void tp_reset(tp_figure* fig, const tp_figure_desc* fig_desc) {
    _tp_arena* arena = fig->arena;
    _tp_arena_clear(arena);

    fig = _tp_fig_init(arena, fig_desc);
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
            "Ignoring out of bounds call to `tp_setup_axes`.\n"
        );

        return;
    }

    tp_axes_desc desc = axes_desc == NULL ? (tp_axes_desc){ 0 } : *axes_desc;

    _tp_axes* axes = fig->axes + (arow * fig->subplot_cols + acol);

    // Not recreating the entirety of the axes struct in case there are 
    // already registered draw commands

    axes->flags = desc.flags;

    axes->xlabel = _tp_str8_copy(fig->arena, desc.xlabel);
    axes->ylabel = _tp_str8_copy(fig->arena, desc.ylabel);
    axes->title = _tp_str8_copy(fig->arena, desc.title);

    if (desc.num_xticks > 0 && desc.xticks != NULL) {
        axes->num_xticks = desc.num_xticks;
        axes->xticks = _TP_PUSH_ARRAY_NZ(fig->arena, tp_f64, axes->num_xticks);
        memcpy(axes->xticks, desc.xticks, sizeof(tp_f64) * axes->num_xticks);
    }

    if (desc.num_yticks > 0 && desc.yticks != NULL) {
        axes->num_yticks = desc.num_yticks;
        axes->yticks = _TP_PUSH_ARRAY_NZ(fig->arena, tp_f64, axes->num_yticks);
        memcpy(axes->yticks, desc.yticks, sizeof(tp_f64) * axes->num_yticks);
    }

    axes->desired_xtick_gap = desc.desired_xtick_gap == 0 ?
        12 : desc.desired_xtick_gap;

    axes->desired_ytick_gap = desc.desired_ytick_gap == 0 ?
        4 : desc.desired_ytick_gap;

    axes->bg_col = desc.bg_col;

    axes->xmin = desc.xmin;
    axes->xmax = desc.xmax;
    axes->ymin = desc.ymin;
    axes->ymax = desc.ymax;
}

static void _tp_plot_pointslike(
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
            "and/or invalid data pointers\n"
        );

        return;
    }

    _tp_axes* axes = fig->axes + (
        points_desc->axes_row * fig->subplot_cols + points_desc->axes_col
    );

    _tp_info_pointslike pointslike = (_tp_info_pointslike){
        .num_points = points_desc->num_points,

        .xs = _TP_PUSH_ARRAY_NZ(fig->arena, tp_f64, points_desc->num_points),
        .ys = _TP_PUSH_ARRAY_NZ(fig->arena, tp_f64, points_desc->num_points),

        .label = _tp_str8_copy(fig->arena, points_desc->label),
    };

    pointslike.col = _tp_rgb8_eq(points_desc->col, (tp_rgb8){ 0 }) ?
        _tp_palette[axes->palette_index] : points_desc->col;

    axes->palette_index = (axes->palette_index + 1) % _TP_ARR_LEN(_tp_palette);

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
        // are in the same union, than any `_tp_info_pointslike` in that union
        // will occupy the same place in memory
        .info.scatter = pointslike
    };

    _TP_SLL_PUSH_BACK(axes->first_draw, axes->last_draw, cmd);
}

void tp_plot_scatter(tp_figure* fig, const tp_points_desc* points) {
    _tp_plot_pointslike(fig, _TP_DRAW_SCATTER, points);
}

void tp_plot_lines(tp_figure* fig, const tp_points_desc* points) {
    _tp_plot_pointslike(fig, _TP_DRAW_LINES, points);
}

////////////////////////////////////////////////////////////////////////////////
// 
// Figure layout
// 
////////////////////////////////////////////////////////////////////////////////

// One for the text, one for padding
// Used for figure and axes titles
#define _TP_TITLE_ROWS 2

typedef struct {
    // Total drawable area
    // Stored row-major, only the first (fig->subplot_rows * fig->subplot_cols)
    // elements are used
    _tp_rect_u32 drawable[TP_MAX_SUBPLOTS];

    // Area of the actual graph
    // Stored row-major, only the first (fig->subplot_rows * fig->subplot_cols)
    // elements are used
    _tp_rect_u32 graphable[TP_MAX_SUBPLOTS];
} _tp_fig_layout;

typedef struct {
    tp_u32 top, bottom, left, right;
} _tp_fixed_size;

// When doing layout, some parts of the figure are allowed to grow and other 
// are fixed. This function calculates the fixed dimensions for a given set of
// axes in the figure
static _tp_fixed_size _tp_axes_fixed_size(
    tp_figure* fig,
    tp_u32 subplot_row, tp_u32 subplot_col
) {
    if (
        subplot_row >= fig->subplot_rows ||
        subplot_col >= fig->subplot_cols
    ) {
        return (_tp_fixed_size){ 0 };
    }

    tp_u32 axes_index = subplot_row * fig->subplot_cols + subplot_col;
    _tp_axes* axes = fig->axes + axes_index;

    _tp_fixed_size size = {
        // (y label & pad) + (y nums + pad) + y axis
        .left = 2 + (TP_AXIS_NUM_CHARS + 1) + 1,

        // (title & y scale & pad)
        .top = _TP_TITLE_ROWS,

        // (x label & x scale & pad) + x nums + x axis
        .bottom = 2 + 1 + 1,
    };

    if (axes->ylabel.size == 0) { size.left--; }
    if (axes->flags & TP_AXES_NO_YNUMS) { size.left -= TP_AXIS_NUM_CHARS; }

    if (axes->flags & TP_AXES_NO_XNUMS) { size.bottom--; }

    return size;
}

// Fills in `_tp_fig_layout` with the current layout given `draw_area` 
// This function ignores fig->char_width/char_height, so callers need to 
// compute `draw_area` externally from the figure size and current terminal 
// size
// 
// Returns true if there is enough space
static tp_b32 _tp_layout_axes(
    tp_figure* fig, _tp_fig_layout* layout, _tp_rect_u32 draw_area
) {
    tp_u32 fixed_x = 2 * fig->margin + (fig->subplot_cols - 1) * fig->padding;
    tp_u32 fixed_y = 2 * fig->margin + (fig->subplot_rows - 1) * fig->padding;

    if (fig->title.size > 0) {
        fixed_y += _TP_TITLE_ROWS; 
    }

    if (draw_area.w <= fixed_x || draw_area.h <= fixed_y) {
        return _TP_FALSE;
    }

    tp_u32 remaining_width = draw_area.w - fixed_x;
    tp_u32 remaining_height = draw_area.h - fixed_y;

    _tp_fixed_size fixed_sizes[TP_MAX_SUBPLOT_ROWS * TP_MAX_SUBPLOT_COLS];

    for (tp_u32 row = 0; row < fig->subplot_rows; row++) {
        for (tp_u32 col = 0; col < fig->subplot_cols; col++) {
            tp_u32 idx = col + row * fig->subplot_cols;
            fixed_sizes[idx] = _tp_axes_fixed_size(fig, row, col);
        }
    }

    // Required fixed width for each columns
    tp_u32 fixed_widths[TP_MAX_SUBPLOT_COLS] = { 0 };

    // Required fixed height for each row
    tp_u32 fixed_heights[TP_MAX_SUBPLOT_COLS] = { 0 };

    for (tp_u32 col = 0; col < fig->subplot_cols; col++) {
        tp_u32 max_width = 0;

        for (tp_u32 row = 0; row < fig->subplot_rows; row++) {
            tp_u32 idx = col + row * fig->subplot_cols;

            max_width = _TP_MAX(
                max_width,
                fixed_sizes[idx].left + fixed_sizes[idx].right
            );
        }

        fixed_widths[col] = max_width;
    }

    for (tp_u32 row = 0; row < fig->subplot_rows; row++) {
        tp_u32 max_height = 0;

        for (tp_u32 col = 0; col < fig->subplot_cols; col++) {
            tp_u32 idx = col + row * fig->subplot_cols;

            max_height = _TP_MAX(
                max_height,
                fixed_sizes[idx].top + fixed_sizes[idx].bottom
            );
        }

        fixed_heights[row] = max_height;
    }

    tp_u32 widths[TP_MAX_SUBPLOT_COLS] = { 0 };
    tp_u32 heights[TP_MAX_SUBPLOT_COLS] = { 0 };

    // TODO: better distribution of area than flooring
    for (tp_u32 x = 0; x < fig->subplot_cols; x++) {
        widths[x] = (tp_u32)floor(
            (tp_f64)remaining_width * fig->width_fractions[x]
        );

        if (widths[x] <= fixed_widths[x]) {
            return _TP_FALSE;
        }
    }

    for (tp_u32 y = 0; y < fig->subplot_rows; y++) {
        heights[y] = (tp_u32)floor(
            (tp_f64)remaining_height * fig->height_fractions[y]
        );

        if (heights[y] < fixed_heights[y]) {
            return _TP_FALSE;
        }
    }

    memset(layout, 0, sizeof(_tp_fig_layout));

    tp_u32 cur_x = fig->margin;
    for (tp_u32 col = 0; col < fig->subplot_cols; col++) {
        tp_u32 cur_y = _TP_TITLE_ROWS + fig->margin;

        for (tp_u32 row = 0; row < fig->subplot_rows; row++) {
            tp_u32 idx = col + row * fig->subplot_cols;

            layout->drawable[idx] = (_tp_rect_u32){
                .x = cur_x + draw_area.x,
                .y = cur_y + draw_area.y,
                .w = widths[col],
                .h = heights[row],
            };

            _tp_fixed_size fs = fixed_sizes[idx];
            
            layout->graphable[idx] = (_tp_rect_u32){
                .x = cur_x + draw_area.x + fs.left,
                .y = cur_y + draw_area.y + fs.top,
                .w = widths[col] - (fs.left + fs.right),
                .h = heights[row] - (fs.top + fs.bottom),
            };

            cur_y += heights[row] + fig->padding;
        }

        cur_x += widths[col] + fig->padding;
    }

    return _TP_TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// Draw target functions
// 
////////////////////////////////////////////////////////////////////////////////

typedef struct {
    tp_u32 width, height;

    // UTF-32 grid of codepoints
    // Row-major of size (width x height)
    tp_u32* codepoints;

    // Foreground colors
    // Row-major of size (width x height)
    tp_rgb8* fg_cols;

    // Background colors
    // Row-major of size (width x height)
    tp_rgb8* bg_cols;

    // A bitmask used for coloring of characters, stored row-major. 
    //
    // This is here because certain raster backends will draw to sub-character
    // units, while coloring can only happen on a per-character basis. The
    // bitmask helps these more advanced rasters keep track of the characters
    // that they have hit
    tp_u64* col_bitmask;
} _tp_draw_target;

static _tp_draw_target _tp_draw_target_create(
    _tp_arena* arena, _tp_dim_u32 dim
) {
    tp_u32 size = dim.width * dim.height;

    _tp_draw_target out = {
        .width = dim.width,
        .height = dim.height,

        .codepoints = _TP_PUSH_ARRAY_NZ(arena, tp_u32, size),
        .fg_cols = _TP_PUSH_ARRAY_NZ(arena, tp_rgb8, size),
        .bg_cols = _TP_PUSH_ARRAY_NZ(arena, tp_rgb8, size),
        .col_bitmask = _TP_PUSH_ARRAY(arena, tp_u64, (size + 63) / 64),
    };

    for (tp_u32 i = 0; i < size; i++) {
        out.codepoints[i] = (tp_u32)' ';
    }

    for (tp_u32 i = 0; i < size; i++) {
        out.fg_cols[i] = (tp_rgb8){ 255, 255, 255 };
    }

    for (tp_u32 i = 0; i < size; i++) {
        out.bg_cols[i] = (tp_rgb8){ 0, 0, 0 };
    }

    return out;
}

// Returns the number of bytes it would take to encode the draw target
static tp_u64 _tp_draw_target_encode_size(
    _tp_draw_target* dt,
    tp_newline_style newline_style,
    tp_b32 with_color
) {
    if (dt->width == 0 || dt->height == 0) {
        return 0;
    }

    // Primary utf8 stream size
    tp_u64 size = _tp_utf8_from_utf32_size(
        dt->codepoints, dt->width * dt->height
    );

    // Newline size
    size += (newline_style == TP_NEWLINE_LF ? 1 : 2) * (dt->height - 1);

    // Ansi color sizes
    if (with_color) {
        // Setup size (foreground + background)
        size += 2 * _TP_ANSI_COL_SIZE;

        tp_rgb8 prev_fg = dt->fg_cols[0];
        tp_rgb8 prev_bg = dt->bg_cols[0];

        for (tp_u32 i = 1; i < dt->width * dt->height; i++) {
            if (!_tp_rgb8_eq(prev_fg, dt->fg_cols[i])) {
                size += _TP_ANSI_COL_SIZE;
                prev_fg = dt->fg_cols[i];
            }

            if (!_tp_rgb8_eq(prev_bg, dt->bg_cols[i])) {
                size += _TP_ANSI_COL_SIZE;
                prev_bg = dt->bg_cols[i];
            }
        }

        size += sizeof(_TP_ANSI_RESET) - 1;
    }

    return size;
}

// Encodes the draw_target to a utf8 code with ansi codes
static void _tp_draw_target_encode(
    _tp_draw_target* dt,
    tp_newline_style newline_style,
    tp_b32 with_color, tp_u8* out
) {
    if (dt->width == 0 || dt->height == 0) {
        return;
    }

    tp_u64 pos = 0;

    if (with_color) {
        _tp_ansi_from_rgb8(dt->fg_cols[0], _TP_TRUE, out + pos);
        pos += _TP_ANSI_COL_SIZE;

        _tp_ansi_from_rgb8(dt->bg_cols[0], _TP_FALSE, out + pos);
        pos += _TP_ANSI_COL_SIZE;
    }

    tp_rgb8 prev_fg = dt->fg_cols[0];
    tp_rgb8 prev_bg = dt->bg_cols[0];

    for (tp_u32 y = 0; y < dt->height; y++) {
        for (tp_u32 x = 0; x < dt->width; x++) {
            tp_u32 index = x + y * dt->width;

            if (with_color) {
                tp_rgb8 fg = dt->fg_cols[index];
                if (!_tp_rgb8_eq(prev_fg, fg)) {
                    _tp_ansi_from_rgb8(fg, _TP_TRUE, out + pos);
                    pos += _TP_ANSI_COL_SIZE;
                    prev_fg = fg;
                }

                tp_rgb8 bg = dt->bg_cols[index];
                if (!_tp_rgb8_eq(prev_bg, bg)) {
                    _tp_ansi_from_rgb8(bg, _TP_FALSE, out + pos);
                    pos += _TP_ANSI_COL_SIZE;
                    prev_bg = bg;
                }
            }

            tp_u32 cp_len = _tp_utf8_encode(dt->codepoints[index], out + pos);
            pos += cp_len;
        }

        if (y < dt->height - 1) {
            switch (newline_style) {
                case TP_NEWLINE_LF: {
                    out[pos++] = '\n';
                } break;

                case TP_NEWLINE_CRLF: {
                    out[pos++] = '\r';
                    out[pos++] = '\n';
                } break;
            }
        }
    }

    if (with_color) {
        memcpy(out + pos, _TP_ANSI_RESET, sizeof(_TP_ANSI_RESET) - 1);
    }
}

// Colors the foreground in the given area in accordance with the `col_bitmask`
// inside the draw target. 
//
// Only looks at the first `area.w * area.h` bits within the `col_bitmask`
static void _tp_draw_target_color_fg(
    _tp_draw_target* dt, tp_rgb8 fg, _tp_rect_u32 area
) {
    tp_u32 num = (area.w * area.h + 63) / 64;

    for (tp_u32 i = 0; i < num; i++) {
        tp_u64 cur_bits = dt->col_bitmask[i];

        while (cur_bits > 0) {
            tp_u32 bit_pos = 63 - _tp_clz_u64(cur_bits);
            cur_bits &= ~((tp_u64)1 << bit_pos);

            tp_u32 index = (i * 64) + bit_pos;

            div_t pos = div((tp_i32)index, (tp_i32)area.w);
            tp_u32 x = (tp_u32)pos.rem;
            tp_u32 y = (tp_u32)pos.quot;

            tp_u32 out_index = (x + area.x) + (y + area.y) * dt->width;
            tp_rgb8 existing = dt->fg_cols[out_index];

            dt->fg_cols[out_index] = (tp_rgb8){
                (tp_u8)_TP_MIN(0xff, (tp_u32)existing.r + fg.r),
                (tp_u8)_TP_MIN(0xff, (tp_u32)existing.g + fg.g),
                (tp_u8)_TP_MIN(0xff, (tp_u32)existing.b + fg.b),
            };
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// 
// Raster backend implementations
// 
////////////////////////////////////////////////////////////////////////////////

static void _tp_braille_draw_clear(
    _tp_draw_target* dt, _tp_rect_u32 graphable
) {
    for (tp_u32 ly = 0; ly < graphable.h; ly++) {
        for (tp_u32 lx = 0; lx < graphable.w; lx++) {
            tp_u32 index = (graphable.x + lx) + (graphable.y + ly) * dt->width;

            dt->codepoints[index] = 0x2800;
        }
    }
}

static void _tp_braille_draw_point(
    _tp_draw_target* dt, _tp_rect_u32 graphable,
    tp_i32 dot_x, tp_i32 dot_y
) {
    if (
        dot_x < 0 || dot_x >= (tp_i32)graphable.w * 2 ||
        dot_y < 0 || dot_y >= (tp_i32)graphable.h * 4
    ) {
        return;
    }

    static const tp_u8 braille_pattern[] = {
        0x1, 0x8,
        0x2, 0x10,
        0x4, 0x20,
        0x40, 0x80,
    };

    tp_u32 char_x = (tp_u32)dot_x / 2;
    tp_u32 char_y = (tp_u32)dot_y / 4;

    tp_u32 lx = (tp_u32)dot_x % 2;
    tp_u32 ly = (tp_u32)dot_y % 4;

    tp_u32 index = (graphable.x + char_x) + (graphable.y + char_y) * dt->width;
    dt->codepoints[index] |= braille_pattern[lx + ly * 2];

    tp_u32 bit_index = char_x + char_y * graphable.w;
    dt->col_bitmask[bit_index / 64] |= (tp_u64)1 << (bit_index % 64);
}

static void _tp_ascii_draw_clear(_tp_draw_target* dt, _tp_rect_u32 graphable) {
    for (tp_u32 ly = 0; ly < graphable.h; ly++) {
        for (tp_u32 lx = 0; lx < graphable.w; lx++) {
            tp_u32 index = (graphable.x + lx) + (graphable.y + ly) * dt->width;

            dt->codepoints[index] = ' ';
        }
    }
}

static void _tp_ascii_draw_point(
    _tp_draw_target* dt, _tp_rect_u32 graphable,
    tp_i32 x, tp_i32 y
) {
    if (
        x < 0 || x >= (tp_i32)graphable.w ||
        y < 0 || y >= (tp_i32)graphable.h
    ) {
        return;
    }

    tp_u32 index = (graphable.x + (tp_u32)x) +
        (graphable.y + (tp_u32)y) * dt->width;

    dt->codepoints[index] = U'#';

    tp_u32 bit_index = (tp_u32)x + (tp_u32)y * graphable.w;
    dt->col_bitmask[bit_index / 64] |= (tp_u64)1 << (bit_index % 64);
}

////////////////////////////////////////////////////////////////////////////////
// 
// Draw context functions --- Primary graph rendering
// 
////////////////////////////////////////////////////////////////////////////////

typedef void (_tp_draw_clear_func)(_tp_draw_target* dt, _tp_rect_u32 graphable);

typedef void (_tp_draw_point_func)(
    _tp_draw_target* dt, _tp_rect_u32 graphable,
    tp_i32 x, tp_i32 y
);

typedef struct {
    tp_f64 xmin, xmax;
    tp_f64 ymin, ymax;
} _tp_axes_bounds;

static const _tp_axes_bounds _tp_def_axes_bounds = {
    .xmin = INFINITY, .xmax = -INFINITY,
    .ymin = INFINITY, .ymax = -INFINITY,
};

typedef struct {
    // How many times more dense the raster grid is from the character grid
    // For example, braille's `raster_xmult` and `raster_ymult` would equal 2
    // and 4 respectively
    tp_f64 raster_xmult;
    tp_f64 raster_ymult;

    _tp_draw_clear_func* raster_clear;
    _tp_draw_point_func* raster_point;

    // Stored row-major, only the first (fig->subplot_rows * fig->subplot_cols)
    // elements are used
    _tp_axes_bounds bounds[TP_MAX_SUBPLOTS];
} _tp_draw_context;

static _tp_axes_bounds _tp_pointslike_bounds(
    const _tp_info_pointslike* pointslike
) {
    _tp_axes_bounds cur = _tp_def_axes_bounds;

    for (tp_u32 i = 0; i < pointslike->num_points; i++) {
        cur.xmin = _TP_MIN(cur.xmin, pointslike->xs[i]);
        cur.xmax = _TP_MAX(cur.xmax, pointslike->xs[i]);

        cur.ymin = _TP_MIN(cur.ymin, pointslike->ys[i]);
        cur.ymax = _TP_MAX(cur.ymax, pointslike->ys[i]);
    }

    tp_f64 xdim = cur.xmax - cur.xmin;
    tp_f64 ydim = cur.ymax - cur.ymin;

    tp_f64 xmargin = xdim * 0.05;
    tp_f64 ymargin = ydim * 0.05;

    cur.xmin -= xmargin;
    cur.xmax += xmargin;
    cur.ymin -= ymargin;
    cur.ymax += ymargin;

    return cur;
}

static void _tp_expand_bounds(
    _tp_axes_bounds* bounds, const _tp_draw_cmd* cmd
) {
    _tp_axes_bounds cur = _tp_def_axes_bounds;

    switch (cmd->kind) {
        case _TP_DRAW_NONE: break;

        case _TP_DRAW_LINES: {
            cur = _tp_pointslike_bounds(&cmd->info.lines);
        } break;
            
        case _TP_DRAW_SCATTER: {
            cur = _tp_pointslike_bounds(&cmd->info.scatter);
        } break;
    }

    bounds->xmin = _TP_MIN(bounds->xmin, cur.xmin);
    bounds->xmax = _TP_MAX(bounds->xmax, cur.xmax);
    bounds->ymin = _TP_MIN(bounds->ymin, cur.ymin);
    bounds->ymax = _TP_MAX(bounds->ymax, cur.ymax);
}

// Computes the bounds for each subplot in the figure and write the results to 
// `draw_context`
static void _tp_compute_bounds(
    tp_figure* fig, _tp_draw_context* draw_context
) {
    for (tp_u32 i = 0; i < fig->subplot_rows * fig->subplot_cols; i++) {
        _tp_axes* axes = fig->axes + i;

        tp_b8 override_x = axes->xmin == 0.0 && axes->xmax == 0.0;
        tp_b8 override_y = axes->ymin == 0.0 && axes->ymax == 0.0;

        draw_context->bounds[i] = (_tp_axes_bounds){
            .xmin = axes->xmin, .xmax = axes->xmax,
            .ymin = axes->ymin, .ymax = axes->ymax,
        };

        if (override_x || override_y) {
            _tp_axes_bounds computed = _tp_def_axes_bounds;

            if (axes->first_draw == NULL) {
                computed = (_tp_axes_bounds){
                    .xmin = 0.0, .xmax = 1.0,
                    .ymin = 0.0, .ymax = 1.0,
                };
            } else {
                for (
                    _tp_draw_cmd* cmd = axes->first_draw;
                    cmd != NULL; cmd = cmd->next
                ) {
                    _tp_expand_bounds(&computed, cmd);
                }
            }

            if (override_x) {
                draw_context->bounds[i].xmin = computed.xmin;
                draw_context->bounds[i].xmax = computed.xmax;
            }

            if (override_y) {
                draw_context->bounds[i].ymin = computed.ymin;
                draw_context->bounds[i].ymax = computed.ymax;
            }
        }
    }
}

static void _tp_draw_context_init(
    tp_figure* fig, _tp_draw_context* draw_context
) {
    memset(draw_context, 0, sizeof(_tp_draw_context));

    switch (fig->raster_type) {
        case TP_RASTER_BRAILLE: {
            *draw_context = (_tp_draw_context){ 
                .raster_xmult = 2,
                .raster_ymult = 4,

                .raster_clear = _tp_braille_draw_clear,
                .raster_point = _tp_braille_draw_point,
            };
        } break;

        case TP_RASTER_ASCII: {
            *draw_context = (_tp_draw_context){ 
                .raster_xmult = 1,
                .raster_ymult = 1,

                .raster_clear = _tp_ascii_draw_clear,
                .raster_point = _tp_ascii_draw_point,
            };
        } break;

        case _TP_RASTER_COUNT: break;
    }

    _tp_compute_bounds(fig, draw_context);
}

typedef struct {
    tp_f64 xscale;
    tp_f64 yscale;
    tp_f64 xoff;
    tp_f64 yoff;
} _tp_bound_transforms;

// Computes transform parameters such that 
//
// x * xscale + xoff
// y * yscale + yoff
//
// transform a given (x, y) from graph space to dot space, where dot space is
// +y down and generally a direct factor of the character dimensions of the 
// draw area
static _tp_bound_transforms _tp_get_bound_transforms(
    const _tp_axes_bounds* bounds,
    tp_f64 xdots, tp_f64 ydots
) {
    tp_f64 xscale = xdots / (bounds->xmax - bounds->xmin);
    tp_f64 yscale = -(ydots / (bounds->ymax - bounds->ymin));

    tp_f64 xoff = -bounds->xmin * xscale;
    tp_f64 yoff = -bounds->ymax * yscale;

    return (_tp_bound_transforms){
        .xscale = xscale,
        .yscale = yscale,
        .xoff = xoff,
        .yoff = yoff,
    };
}

static void _tp_draw_scatter(
    _tp_draw_target* dt,
    const _tp_draw_context* context,
    const _tp_info_pointslike* pointslike,
    const _tp_bound_transforms* bt,
    _tp_rect_u32 graphable
) {
    for (tp_u32 i = 0; i < pointslike->num_points; i++) {
        tp_i32 dot_x = (tp_i32)(pointslike->xs[i] * bt->xscale + bt->xoff);
        tp_i32 dot_y = (tp_i32)(pointslike->ys[i] * bt->yscale + bt->yoff);

        context->raster_point(dt, graphable, dot_x, dot_y);
    }
}

static void _tp_draw_lines(
    _tp_draw_target* dt,
    const _tp_draw_context* context,
    const _tp_info_pointslike* pointslike,
    const _tp_bound_transforms* bt,
    _tp_rect_u32 graphable
) {
    tp_i32 x0 = (tp_i32)(pointslike->xs[0] * bt->xscale + bt->xoff);
    tp_i32 y0 = (tp_i32)(pointslike->ys[0] * bt->yscale + bt->yoff);

    context->raster_point(dt, graphable, x0, y0);

    for (tp_u32 i = 1; i < pointslike->num_points; i++) {
        tp_i32 x1 = (tp_i32)(pointslike->xs[i] * bt->xscale + bt->xoff);
        tp_i32 y1 = (tp_i32)(pointslike->ys[i] * bt->yscale + bt->yoff);

        tp_i32 dx = _TP_ABS(x1 - x0);
        tp_i32 dy = _TP_ABS(y1 - y0);
        tp_i32 sx = x0 < x1 ? 1 : -1;
        tp_i32 sy = y0 < y1 ? 1 : -1;

        tp_i32 err = (dx > dy ? dx : -dy) / 2;
        for (tp_u32 worstcase = 0; worstcase < 0xffffff; worstcase++) {
            if (x0 >= x1 && y0 >= y1) { break; }

            tp_i32 new_err = err;
            if (err > -dx) { 
                new_err -= dy;
                x0 += sx;
            }

            if (err < dy) {
                new_err += dx;
                y0 += sy;
            }

            err = new_err;

            context->raster_point(dt, graphable, x0, y0);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// 
// High level drawing functions
// 
////////////////////////////////////////////////////////////////////////////////

// Draws the given text horizontally centered within the draw area at
// the given y offset
//
// If there is not enough space to fit the text, it will be clipped and show 
// the first `area.w` codepoints
static void _tp_draw_hcenter_text(
    _tp_draw_target* dt, _tp_rect_u32 area,
    tp_string8 text, tp_u32 y_off, tp_rgb8 fg_col
) {
    if (text.size == 0) { return; }

    y_off = _TP_MIN(y_off, area.h - 1);

    tp_u32 text_width = _tp_str8_width(text);
    tp_u32 fig_mid = area.w / 2;

    tp_u32 draw_offset = text_width / 2 > fig_mid ?
        0 : fig_mid - text_width / 2;
    text_width = _TP_MIN(text_width, area.w - draw_offset);

    tp_u32* out_codes = dt->codepoints + (
        (area.x + draw_offset) + (area.y + y_off) * dt->width
    );

    _tp_utf32_from_utf8(text, out_codes, text_width);

    for (tp_u32 i = 0; i < text_width; i++) {
        tp_u32 index = (area.x + draw_offset + i) +
            (area.y + y_off) * dt->width;

        dt->fg_cols[index] = fg_col;
    }
}

// Draws the given text vertically centered within the draw area at the given
// x offset
//
// If there is not enough space to fit the text, it will be clipped and show 
// the first `area.h` codepoints
static void _tp_draw_vcenter_text(
    _tp_draw_target* dt, _tp_rect_u32 area,
    tp_string8 text, tp_u32 x_off, tp_rgb8 fg_col
) {
    if (text.size == 0) { return; }

    x_off = _TP_MIN(x_off, area.w - 1);

    tp_u32 text_height = _tp_str8_width(text);
    tp_u32 fig_mid = area.h / 2;

    tp_u32 draw_offset = text_height / 2 > fig_mid ?
        0 : fig_mid - text_height / 2;
    text_height = _TP_MIN(text_height, area.h - draw_offset);

    tp_u64 byte_pos = 0;
    tp_u32 cur_len = 1;
    for (
        tp_u32 y_off = 0;
        y_off < text_height && byte_pos < text.size && cur_len > 0;
        y_off++, byte_pos += cur_len
    ) {
        tp_u32 codepoint = ' ';
        cur_len = _tp_utf8_decode(
            text.str + byte_pos, text.size - byte_pos, &codepoint
        );

        tp_u32 index = (area.x + x_off) +
            (area.y + draw_offset + y_off) * dt->width;

        dt->codepoints[index] = codepoint;
        dt->fg_cols[index] = fg_col;
    }
}

static _tp_dim_u32 _tp_size_figure(tp_figure* fig, tp_b32 clamp_to_term) {
    _tp_dim_u32 fig_size = { fig->char_width, fig->char_height };
    _tp_dim_u32 term_size = _tp_get_termsize();

    if (fig_size.width == 0 && fig_size.height == 0) {
        fig_size = term_size;
    }

    fig_size.width = (tp_u32)_TP_MAX(
        0, (tp_i32)fig_size.width + fig->width_offset
    );

    fig_size.height = (tp_u32)_TP_MAX(
        0, (tp_i32)fig_size.height + fig->height_offset
    );

    if (clamp_to_term) {
        fig_size.width = _TP_MIN(fig_size.width, term_size.width);
        fig_size.height = _TP_MIN(fig_size.height, term_size.height);
    }

    return fig_size;
}

static tp_f64* _tp_compute_auto_ticks(
    _tp_arena* arena, tp_f64 start, tp_f64 end,
    tp_u32 desired_count, tp_u32* count
) {
    if (start == end) { 
        *count = 0;
        return NULL;
    }

    if (end < start) {
        tp_f64 temp = start;
        start = end;
        end = temp;
    }

    tp_f64 unrounded_step = (end - start) / (tp_f64)(desired_count);

    tp_f64 step_base = pow(10.0, floor(log10(unrounded_step)));

    tp_u32 factors[] = { 1, 2, 5, 10 };
    tp_f64 step = step_base;

    for (tp_u32 i = 0; i < _TP_ARR_LEN(factors); i++) {
        tp_f64 s = step_base * factors[i];

        if (_TP_ABS(s - unrounded_step) < _TP_ABS(step - unrounded_step)) {
            step = s;
        }
    }

    tp_f64 tick_base = start - fmod(start, step);

    *count = (tp_u32)ceil((end-tick_base) / step);

    // Sometimes, tick_base will be less than start
    // This does not really matter, because the tick drawing will skip OOB 
    // ticks, but I figure I might as well filter it here. 
    if (tick_base < start) {
        *count -= 1;
        tick_base += step;
    }

    tp_f64* ticks = _TP_PUSH_ARRAY(arena, tp_f64, *count);

    for (tp_u32 i = 0; i < *count; i++) {
        ticks[i] = tick_base + (tp_f64)i * step;
    }

    return ticks;
}

static void _tp_draw_axes(
    const tp_figure* fig, _tp_draw_target* dt, 
    tp_u32 axes_row, tp_u32 axes_col,
    const _tp_fig_layout* layout,
    const _tp_draw_context* context
) {
    tp_u32 axes_index = axes_col + axes_row * fig->subplot_cols;
    const _tp_axes* axes = fig->axes + axes_index;

    _tp_rect_u32 drawable = layout->drawable[axes_index];
    _tp_rect_u32 graphable = layout->graphable[axes_index];
    _tp_axes_bounds bounds = context->bounds[axes_index];

    _tp_draw_hcenter_text(
        dt, drawable, axes->title, 0, (tp_rgb8){ 255, 255, 255}
    );
    
    tp_u32 origin = U'┗';
    tp_u32 xbar = U'━';
    tp_u32 ybar = U'┃';
    tp_u32 xtick = U'┯';
    tp_u32 ytick = U'┨';

    if (fig->raster_type == TP_RASTER_ASCII) {
        origin = U'o';
        xbar = U'-';
        ybar = U'|';
        xtick = U'+';
        ytick = U'+';
    }

    // This is the horizontal position that the y axis is drawn to
    tp_u32 yaxis_x = graphable.x >= 1 ? graphable.x - 1 : 0;

    // This is the vertical position that the x axis is drawn to
    tp_u32 xaxis_y = _TP_MIN(
        graphable.y + graphable.h,
        drawable.y + drawable.h - 1
    );

    // Drawing base axes
    {
        for (tp_u32 ly = 0; ly < graphable.h; ly++) {
            tp_u32 index = yaxis_x + (graphable.y + ly) * dt->width;
            dt->codepoints[index] = ybar;
        }

        for (tp_u32 lx = 0; lx < graphable.w + 1; lx++) {
            tp_u32 index = (yaxis_x + lx) + xaxis_y * dt->width;
            dt->codepoints[index] = xbar;
        }

        tp_u32 origin_index = yaxis_x + xaxis_y * dt->width; 
        dt->codepoints[origin_index] = origin;
    }

    // Axis transforms (for x and y ticks)
    _tp_bound_transforms ax_t = _tp_get_bound_transforms(
        &bounds, graphable.w, graphable.h
    );

    tp_b8 has_xticks = (axes->flags & TP_AXES_NO_XTICKS) != TP_AXES_NO_XTICKS;
    tp_b8 has_yticks = (axes->flags & TP_AXES_NO_YTICKS) != TP_AXES_NO_YTICKS;
    tp_b8 has_xnums = (axes->flags & TP_AXES_NO_XNUMS) != TP_AXES_NO_XNUMS;
    tp_b8 has_ynums = (axes->flags & TP_AXES_NO_YNUMS) != TP_AXES_NO_YNUMS;

    // +1 for null terminator
    tp_u8 num_cstr[TP_AXIS_NUM_CHARS + 1] = { 0 };

    // Used to allocate the automatic ticks
    _tp_arena_temp temp_arena = _tp_arena_temp_begin(fig->arena);

    // Drawing x ticks
    if (has_xticks || has_xnums) {
        tp_u32 num_xticks = axes->num_xticks;
        tp_f64* xticks = axes->xticks;

        if (num_xticks == 0 || xticks == NULL) {
            xticks = _tp_compute_auto_ticks(
                temp_arena.arena, bounds.xmin, bounds.xmax,
                graphable.w / axes->desired_xtick_gap, &num_xticks
            );
        }

        tp_u32 prev_draw_end_x = 0;

        for (tp_u32 i = 0; i < num_xticks; i++) {
            tp_i32 x = (tp_i32)(xticks[i] * ax_t.xscale + ax_t.xoff);

            if (x < 0 || x > (tp_i32)graphable.w) { continue; }

            if (has_xticks) {
                tp_u32 index = (graphable.x + (tp_u32)x) + xaxis_y * dt->width;
                dt->codepoints[index] = xtick;
            }

            if (has_xnums) {
                tp_i32 written = TP_SNPRINTF(
                    (char*)num_cstr, sizeof(num_cstr),
                    "%*g", TP_AXIS_NUM_CHARS, xticks[i]
                );
                written = _TP_MIN(written, TP_AXIS_NUM_CHARS);

                tp_i32 width = TP_AXIS_NUM_CHARS;
                for (tp_i32 j = 0; j < written && num_cstr[j] == ' '; j++) {
                    width--;
                }

                tp_i32 unclamped_start_x = (tp_i32)graphable.x + x - width/2;
                tp_u32 draw_start_x = (tp_u32)_TP_MAX(
                    unclamped_start_x, (tp_i32)drawable.x
                );

                if (
                    draw_start_x <= prev_draw_end_x + 1 ||
                    (tp_u32)width > drawable.x + drawable.w - draw_start_x
                ) {
                    continue; 
                }

                tp_u32 draw_start = draw_start_x + (xaxis_y + 1) * dt->width;
                for (tp_u32 j = 0; j < (tp_u32)width; j++) {
                    dt->codepoints[draw_start + j] =
                        num_cstr[TP_AXIS_NUM_CHARS - width + (tp_i32)j];
                }

                prev_draw_end_x = draw_start_x + (tp_u32)width;
            }
        }
    }

    // Drawing y ticks
    if (has_yticks || has_ynums) {
        tp_u32 num_yticks = axes->num_yticks;
        tp_f64* yticks = axes->yticks;

        if (num_yticks == 0 || yticks == NULL) {
            yticks = _tp_compute_auto_ticks(
                temp_arena.arena, bounds.ymin, bounds.ymax,
                graphable.h / axes->desired_ytick_gap, &num_yticks
            );
        }

        tp_u32 draw_num_start = yaxis_x >= drawable.x + TP_AXIS_NUM_CHARS + 1 ?
            yaxis_x - TP_AXIS_NUM_CHARS - 1 : drawable.x;

        for (tp_u32 i = 0; i < num_yticks; i++) {
            tp_i32 y = (tp_i32)(yticks[i] * ax_t.yscale + ax_t.yoff);

            if (y < 0 || y > (tp_i32)graphable.h) { continue; }

            if (has_yticks) {
                tp_u32 index = yaxis_x + (graphable.y + (tp_u32)y) * dt->width;
                dt->codepoints[index] = ytick;
            }

            if (has_ynums) {
                tp_i32 written = TP_SNPRINTF(
                    (char*)num_cstr, sizeof(num_cstr),
                    "%*g", TP_AXIS_NUM_CHARS, yticks[i]
                );
                written = _TP_MIN(written, TP_AXIS_NUM_CHARS);

                tp_u32 draw_start = draw_num_start +
                    (graphable.y + (tp_u32)y) * dt->width;

                for (tp_i32 j = 0; j < written; j++) {
                    dt->codepoints[draw_start + (tp_u32)j] = num_cstr[j];
                }
            }
        }
    }

    _tp_arena_temp_end(temp_arena);

    if (axes->xlabel.size > 0) {
        _tp_rect_u32 xlabel_area = {
            .x = graphable.x, .w = graphable.w,
            .y = drawable.y, .h = drawable.h,
        };

        _tp_draw_hcenter_text(
            dt, xlabel_area, axes->xlabel, xaxis_y + 2,
            (tp_rgb8){ 200, 200, 200 }
        );
    }

    if (axes->ylabel.size > 0) {
        _tp_rect_u32 ylabel_area = {
            .x = drawable.x, .w = drawable.w,
            .y = graphable.y, .h = graphable.h
        };

        _tp_draw_vcenter_text(
            dt, ylabel_area, axes->ylabel, 0,
            (tp_rgb8){ 200, 200, 200 }
        );
    }

    context->raster_clear(dt, graphable);

    for (tp_u32 ly = 0; ly < graphable.h; ly++) {
        for (tp_u32 lx = 0; lx < graphable.w; lx++) {
            tp_u32 index = (graphable.x + lx) + (graphable.y + ly) * dt->width;

            dt->bg_cols[index] = axes->bg_col;
            dt->fg_cols[index] = (tp_rgb8){ 0 };
        }
    }

    _tp_bound_transforms transforms = _tp_get_bound_transforms(
        &bounds,
        context->raster_xmult * graphable.w,
        context->raster_ymult * graphable.h
    );

    if (!fig->monochrome) {
        memset(dt->col_bitmask, 0, (graphable.w * graphable.h + 7) / 8);
    }

    for (_tp_draw_cmd* cmd = axes->first_draw; cmd != NULL; cmd = cmd->next) {
        tp_b8 color_pass = !fig->monochrome;
        tp_rgb8 fg_col = { 0 };

        switch (cmd->kind) {
            case _TP_DRAW_NONE: break;

            case _TP_DRAW_SCATTER: {
                fg_col = cmd->info.scatter.col;

                _tp_draw_scatter(
                    dt, context,
                    &cmd->info.scatter, &transforms, graphable
                );
            } break;

            case _TP_DRAW_LINES: {
                fg_col = cmd->info.lines.col;

                _tp_draw_lines(
                    dt, context,
                    &cmd->info.lines, &transforms, graphable
                );
            } break;
        }

        if (color_pass) {
            _tp_draw_target_color_fg(dt, fg_col, graphable);

            memset(dt->col_bitmask, 0, (graphable.w * graphable.h + 7) / 8);
        }
    }
}

static void _tp_draw_figure(
    const tp_figure* fig, _tp_draw_target* dt,
    const _tp_fig_layout* layout, const _tp_draw_context* context,
    _tp_rect_u32 area
) {
    _tp_draw_hcenter_text(
        dt, area, fig->title, fig->margin,
        (tp_rgb8){ 255, 255, 255 }
    );

    for (tp_u32 row = 0; row < fig->subplot_rows; row++) {
        for (tp_u32 col = 0; col < fig->subplot_cols; col++) {
            _tp_draw_axes(fig, dt, row, col, layout, context);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// 
// User facing drawing functions
// 
////////////////////////////////////////////////////////////////////////////////

tp_string8 tp_render_utf8(
    tp_figure* fig, tp_newline_style newline_style,
    const tp_allocator* allocator
) {
    tp_string8 out = { 0 };

    _tp_arena_temp temp_arena = _tp_arena_temp_begin(fig->arena);

    _tp_dim_u32 fig_size = _tp_size_figure(fig, _TP_FALSE);
    _tp_rect_u32 draw_area = {
        .w = fig_size.width,
        .h = fig_size.height,
    };

    _tp_draw_target draw_target = _tp_draw_target_create(
        temp_arena.arena, fig_size
    );

    _tp_fig_layout layout;
    if (!_tp_layout_axes(fig, &layout, draw_area)) {
        TP_WARN(
            "[term-plot Warning] "
            "Failed to layout figure, expand figure size to properly render\n"
        );
        goto end;
    }

    _tp_draw_context draw_context = { 0 };
    _tp_draw_context_init(fig, &draw_context);

    _tp_draw_figure(fig, &draw_target, &layout, &draw_context, draw_area);

    tp_u64 rendered_size = _tp_draw_target_encode_size(
        &draw_target, newline_style, !fig->monochrome
    );

    if (allocator == NULL) {
        out.str = malloc(rendered_size);
    } else {
        out.str = allocator->alloc_fn(rendered_size, allocator->user_data);
    }

    if (out.str != NULL) {
        out.size = rendered_size;

        _tp_draw_target_encode(
            &draw_target, newline_style,
            !fig->monochrome, out.str
        );
    }

end:
    _tp_arena_temp_end(temp_arena);

    return out;
}

void tp_show(tp_figure* fig) {
    _tp_arena_temp temp_arena = _tp_arena_temp_begin(fig->arena);

    _tp_dim_u32 fig_size = _tp_size_figure(fig, _TP_FALSE);
    _tp_rect_u32 draw_area = {
        .w = fig_size.width,
        .h = fig_size.height,
    };

    _tp_draw_target draw_target = _tp_draw_target_create(
        temp_arena.arena, fig_size
    );

    _tp_fig_layout layout;
    if (!_tp_layout_axes(fig, &layout, draw_area)) {
        TP_WARN(
            "[term-plot Warning] "
            "Failed to layout figure, expand figure size to properly render\n"
        );
        goto end;
    }

    _tp_draw_context draw_context = { 0 };
    _tp_draw_context_init(fig, &draw_context);

    _tp_draw_figure(fig, &draw_target, &layout, &draw_context, draw_area);

    tp_u64 rendered_size = _tp_draw_target_encode_size(
        &draw_target, TP_NEWLINE_LF, !fig->monochrome
    );

    tp_string8 rendered = {
        .size = rendered_size,
        .str = _TP_PUSH_ARRAY_NZ(temp_arena.arena, tp_u8, rendered_size),
    };

    _tp_draw_target_encode(
        &draw_target, TP_NEWLINE_LF, !fig->monochrome, rendered.str
    );

    tp_enable_utf8_output();
    _tp_term_write(rendered);

end:
    _tp_arena_temp_end(temp_arena);
}

void tp_show_and_end(tp_figure* fig) {
    tp_show(fig);
    tp_end(fig);
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

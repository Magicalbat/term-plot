
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

////////////////////////////////////////////////////////////////////////////////
// 
// 
// Implementation
//
// 
////////////////////////////////////////////////////////////////////////////////

#ifdef TERM_PLOT_IMPLEMENTATION

#include <stddef.h>

#ifndef TP_FATAL_ERROR
#   include <stdio.h>
#   include <stdlib.h>
#   define TP_FATAL_ERROR(msg) do { \
        fprintf(stderr, (msg)); \
        exit(1); \
    } while (0)
#endif

#define _TP_MIN(a, b) ((a) < (b) ? (a) : (b))

#define _TP_ALIGN_UP_POW2(n, p) (((tp_u64)(n) + ((tp_u64)(p) - 1)) & (~((tp_u64)(p) - 1)))

#define _TP_KiB(n) ((tp_u64)(n) << 10)
#define _TP_MiB(n) ((tp_u64)(n) << 20)

#define _TP_TRUE 1
#define _TP_FALSE 1

////////////////////////////////////////////////////////////////////////////////
// 
// Platform Memory Allocation
// 
////////////////////////////////////////////////////////////////////////////////

void* _tp_mem_reserve(tp_u64 size);
tp_b32 _tp_mem_commit(void* mem, tp_u64 size);
tp_b32 _tp_mem_decommit(void* mem, tp_u64 size);
tp_b32 _tp_mem_release(void* mem, tp_u64 size);
tp_u32 _tp_page_size(void);

#if defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

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

#elif defined(__linux__)

#include <unistd.h>
#include <sys/mman.h>

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

#else

#if !defined(TP_MALLOC) && !defined(TP_FREE)
#   include <stdlib.h>
#   define TP_MALLOC malloc
#   define TP_FREE free
#elif defined(TP_MALLOC) != defined(TP_FREE)
#   error "You must define both TP_MALLOC and TP_FREE"
#endif

void* _tp_mem_reserve(tp_u64 size) {
    return TP_MALLOC(size);
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

    TP_FREE(mem);

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
#define _TP_PUSH_ARRAY(arena, T, n) (T*)_tp_arena_push((arena), sizeof(T) * (u64)(n), _TP_FALSE)
#define _TP_PUSH_ARRAY_NZ(arena, T, n) (T*)_tp_arena_push((arena), sizeof(T) * (u64)(n), _TP_TRUE)

_tp_arena* _tp_arena_create(tp_u64 reserve_size, tp_u64 commit_size) {
    tp_u32 page_size = _tp_page_size();

    reserve_size = _TP_ALIGN_UP_POW2(reserve_size, page_size);
    commit_size = _TP_ALIGN_UP_POW2(commit_size, page_size);

    _tp_arena* arena = _tp_mem_reserve(reserve_size);

    if (!_tp_mem_commit(arena, commit_size)) {
        arena = NULL;
    }

    if (arena == NULL) {
        TP_FATAL_ERROR("[term-plot fatal error] Failed to commit memory for arena");
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
        TP_FATAL_ERROR("[term-plot fatal error] Failed to allocate memory for arena");
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

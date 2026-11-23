
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

#if 1

static const char* key_action_names[] = {
#define X(action) #action,
    _TP_KEY_ACTION_XLIST
#undef X
};

int main(void) {
    _tp_arena* arena = _tp_arena_create(_TP_MiB(1), _TP_KiB(64));
    _tp_term_context* tc = _tp_term_enter_raw(arena);

    printf("\x1b[?1003h\x1b[?1006h");

    bool running = true;
    while (running) {
#if 0
        tp_u8 c = 0;
        while ((c = _tp_term_read_char()) != 0) {
            if (c == 'q') running = false;

            printf("%3u: '%c'\r\n", c, c >= 32 && c <= 126 ? c : '?');
        }

#else
        _tp_event event = { 0 };
        while (_tp_next_event(&event)) {
            switch (event.kind) {
                case _TP_EVENT_NONE:
                    printf("[Event None]\n");
                    break;

                case _TP_EVENT_UNKNOWN:
                    printf("[Event Unknown]\n");
                    break;

                case _TP_EVENT_KEY_ACTION:
                    printf(
                        "[Event Key Action] %s\n",
                        key_action_names[event.key_action]
                    );

                    if (event.key_action == _TP_KEY_ACTION_QUIT) {
                        running = false;
                    }

                    break;

                case _TP_EVENT_MOUSE_DOWN:
                    printf(
                        "[Event Mouse Down] at (%u %u)\n",
                        event.mouse_x, event.mouse_y
                    );
                    break;

                case _TP_EVENT_MOUSE_UP:
                    printf(
                        "[Event Mouse Up] at (%u %u)\n",
                        event.mouse_x, event.mouse_y
                    );
                    break;

                case _TP_EVENT_MOUSE_MOVE:
                    printf(
                        "[Event Mouse Move] at (%u %u)\n",
                        event.mouse_x, event.mouse_y
                    );
                    break;


                case _TP_EVENT_SCROLL_DOWN:
                    printf(
                        "[Event Scroll Down] at (%u %u)\n",
                        event.mouse_x, event.mouse_y
                    );
                    break;

                case _TP_EVENT_SCROLL_UP:
                    printf(
                        "[Event Scroll Up] at (%u %u)\n",
                        event.mouse_x, event.mouse_y
                    );
                    break;
            }
        }
#endif

        _tp_sleep_ms(20);
    }

    printf("\x1b[?1000l");

    _tp_term_exit_raw(tc);

    return 0;
}



#else

int main(void) {
    tp_figure* fig = tp_begin(&(tp_figure_desc){
        .subplot_rows = 2,
        .subplot_cols = 2,

        .title = TP_STR8("Example Figure"),
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

#endif



#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <windows.h>

typedef int32_t i32;

typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;

#define ABS(x) ((x) < 0 ? -(x) : (x))

typedef struct {
    u32 char_width, char_height;

    // Each element of `dots` is equal to the corresponding braille codepoint
    // minus 0x2800
    u8* dots;
} braille_map;

static const u8 dot_values[8] = {
    0x01, 0x08,
    0x02, 0x10,
    0x04, 0x20,
    0x40, 0x80,
};

void set_braille_at(braille_map* map, u32 dot_x, u32 dot_y) {
    if (dot_x >= map->char_width * 2 || dot_y >= map->char_height * 4) {
        return;
    }

    u32 char_x = dot_x / 2;
    u32 local_x = dot_x % 2;
    
    u32 char_y = dot_y / 4;
    u32 local_y = dot_y % 4;

    map->dots[char_x + char_y * map->char_width] |=
        dot_values[local_x + local_y * 2];
}

// All coords in dot coords
void braille_line(braille_map* map, i32 x0, i32 y0, i32 x1, i32 y1) {
    i32 dx = ABS(x1 - x0);
    i32 x_dir = x0 < x1 ? 1 : -1;

    i32 dy = ABS(y1 - y0);
    i32 y_dir = y0 < y1 ? 1 : -1;

    i32 err = (dx > dy ? dx : -dy) / 2;
    i32 x = x0, y = y0;
    while (1) {
        if (x >= 0 && y >= 0) {
            set_braille_at(map, (u32)x, (u32)y);
        }

        if (x == x1 && y == y1) { break; }

        i32 new_err = err;
        if (err > -dx) { 
            new_err -= dy;
            x += x_dir;
        }

        if (err < dy) {
            new_err += dx;
            y += y_dir;
        }

        err = new_err;
    }
}

void braille_map_print(braille_map* map) {
    u8 bytes[3] = { 0 };

    for (u32 y = 0; y < map->char_height; y++) {
        for (u32 x = 0; x < map->char_width; x++) {
            u32 codepoint = 0x2800 + map->dots[x + y * map->char_width];

            bytes[0] = (u8)(0xe0 | (codepoint >> 12));
            bytes[1] = (u8)(0x80 | ((codepoint >> 6) & 0x3f));
            bytes[2] = (u8)(0x80 | (codepoint & 0x3f));

            printf("%.3s", bytes);
        }

        printf("\n");
    }
}

int main(void) {
    SetConsoleOutputCP(CP_UTF8);

    u8 dots[20 * 20] = { 0 };
    braille_map map = { 20, 20, dots };

    set_braille_at(&map, 0, 0);
    set_braille_at(&map, 1, 1);
    set_braille_at(&map, 0, 2);
    set_braille_at(&map, 1, 3);

    set_braille_at(&map, 0, 4);
    set_braille_at(&map, 1, 5);
    set_braille_at(&map, 0, 6);
    set_braille_at(&map, 1, 7);

    braille_line(&map, 5, 5, 30, 75);
    braille_line(&map, 5, 75, 30, 5);

    braille_map_print(&map);

    return 0;
}


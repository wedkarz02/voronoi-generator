#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>

#define WIDTH 1280
#define HEIGHT 720

#define COLOR_RED   0xFF0000FF
#define COLOR_GREEN 0x00FF00FF
#define COLOR_BLUE  0x0000FFFF

static uint32_t image[HEIGHT][WIDTH];

void fill_image(uint32_t color) {
    for (size_t y = 0; y < HEIGHT; y++) {
        for (size_t x = 0; x < WIDTH; x++) {
            image[y][x] = color;
        }
    }
}

void create_ppm_header(const char *file_path) {
    FILE *file = fopen(file_path, "wb");

    if (file == NULL) {
        fprintf(stderr, "ERROR: Could not open the %s file: %s\n",
            file_path,
            strerror(errno)
        );
        exit(1);
    }

    fprintf(file, "P6\n%d %d 255\n", WIDTH, HEIGHT);
    fclose(file);
}

void create_ppm_image(const char *file_path) {
    // create_ppm_header(file_path);

    FILE *file = fopen(file_path, "wb");

    if (file == NULL) {
        fprintf(stderr, "ERROR: Could not open the %s file: %s\n",
            file_path,
            strerror(errno)
        );
        exit(1);
    }

    fprintf(file, "P6\n%d %d 255\n", WIDTH, HEIGHT);

    for (size_t y = 0; y < HEIGHT; y++) {
        for (size_t x = 0; x < WIDTH; x++) {
            uint32_t pixel = image[y][x];
            uint32_t rgb_bytes_arr[3] = {
                (pixel & 0xFF0000) >> 8*0,
                (pixel & 0x00FF00) >> 8*1,
                (pixel & 0x0000FF) >> 8*2
            };
            fwrite(rgb_bytes_arr, sizeof(rgb_bytes_arr), 1, file);
        }
    }

    fclose(file);
}

int main() {
    fill_image(COLOR_BLUE);
    create_ppm_image("generated-voronoi.ppm");
    return 0;
}
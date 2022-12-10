#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define WIDTH 600
#define HEIGHT 400

#define NODE_COUNT 2
#define NODE_RADIUS 3

//                  0xAABBGGRR
#define COLOR_RED   0xFF0000FF
#define COLOR_GREEN 0xFF00FF00
#define COLOR_BLUE  0xFFFF0000

#define COLOR_BG    0xFF2E2E2E

// Color palette
#define COLOR_GREY  0xFF272727
#define COLOR_LBLUE 0xFFAA502B
#define COLOR_LPINK 0xFFE59FFF
#define COLOR_BEIGE 0xFFD4D4FF
#define COLOR_DPINK 0xFF8D85FF

static const int PALETTE[] = {
    COLOR_GREY,
    COLOR_LBLUE,
    COLOR_LPINK,
    COLOR_BEIGE,
    COLOR_DPINK
};

typedef struct {
    int x;
    int y;
    int color;
} Node;

static uint32_t image[HEIGHT][WIDTH];
static Node nodes[NODE_COUNT];

void generate_nodes() {
    for (size_t i = 0; i < NODE_COUNT; i++) {
        nodes[i].x = rand() % WIDTH;
        nodes[i].y = rand() % HEIGHT;
        nodes[i].color = PALETTE[i % 5]; 
    }
}

void fill_image(uint32_t color) {
    for (size_t y = 0; y < HEIGHT; y++) {
        for (size_t x = 0; x < WIDTH; x++) {
            image[y][x] = color;
        }
    }
}

void fill_circle(Node node, int radius, uint32_t color) {
    int x0 = node.x - radius;
    int y0 = node.y - radius;
    int x1 = node.x + radius;
    int y1 = node.y + radius;

    for (int x = x0; x < x1; x++) {
        if (x >= 0 && x < WIDTH) {
            for (int y = y0; y < y1; y++) {
                if (y >= 0 && y < HEIGHT) {
                    int dx = node.x - x;
                    int dy = node.y - y;
                    if (dx*dx + dy*dy <= radius*radius) {
                        image[y][x] = color;
                    }
                }
            }
        }
    }
}

void create_ppm_image(const char *file_path) {
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
            uint8_t rgb_bytes_arr[3] = {
                (pixel & 0x0000FF) >> 8*0,
                (pixel & 0x00FF00) >> 8*1,
                (pixel & 0xFF0000) >> 8*2
            };
            fwrite(rgb_bytes_arr, sizeof(rgb_bytes_arr), 1, file);
        }
    }

    fclose(file);
}

int calculate_distance(int x, int y, Node node) {
    int x_distance = abs(node.x - x);
    int y_distance = abs(node.y - y);
    return sqrt(x_distance*x_distance + y_distance+y_distance);
}

Node find_closest_node(int x, int y) {
    Node closest = nodes[0];
    for (size_t i = 0; i < NODE_COUNT; i++) {
        int closest_distance = calculate_distance(x, y, closest);
        int current_distance = calculate_distance(x, y, nodes[i]);
        if (current_distance < closest_distance) {
            closest = nodes[i];
        }
    }

    return closest;
}

void fill_voronoi() {
    for (size_t y = 0; y < HEIGHT; y++) {
        for (size_t x = 0; x < WIDTH; x++) {
            image[y][x] = find_closest_node(x, y).color;
        }
    }
}

int main() {
    fill_image(COLOR_BG);

    srand((unsigned)time(NULL));
    generate_nodes();
    fill_voronoi();

    for (size_t i = 0; i < NODE_COUNT; i++) {
        fill_circle(nodes[i], NODE_RADIUS, COLOR_RED);
    }

    create_ppm_image("generated-voronoi.ppm");

    return 0;
}

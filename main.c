#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define WIDTH 1280
#define HEIGHT 720

#define NODE_COUNT 50
#define NODE_RADIUS 2

//                    0xAABBGGRR
#define COLOR_RED     0xFF0000FF
#define COLOR_GREEN   0xFF00FF00
#define COLOR_BLUE    0xFFFF0000

#define COLOR_BG      0xFF2E2E2E
#define COLOR_WHITE   0xFFFFFFFF
#define COLOR_BLACK   0xFF000000

// Color palette
#define COLOR_DPINK   0xFF42019E
#define COLOR_LPINK   0xFF4F3ED5
#define COLOR_DORANGE 0xFF436DF4
#define COLOR_LORANGE 0xFF61AEFD
#define COLOR_BEIGE   0xFF8BE0FE
#define COLOR_YELLOW  0xFF98F5E6
#define COLOR_LGREEN  0xFFA4DDAB
#define COLOR_MINT    0xFFA5C266
#define COLOR_LBLUE   0xFFBD8832
#define COLOR_PURPLE  0xFFA24F5E

static const uint32_t PALETTE[] = {
    COLOR_DPINK,
    COLOR_LPINK,
    COLOR_DORANGE,
    COLOR_LORANGE,
    COLOR_BEIGE,
    COLOR_YELLOW,
    COLOR_LGREEN,
    COLOR_MINT,
    COLOR_LBLUE,
    COLOR_PURPLE
};

#define PALETTE_COUNT (sizeof(PALETTE) / sizeof(*PALETTE))

typedef struct {
    int x;
    int y;
    uint32_t color;
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

void fill_circle(Node *node, int radius, uint32_t color) {
    int x0 = node->x - radius;
    int y0 = node->y - radius;
    int x1 = node->x + radius;
    int y1 = node->y + radius;

    for (int x = x0; x < x1; x++) {
        if (x >= 0 && x < WIDTH) {
            for (int y = y0; y < y1; y++) {
                if (y >= 0 && y < HEIGHT) {
                    int dx = node->x - x;
                    int dy = node->y - y;
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

int calculate_square_distance(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    return dx*dx + dy*dy;
}

void fill_voronoi() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int closest_idx = 0;
            for (size_t i = 1; i < NODE_COUNT; i++) {
                int current_distance = calculate_square_distance(nodes[i].x, nodes[i].y, x, y);
                int closest_distance = calculate_square_distance(nodes[closest_idx].x, nodes[closest_idx].y, x, y);
                
                if (current_distance < closest_distance) {
                    closest_idx = i;
                }
            }
            image[y][x] = PALETTE[closest_idx % PALETTE_COUNT];
        }
    }
}

int main() {
    fill_image(COLOR_BG);

    srand((unsigned)time(NULL));
    generate_nodes();
    fill_voronoi();

    for (size_t i = 0; i < NODE_COUNT; i++) {
        fill_circle(&nodes[i], NODE_RADIUS, COLOR_WHITE);
    }

    create_ppm_image("generated-voronoi.ppm");

    return 0;
}

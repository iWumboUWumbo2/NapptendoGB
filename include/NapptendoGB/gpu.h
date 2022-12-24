#ifndef GPU_H
#define GPU_H

#include <stdint.h>

#include "bus.h"

typedef struct gpu gpu_t;

#define LCDC    0xFF40
#define STAT    0xFF41
#define SCY     0xFF42
#define SCX     0xFF43
#define LY      0xFF44
#define LYC     0xFF45
#define BGP     0xFF47
#define OBP0    0xFF48
#define OBP1    0xFF49
#define WY      0xFF4A
#define WX      0xFF4B

#define MODE_2  376
#define MODE_3  204

enum Color {
    WHITE, LIGHT_GRAY, DARK_GRAY, BLACK
};

struct gpu {
   bus_t* bus;
   uint8_t screen_data[144][160][3];
   int32_t scanln_ctr;
};

void gpu_reset_screen(gpu_t* gpu);

uint8_t gpu_lcd_enabled(gpu_t* gpu);
uint8_t gpu_window_enabled(gpu_t* gpu);
uint8_t gpu_obj_enabled(gpu_t* gpu);
uint8_t gpu_bg_enabled(gpu_t* gpu);

void gpu_draw_scanline(gpu_t* gpu);
void gpu_draw_tiles(gpu_t* gpu);
void gpu_draw_sprites(gpu_t* gpu);

enum Color gpu_get_color(gpu_t* gpu, uint8_t color_num, uint16_t addr);
void gpu_set_lcd_status(gpu_t* gpu);
void gpu_update_graphics(gpu_t* gpu);

#endif //GPU_H

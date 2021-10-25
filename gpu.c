#include "gpu.h"

void gpu_reset_screen(gpu_t* gpu) {
    uint8_t x, y;
    for (x = 0 ; x < 144; x++) {
        for (y = 0; y < 160; y++) {
            gpu->screen_data[x][y][0] = 0xFF ;
            gpu->screen_data[x][y][1] = 0xFF ;
            gpu->screen_data[x][y][2] = 0xFF ;
        }
    }
}

uint8_t gpu_lcd_enabled(gpu_t* gpu) {
    return bus_read(gpu->bus, LCDC) & 0x80 ? 1 : 0;
}

uint8_t gpu_window_enabled(gpu_t* gpu) {
    return bus_read(gpu->bus, LCDC) & 0x20 ? 1 : 0;
}

uint8_t gpu_obj_enabled(gpu_t* gpu) {
    return bus_read(gpu->bus, LCDC) & 0x02 ? 1 : 0;
}

uint8_t gpu_bg_enabled(gpu_t* gpu) {
    return bus_read(gpu->bus, LCDC) & 0x01 ? 1 : 0;
}

void gpu_update_graphics(gpu_t* gpu) {
    gpu_set_lcd_status(gpu);
    uint8_t cycles = gpu->bus->cpu->cycles;

    if (gpu_lcd_enabled(gpu)) {
        gpu->scanln_ctr -= cycles;
        if (gpu->scanln_ctr <= 0) {
            gpu->bus->rom[LY]++;

            uint8_t currln = bus_read(gpu->bus, LY);
            gpu->scanln_ctr = 456;

            // Request VBlank interrupt
            if (currln == 0x90) {
                interrupt_req(gpu->bus->interrupt, 0);
            }
            else if (currln >= 0x9A) {
                gpu->bus->rom[LY] = 0;
            }
            else if (currln < 0x90) {
                gpu_draw_scanline(gpu);
            }
        }
    }
}

void gpu_set_lcd_status(gpu_t* gpu) {
    uint8_t status = bus_read(gpu->bus, STAT);
    if (!gpu_lcd_enabled(gpu)) {
        gpu->scanln_ctr = 456;
        gpu->bus->rom[LY] = 0;
        status = status & 0xFC | 1;
    }
    else {
        uint8_t curr_line = bus_read(gpu->bus, LY);
        uint8_t curr_mode = status & 0x03;
        uint8_t mode = 0;
        uint8_t oam_irq = 0,
            vblank_irq = 0,
            hblank_irq = 0;

        if (curr_line >= 144) {
            mode = 1;
            vblank_irq = status & 0x10 ? 1 : 0;
        }
        else {
            if (gpu->scanln_ctr >= MODE_2) {
                mode = 2;
                oam_irq = status & 0x20 ? 1 : 0;
            }
            else if (gpu->scanln_ctr >= MODE_3) {
                mode = 3;
            }
            else {
                mode = 0;
                hblank_irq = status & 0x08 ? 1 : 0;
            }
        }
        status = status & 0xFC | mode;

        if ((oam_irq || vblank_irq || hblank_irq) && (mode != curr_mode))
            interrupt_req(gpu->bus->interrupt, 1);
        if (curr_line == bus_read(gpu->bus, LYC)) {
            status |= 4;
            if (status & 0x40)
                interrupt_req(gpu->bus->interrupt, 1);
        }
        else {
            status &= ~4;
        }
    }
    bus_write(gpu->bus, STAT, status);
}

void gpu_draw_scanline(gpu_t* gpu) {
    if (gpu_bg_enabled(gpu))
        gpu_draw_tiles(gpu);

    if (gpu_obj_enabled(gpu))
        gpu_draw_sprites(gpu);
}

void gpu_draw_tiles(gpu_t* gpu) {
    uint8_t lcdc = bus_read(gpu->bus, LCDC);

    uint8_t scrX = bus_read(gpu->bus, SCX);
    uint8_t scrY = bus_read(gpu->bus, SCY);
    uint8_t winX = bus_read(gpu->bus, WX) - 7;
    uint8_t winY= bus_read(gpu->bus, WY);

    uint8_t using_window =
            gpu_window_enabled(gpu) && (winY <= bus_read(gpu->bus, LY));


    uint8_t is_unsign_addr;
    uint16_t bgwin_tile_data;
    if (lcdc & 0x10) {
        bgwin_tile_data = 0x8000;
        is_unsign_addr = 1;
    }
    else {
        bgwin_tile_data = 0x8800;
        is_unsign_addr = 0;
    }

    uint16_t bg_mem = (lcdc & (using_window ? 0x40 : 0x08)) ? 0x9C00 : 0x9800;

    uint8_t ly = bus_read(gpu->bus, LY);
    uint8_t posY = (!using_window) ? (scrY + ly) : (ly - winY);
    uint16_t tile_row = ((uint16_t) (posY >> 3)) << 5;

    int pix;
    for (pix = 0; pix < 160; pix++) {
        uint8_t posX = pix + scrX;
        if (using_window && pix >= winX) {
            posX = pix - winX;
        }

        uint16_t tile_col = posX / 8;
        uint16_t tile_adr = bg_mem + tile_row + tile_col;
        uint16_t tile_num = bus_read(gpu->bus, tile_adr);
        if (is_unsign_addr) {
            tile_num |= (tile_num & 0x80) ? 0xFF00 : 0x0000;
        }

        uint16_t tile_loc =
                bgwin_tile_data + ((tile_num + (is_unsign_addr ? 0 : 128)) * 16);
        uint8_t line = (posY % 8) * 2;
        uint8_t data1 = bus_read(gpu->bus, tile_loc + line),
                data2 = bus_read(gpu->bus, tile_loc + line + 1);
        uint8_t color_bit = 7 - posX % 8;
        uint8_t color_num =
                (((data2 >> color_bit) & 1) << 1) | ((data1 >> color_bit) & 1);

        enum Color color = gpu_get_color(gpu, color_num, BGP);
        uint8_t red, green, blue;
        switch (color) {
            case WHITE:
                red = green = blue = 0xFF;
                break;
            case LIGHT_GRAY:
                red = green = blue = 0xCC;
                break;
            case DARK_GRAY:
                red = green = blue = 0x77;
                break;
            case BLACK:
            default:
                red = green = blue = 0x00;
                break;
        }

        ly = bus_read(gpu->bus, LY);
        if (ly < 0 || ly > 143 || pix < 0 || pix > 159)
            continue;

        gpu->screen_data[pix][ly][0] = red;
        gpu->screen_data[pix][ly][1] = green;
        gpu->screen_data[pix][ly][2] = blue;
    }
}

enum Color gpu_get_color(gpu_t* gpu, uint8_t color_num, uint16_t addr) {
    uint8_t palette = bus_read(gpu->bus, addr);
    uint8_t color = (palette >> (2 * color_num)) & 3;
    return color;
}

void gpu_draw_sprites(gpu_t* gpu) {
    uint8_t is8x16 = (bus_read(gpu->bus, LCDC) >> 1) & 1;
    uint8_t sprite;
    for (sprite = 0; sprite < 40; sprite++) {
        uint8_t index = sprite * 4;
        uint8_t posX = bus_read(gpu->bus, 0xFE00 + index + 1) - 8;
        uint8_t posY = bus_read(gpu->bus, 0xFE00 + index) - 16;
        uint8_t tileloc = bus_read(gpu->bus, 0xFE00 + index + 2);
        uint8_t attr = bus_read(gpu->bus, 0xFE00 + index + 3);

        uint8_t flipX = attr & 0x40 ? 1 : 0;
        uint8_t flipY = attr & 0x20 ? 1 : 0;

        uint8_t szY = (is8x16) ? 16 : 8;
        uint8_t ly = bus_read(gpu->bus, LY);

        if (ly >= posY && ly < (posY + szY)) {
            uint8_t curr_line = ly - posY;
            if (flipY)
                curr_line = szY - curr_line;
            curr_line *= 2;

            uint16_t data_addr = curr_line + (0x8000 + tileloc * 16);
            uint8_t data1 = bus_read(gpu->bus, data_addr);
            uint8_t data2 = bus_read(gpu->bus, data_addr + 1);

            uint8_t tilepx;
            for (tilepx = 7; tilepx >= 0; tilepx--) {
                uint8_t color_bit = tilepx;
                if (flipX)
                    color_bit = 7 - color_bit;
                uint8_t color_num =
                        (((data2 >> color_bit) & 1) << 1) | ((data1 >> color_bit) & 1);
                uint16_t color_addr = (attr & 0x10) ? 0xFF49 : 0xFF48;
                enum Color color = gpu_get_color(gpu, color_num, color_addr);
                if (color == WHITE) continue;

                uint8_t red, green, blue;
                switch (color) {
                    case LIGHT_GRAY:
                        red = green = blue = 0xCC;
                        break;
                    case DARK_GRAY:
                        red = green = blue = 0x77;
                        break;
                    case BLACK:
                        red = green = blue = 0x00;
                        break;
                    default:
                        red = green = blue = 0xFF;
                        break;
                }

                uint8_t pixX = 7 - tilepx;
                uint8_t pix = pixX + posX;

                ly = bus_read(gpu->bus, LY);
                if (ly < 0 || ly > 143 || pix < 0 || pix > 159)
                    continue;

                gpu->screen_data[pix][ly][0] = red;
                gpu->screen_data[pix][ly][1] = green;
                gpu->screen_data[pix][ly][2] = blue;
            }
        }
    }
}
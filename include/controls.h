#ifndef CONTROLS_H
#define CONTROLS_H

#include "stdint.h"
#include "types.h"

#define JOYP 0xFF00

// Joypad state
extern uint8_t joypad_state;

uint8_t controls_get_state(bus_t* bus);
void controls_update(uint8_t keys);

// Key bits
#define KEY_A      0x01
#define KEY_B      0x02  
#define KEY_SELECT 0x04
#define KEY_START  0x08
#define KEY_RIGHT  0x10
#define KEY_LEFT   0x20
#define KEY_UP     0x40
#define KEY_DOWN   0x80

#endif

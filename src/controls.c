#include "controls.h"
#include "bus.h"

uint8_t joypad_state = 0xFF; // All buttons released initially

void controls_update(uint8_t keys) {
    joypad_state = keys;
}

uint8_t controls_get_state(bus_t* bus) {
    uint8_t joyp = bus->rom[JOYP]; // Read directly from ROM to avoid recursion
    uint8_t result = 0xFF;
    
    // Check which button group is being read
    if (!(joyp & 0x10)) { // Direction keys
        result &= ~((joypad_state & KEY_DOWN)  ? 0x08 : 0);
        result &= ~((joypad_state & KEY_UP)    ? 0x04 : 0);
        result &= ~((joypad_state & KEY_LEFT)  ? 0x02 : 0);
        result &= ~((joypad_state & KEY_RIGHT) ? 0x01 : 0);
    }
    
    if (!(joyp & 0x20)) { // Action buttons  
        result &= ~((joypad_state & KEY_START)  ? 0x08 : 0);
        result &= ~((joypad_state & KEY_SELECT) ? 0x04 : 0);
        result &= ~((joypad_state & KEY_B)      ? 0x02 : 0);
        result &= ~((joypad_state & KEY_A)      ? 0x01 : 0);
    }
    
    return result;
}
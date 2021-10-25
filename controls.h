#ifndef CONTROLS_H
#define CONTROLS_H

#include "stdint.h"

typedef struct bus bus_t;

#define JOYP 0xFF00

uint8_t controls_get_state(bus_t* bus);
//uint8_t controls_keypress()
//uint8_t controls_keyrelease()

#endif

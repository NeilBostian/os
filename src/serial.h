#ifndef SERIAL_H
#define SERIAL_H

#include "types.h"

void outb(uint16 port, uint8 val);
uint8 inb(uint16 port);

#endif
#ifndef ATA_H
#define ATA_H

#include "types.h"

uint8 get_status_register();
uint8 get_error_register();
void test_atapio();

#endif
#pragma once

#include "types.h"

void ata_handle_irq();
uint8 get_status_register();
uint8 get_error_register();
void test_ata();

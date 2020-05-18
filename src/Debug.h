#pragma once

#include "Types.h"

extern "C"
{
    // Returns a pointer to the next address that will be executed after this function is called.
    // AKA - returns the return address of this function
    uint32 dbg_getreg_eip();

    // Returns register %ebp
    uint32 dbg_getreg_ebp();
}

// Prints memory to the console in a readable format
void dbg_print_memory(void *ptr, uint32 num_bytes);

// Prints stack to the console in a readable format
void dbg_print_stack(uint32 num_items);

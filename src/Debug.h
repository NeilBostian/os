#pragma once

#include "Types.h"

class Debug
{
public:
    static void PrintMemory(void *ptr, uint32 num_bytes);
    static void PrintStack(uint32 num_items);
    static uint32 GetRegEIP();
    static uint32 GetRegEBP();
};

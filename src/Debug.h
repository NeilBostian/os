#pragma once

#include <Types.h>
#include <Drivers/Elf.h>

class Debug
{
public:
    static elf32_symtab_entry *GetSymbolFromAddress(uint32 addr);

    static void PrintMemory(void *ptr, uint32 num_bytes);
    static void PrintStack(uint32 num_items);
    static void PrintStack(uint32 num_items, uint32 ebp);
    static void PrintCallStack(uint32 num_items);
    static void PrintCallStack(uint32 num_items, uint32 ebp);

    static void WriteSymbol();
    static void WriteSymbol(uint32 eip);
};

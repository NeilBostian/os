#pragma once

#include <Types.h>

typedef struct
{
    uint32 cr2;
    uint32 ebp;
    uint32 edi;
    uint32 esi;
    uint32 edx;
    uint32 ecx;
    uint32 ebx;
    uint32 eax;
} __attribute__((packed)) registers;

class Cpu
{
public:
    static void Initialize();
    static void Panic();
    static void HandleInterrupt(registers cpu, uint32 isr, uint32 error_code, uint32 eip);

private:
    static void InitGdt();
    static void InitIdt();
    static void PanicInternal();
};

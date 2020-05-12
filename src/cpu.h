#ifndef CPU_H
#define CPU_H

#include "types.h"

typedef struct
{
    uint16 limit_0_15; // bits 0-15 of limit
    uint16 base_0_15;
    uint8 base_16_23;
    uint8 access_byte;
    uint8 flags_and_limit_16_19;
    uint8 base_24_31;
} __attribute__((packed)) gdt_entry;

typedef struct
{
    uint16 limit;
    gdt_entry *address;
} __attribute__((packed)) gdt_descriptor;

typedef struct
{
    uint16 offset_1; // offset bits 0..15
    uint16 selector; // a code segment selector in GDT or LDT
    uint8 zero;      // unused, set to 0
    uint8 type_attr; // type and attributes, see below
    uint16 offset_2; // offset bits 16..31
} __attribute__((packed)) idt_entry;

typedef struct
{
    uint16 limit;
    idt_entry *address;
} __attribute__((packed)) idt_descriptor;

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
} __attribute__((packed)) cpu_state;

void create_gdt();
void create_idt();
void panic();

#endif

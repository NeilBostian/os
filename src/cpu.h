#pragma once

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

extern "C"
{
    void reload_segments();

    void interrupt_handler_0();
    void interrupt_handler_1();
    void interrupt_handler_2();
    void interrupt_handler_3();
    void interrupt_handler_4();
    void interrupt_handler_5();
    void interrupt_handler_6();
    void interrupt_handler_7();
    void interrupt_handler_8();
    void interrupt_handler_9();
    void interrupt_handler_10();
    void interrupt_handler_11();
    void interrupt_handler_12();
    void interrupt_handler_13();
    void interrupt_handler_14();
    void interrupt_handler_15();
    void interrupt_handler_16();
    void interrupt_handler_17();
    void interrupt_handler_18();
    void interrupt_handler_19();
    void interrupt_handler_20();
    void interrupt_handler_21();
    void interrupt_handler_22();
    void interrupt_handler_23();
    void interrupt_handler_24();
    void interrupt_handler_25();
    void interrupt_handler_26();
    void interrupt_handler_27();
    void interrupt_handler_28();
    void interrupt_handler_29();
    void interrupt_handler_30();
    void interrupt_handler_31();
    void interrupt_handler_32();
    void interrupt_handler_33();
    void interrupt_handler_34();
    void interrupt_handler_35();
    void interrupt_handler_36();
    void interrupt_handler_37();
    void interrupt_handler_38();
    void interrupt_handler_39();
    void interrupt_handler_40();
    void interrupt_handler_41();
    void interrupt_handler_42();
    void interrupt_handler_43();
    void interrupt_handler_44();
    void interrupt_handler_45();
    void interrupt_handler_46();
    void interrupt_handler_47();
    void interrupt_handler_48();
    void interrupt_handler_49();
    void interrupt_handler_50();
    void interrupt_handler_51();
    void interrupt_handler_52();
    void interrupt_handler_53();
    void interrupt_handler_54();
    void interrupt_handler_55();
    void interrupt_handler_56();
    void interrupt_handler_57();
    void interrupt_handler_58();
    void interrupt_handler_59();
    void interrupt_handler_60();
    void interrupt_handler_61();
    void interrupt_handler_62();
    void interrupt_handler_63();
}

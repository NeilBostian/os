.intel_syntax noprefix

.global reload_segments
.global isr_handle

.extern interrupt_handler

.section .text
reload_segments:
    /*
        Long-jump to the segment for kernel code (GDT offset 0x8)
        Have to use jump to update the code segment (cs) register.
        All other segment registers can be assigned explicitly (reload_cs below)
        Other segment registers point to segment for kernel data (GDT offset 0x10 DEC 16)
    */
    jmp 0x08:reload_cs

reload_cs:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret

.global interrupt_handler_0
interrupt_handler_0:
    push 0
    push 0
    jmp common_interrupt_handler

.global interrupt_handler_1
interrupt_handler_1:
    push 0
    push 1
    jmp common_interrupt_handler

.global interrupt_handler_2
interrupt_handler_2:
    push 0
    push 2
    jmp common_interrupt_handler

.global interrupt_handler_3
interrupt_handler_3:
    push 0
    push 3
    jmp common_interrupt_handler

.global interrupt_handler_4
interrupt_handler_4:
    push 0
    push 4
    jmp common_interrupt_handler

.global interrupt_handler_5
interrupt_handler_5:
    push 0
    push 5
    jmp common_interrupt_handler

.global interrupt_handler_6
interrupt_handler_6:
    push 0
    push 6
    jmp common_interrupt_handler

.global interrupt_handler_7
interrupt_handler_7:
    push 0
    push 7
    jmp common_interrupt_handler

.global interrupt_handler_8
interrupt_handler_8:
    push 0
    push 8
    jmp common_interrupt_handler

.global interrupt_handler_9
interrupt_handler_9:
    push 0
    push 9
    jmp common_interrupt_handler

.global interrupt_handler_10
interrupt_handler_10:
    push 0
    push 10
    jmp common_interrupt_handler

.global interrupt_handler_11
interrupt_handler_11:
    push 0
    push 11
    jmp common_interrupt_handler

.global interrupt_handler_12
interrupt_handler_12:
    push 0
    push 12
    jmp common_interrupt_handler

.global interrupt_handler_13
interrupt_handler_13:
    push 0
    push 13
    jmp common_interrupt_handler

.global interrupt_handler_14
interrupt_handler_14:
    # don't push an error code, the cpu will do that
    push 14
    jmp common_interrupt_handler

.global interrupt_handler_15
interrupt_handler_15:
    push 0
    push 15
    jmp common_interrupt_handler

.global interrupt_handler_16
interrupt_handler_16:
    push 0
    push 16
    jmp common_interrupt_handler

.global interrupt_handler_17
interrupt_handler_17:
    push 0
    push 17
    jmp common_interrupt_handler

.global interrupt_handler_18
interrupt_handler_18:
    push 0
    push 18
    jmp common_interrupt_handler

.global interrupt_handler_19
interrupt_handler_19:
    push 0
    push 19
    jmp common_interrupt_handler

.global interrupt_handler_20
interrupt_handler_20:
    push 0
    push 20
    jmp common_interrupt_handler

.global interrupt_handler_21
interrupt_handler_21:
    push 0
    push 21
    jmp common_interrupt_handler

.global interrupt_handler_22
interrupt_handler_22:
    push 0
    push 22
    jmp common_interrupt_handler

.global interrupt_handler_23
interrupt_handler_23:
    push 0
    push 23
    jmp common_interrupt_handler

.global interrupt_handler_24
interrupt_handler_24:
    push 0
    push 24
    jmp common_interrupt_handler

.global interrupt_handler_25
interrupt_handler_25:
    push 0
    push 25
    jmp common_interrupt_handler

.global interrupt_handler_26
interrupt_handler_26:
    push 0
    push 26
    jmp common_interrupt_handler

.global interrupt_handler_27
interrupt_handler_27:
    push 0
    push 27
    jmp common_interrupt_handler

.global interrupt_handler_28
interrupt_handler_28:
    push 0
    push 28
    jmp common_interrupt_handler

.global interrupt_handler_29
interrupt_handler_29:
    push 0
    push 29
    jmp common_interrupt_handler

.global interrupt_handler_30
interrupt_handler_30:
    push 0
    push 30
    jmp common_interrupt_handler

.global interrupt_handler_31
interrupt_handler_31:
    push 0
    push 31
    jmp common_interrupt_handler

.global interrupt_handler_32
interrupt_handler_32:
    push 0
    push 32
    jmp common_interrupt_handler

.global interrupt_handler_33
interrupt_handler_33:
    push 0
    push 33
    jmp common_interrupt_handler

.global interrupt_handler_34
interrupt_handler_34:
    push 0
    push 34
    jmp common_interrupt_handler

.global interrupt_handler_35
interrupt_handler_35:
    push 0
    push 35
    jmp common_interrupt_handler

.global interrupt_handler_36
interrupt_handler_36:
    push 0
    push 36
    jmp common_interrupt_handler

.global interrupt_handler_37
interrupt_handler_37:
    push 0
    push 37
    jmp common_interrupt_handler

.global interrupt_handler_38
interrupt_handler_38:
    push 0
    push 38
    jmp common_interrupt_handler

.global interrupt_handler_39
interrupt_handler_39:
    push 0
    push 39
    jmp common_interrupt_handler

.global interrupt_handler_40
interrupt_handler_40:
    push 0
    push 40
    jmp common_interrupt_handler

.global interrupt_handler_41
interrupt_handler_41:
    push 0
    push 41
    jmp common_interrupt_handler

.global interrupt_handler_42
interrupt_handler_42:
    push 0
    push 42
    jmp common_interrupt_handler

.global interrupt_handler_43
interrupt_handler_43:
    push 0
    push 43
    jmp common_interrupt_handler

.global interrupt_handler_44
interrupt_handler_44:
    push 0
    push 44
    jmp common_interrupt_handler

.global interrupt_handler_45
interrupt_handler_45:
    push 0
    push 45
    jmp common_interrupt_handler

.global interrupt_handler_46
interrupt_handler_46:
    push 0
    push 46
    jmp common_interrupt_handler

.global interrupt_handler_47
interrupt_handler_47:
    push 0
    push 47
    jmp common_interrupt_handler

.global interrupt_handler_48
interrupt_handler_48:
    push 0
    push 48
    jmp common_interrupt_handler

.global interrupt_handler_49
interrupt_handler_49:
    push 0
    push 49
    jmp common_interrupt_handler

.global interrupt_handler_50
interrupt_handler_50:
    push 0
    push 50
    jmp common_interrupt_handler

.global interrupt_handler_51
interrupt_handler_51:
    push 0
    push 51
    jmp common_interrupt_handler

.global interrupt_handler_52
interrupt_handler_52:
    push 0
    push 52
    jmp common_interrupt_handler

.global interrupt_handler_53
interrupt_handler_53:
    push 0
    push 53
    jmp common_interrupt_handler

.global interrupt_handler_54
interrupt_handler_54:
    push 0
    push 54
    jmp common_interrupt_handler

.global interrupt_handler_55
interrupt_handler_55:
    push 0
    push 55
    jmp common_interrupt_handler

.global interrupt_handler_56
interrupt_handler_56:
    push 0
    push 56
    jmp common_interrupt_handler

.global interrupt_handler_57
interrupt_handler_57:
    push 0
    push 57
    jmp common_interrupt_handler

.global interrupt_handler_58
interrupt_handler_58:
    push 0
    push 58
    jmp common_interrupt_handler

.global interrupt_handler_59
interrupt_handler_59:
    push 0
    push 59
    jmp common_interrupt_handler

.global interrupt_handler_60
interrupt_handler_60:
    push 0
    push 60
    jmp common_interrupt_handler

.global interrupt_handler_61
interrupt_handler_61:
    push 0
    push 61
    jmp common_interrupt_handler

.global interrupt_handler_62
interrupt_handler_62:
    push 0
    push 62
    jmp common_interrupt_handler

.global interrupt_handler_63
interrupt_handler_63:
    push 0
    push 63
    jmp common_interrupt_handler

common_interrupt_handler:
    # save the registers
    push eax
    push ebx
    push ecx
    push edx
    push esi
    push edi
    push ebp
    mov eax, cr2
    push eax

    # call the C function
    call interrupt_handler

    # restore the registers
    add esp, 4 # cr2
    pop ebp
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax

    # pop error_code and interrupt_number off the stack
    add esp, 8

    # return to the code that got interrupted
    iret

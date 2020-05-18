.intel_syntax noprefix

.global cpu_reload_segments

.section .text
cpu_reload_segments:
    /*
        Long-jump to the segment for kernel code (GDT offset 0x8)
        Have to use jump to update the code segment (cs) register.
        All other segment registers can be assigned explicitly (reload_cs below)
        Other segment registers point to segment for kernel data (GDT offset 0x10 DEC 16)
    */
    jmp 0x08:cpu_reload_cs

cpu_reload_cs:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret

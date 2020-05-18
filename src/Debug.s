.intel_syntax noprefix

.global debug_getreg_eip
.global debug_getreg_ebp

.section .text
debug_getreg_ebp:
    mov eax, ebp
    ret

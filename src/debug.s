.intel_syntax noprefix

.global dbg_getreg_eip
.global dbg_getreg_ebp

.section .text
dbg_getreg_eip:
    mov eax, [esp]
    ret

dbg_getreg_ebp:
    mov eax, ebp
    ret
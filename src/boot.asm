bits 32
section .multiboot2
align 8
mb2_start:
    dd 0xe85250d6
    dd 0
    dd mb2_end - mb2_start
    dd -(0xe85250d6 + 0 + (mb2_end - mb2_start))

    ; Request 1024x768x32 linear framebuffer
    align 8
    dw 5
    dw 0
    dd 20
    dd 1024
    dd 768
    dd 32

    ; Multiboot2 end tag
    align 8
    dw 0
    dw 0
    dd 8
mb2_end:

section .bss
align 4096
pml4: resb 4096
pdpt: resb 4096
pd:   resb 4096 * 4
stack_bottom:
    resb 16384
stack_top:

section .rodata
gdt64:
    dq 0
.code: equ $ - gdt64
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53)
.pointer:
    dw $ - gdt64 - 1
    dq gdt64

section .text
global _start
extern kernel_main

_start:
    mov esp, stack_top
    mov edi, ebx

    ; Identity-map first 4 GiB using 2 MiB pages
    mov eax, pdpt
    or eax, 0b11
    mov [pml4], eax

    xor ecx, ecx
.map_pdpt:
    mov eax, 4096
    mul ecx
    add eax, pd
    or eax, 0b11
    mov [pdpt + ecx * 8], eax
    inc ecx
    cmp ecx, 4
    jne .map_pdpt

    xor ecx, ecx
.map_pd:
    mov eax, 0x200000
    mul ecx
    or eax, 0b10000011
    mov [pd + ecx * 8], eax
    mov [pd + ecx * 8 + 4], edx
    inc ecx
    cmp ecx, 2048
    jne .map_pd

    ; Enable PAE
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; Load page table root
    mov eax, pml4
    mov cr3, eax

    ; Enable Long Mode
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; Enable Paging
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    lgdt [gdt64.pointer]
    jmp gdt64.code:long_mode_start

bits 64
long_mode_start:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov rsp, stack_top
    mov edi, edi
    call kernel_main
.halt:
    cli
    hlt
    jmp .halt

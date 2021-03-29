.set IRQ_BASE, 0x20

.section .text

.extern _ZN16InterruptManager15handleInterruptEhj

.macro handleException num
.global _ZN16InterruptManager19handleException\num\()Ev
_ZN16InterruptManager19handleException\num\()Ev:
    movb $\num, (interruptnumber)
    jmp int_bottom
.endm


.macro handleInterrupt num
.global _ZN16InterruptManager19handleInterrupt\num\()Ev
_ZN16InterruptManager19handleInterrupt\num\()Ev:
    movb $\num + IRQ_BASE, (interruptnumber)
    pushl $0
    jmp int_bottom
.endm

handleException 0x00
handleException 0x01
handleException 0x02
handleException 0x03
handleException 0x04
handleException 0x05
handleException 0x06
handleException 0x07
handleException 0x08
handleException 0x09
handleException 0x0A
handleException 0x0B
handleException 0x0C
handleException 0x0D
handleException 0x0E
handleException 0x0F
handleException 0x10
handleException 0x11
handleException 0x12
handleException 0x13

handleInterrupt 0x00
handleInterrupt 0x01
handleInterrupt 0x02
handleInterrupt 0x03
handleInterrupt 0x04
handleInterrupt 0x05
handleInterrupt 0x06
handleInterrupt 0x07
handleInterrupt 0x08
handleInterrupt 0x09
handleInterrupt 0x0A
handleInterrupt 0x0B
handleInterrupt 0x0C
handleInterrupt 0x0D
handleInterrupt 0x0E
handleInterrupt 0x0F
handleInterrupt 0x31
handleInterrupt 0x80

int_bottom:
    pushl %ebp
    pushl %edi
    pushl %esi

    pushl %edx
    pushl %ecx
    pushl %ebx
    pushl %eax

    pushl %esp
    push (interruptnumber)
    call _ZN16InterruptManager15handleInterruptEhj
    mov %eax, %esp

    popl %eax
    popl %ebx
    popl %ecx
    popl %edx

    popl %esi
    popl %edi
    popl %ebp

    add $4, %esp

.global _ZN16InterruptManager15ignoreInterruptEv
_ZN16InterruptManager15ignoreInterruptEv:
    iret

.data
    interruptnumber: .byte 0
; Ralph Doncaster 2020 MIT License open source
; print functions
; uses external write_r18 function
; 20200414 - working versions of print for u8b10, u8b16, and flash strings

.macro addi Rd, K
    subi \Rd, -(\K)
.endm

.section .text.printu16b10

; print number in 22:21 as unsigned decimal (base 10): 31 instructions
; optimized from Peter Dannegger's version
; clobbers r20
.global printu16b10_r22
printu16b10_r22:
    clt                                 ; T set for non-zero digit
    ldi r20, -1 + '0'                   ; ten-thousands
1:  inc r20
    subi r22, lo8(10000)
    sbci r23, hi8(10000)
    brcc 1b
    rcall skip_leading0

    ldi r20, 10 + '0'                   ; thousands
2:  dec r20
    subi r22, lo8(-1000)
    sbci r23, hi8(-1000)
    brcs 2b
    rcall skip_leading0

    ldi r20, -1 + '0'                   ; hundreds
3:  inc r20
    subi r22, lo8(100)
    sbci r23, hi8(100)
    brcc 3b
    rcall skip_leading0

    ldi r20, 10 + '0'                   ; tens
4:  dec r20
    addi r22, 10
    brcs 4b
    rcall skip_leading0

    addi r22, '0'                       ; ones
    mov r20, r22
putc:
    rjmp write_r20

skip_leading0:
    brts putc
    cpi r20, '0'
    brne putc                           ; write_r20 leaves T set
    ret


.section .text.printu8b16

; print number in r20 as unsigned hex (base 16): 10 instructions
.global printu8b16_r20
printu8b16_r20:
    push r20
    swap r20
    rcall nibbletohex                   ; convert hi digit
    pop r20
    ; fall into nibbletohex to convert lo digit
nibbletohex:
    andi r20, 0x0F
    cpi r20, 10
    brlo 1f 
    addi r20, 'A'-':'
1:  ; less than 10
    addi r20, '0'
    rjmp write_r20


.section .text.printsp

; print null-terminated string in progmem, pointer in Z: 5 instructions
; clobbers r20 & Z
.global printsp_z
printsp_z_write:
    rcall write_r20
printsp_z:
#if defined(__AVR_TINY__)
    ld r20, Z+                          ; read next char
#else
    lpm r20, Z+                         ; read next char
#endif
    tst r20
    brne printsp_z_write
    ret


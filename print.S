; Ralph Doncaster 2020 MIT License open source
; print functions
; uses external write_r18 function
; 20200414 - working versions of print for u8b10, u8b16, and flash strings

#if defined(__AVR_TINY__)
#define TX_REG r25
#else
#define TX_REG r18
#endif

.macro addi Rd, K
    subi \Rd, -(\K)
.endm

.section .text.printu16b10

; print number in 21:20 as unsigned decimal (base 10): 31 instructions
; optimized from Peter Dannegger's version
; clobbers r18 (r25 on avrtiny)
.global printu16b10_r20
printu16b10_r20:
    clt                                 ; T set for non-zero digit
    ldi TX_REG, -1 + '0'                ; ten-thousands
1:  inc TX_REG
    subi r20, lo8(10000)
    sbci r21, hi8(10000)
    brcc 1b
    rcall skip_leading0

    ldi TX_REG, 10 + '0'                ; thousands
2:  dec TX_REG
    subi r20, lo8(-1000)
    sbci r21, hi8(-1000)
    brcs 2b
    rcall skip_leading0

    ldi TX_REG, -1 + '0'                ; hundreds
3:  inc TX_REG
    subi r20, lo8(100)
    sbci r21, hi8(100)
    brcc 3b
    rcall skip_leading0

    ldi TX_REG, 10 + '0'                ; tens
4:  dec TX_REG
    addi r20, 10
    brcs 4b
    rcall skip_leading0

    addi r20, '0'                       ; ones
    mov TX_REG, r20
putc:
    rjmp write_r18

skip_leading0:
    brts putc
    cpi TX_REG, '0'
    brne putc                           ; write_r18 leaves T set
    ret


.section .text.printu8b16

; print number in r18 (r25 on avrtiny) as unsigned hex (base 16): 10 instructions
.global printu8b16_r18
printu8b16_r18:
    push TX_REG
    swap TX_REG
    rcall nibbletohex                   ; convert hi digit
    pop TX_REG
    ; fall into nibbletohex to convert lo digit
nibbletohex:
    andi TX_REG, 0x0F
    cpi TX_REG, 10
    brlo 1f 
    addi TX_REG, 'A'-':'
1:  ; less than 10
    addi TX_REG, '0'
    rjmp write_r18


.section .text.printsp

; print null-terminated string in progmem, pointer in Z: 5 instructions
; clobbers r18 (r25 on avrtiny) & Z
.global printsp_z
printsp_z_write:
    rcall write_r18
printsp_z:
#if defined(__AVR_TINY__)
    ld TX_REG, Z+                       ; read next char
#else
    lpm TX_REG, Z+                      ; read next char
#define TX_REG r18
#endif
    tst TX_REG
    brne printsp_z_write
    ret


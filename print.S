; Ralph Doncaster 2020 MIT License open source
; print functions
; uses external write_r18 function
; 20200414 - working versions of print for u8b10, u8b16, and flash strings

.macro addi Rd, K
    subi \Rd, -(\K)
.endm

.section .text.printu16b10

; print number in 21:20 as unsigned decimal (base 10): 31 instructions
; optimized from Peter Dannegger's version
; clobbers r18
.global printu16b10_r20
printu16b10_r20:
    clt                                 ; T set for non-zero digit
    ldi r18, -1 + '0'                   ; ten-thousands
1:  inc r18
    subi r20, lo8(10000)
    sbci r21, hi8(10000)
    brcc 1b
    rcall skip_leading0

    ldi r18, 10 + '0'                   ; thousands
2:  dec r18
    subi r20, lo8(-1000)
    sbci r21, hi8(-1000)
    brcs 2b
    rcall skip_leading0

    ldi r18, -1 + '0'                   ; hundreds
3:  inc r18
    subi r20, lo8(100)
    sbci r21, hi8(100)
    brcc 3b
    rcall skip_leading0

    ldi r18, 10 + '0'                   ; tens
4:  dec r18
    addi r20, 10
    brcs 4b
    rcall skip_leading0

    addi r20, '0'                       ; ones
    mov r18, r20
putc:
    rjmp write_r18

skip_leading0:
    brts putc
    cpi r18, '0'
    brne putc                           ; write_r18 leaves T set
    ret


.section .text.printu8b16

; print number in r18 as unsigned hex (base 16): 10 instructions
.global printu8b16_r18
printu8b16_r18:
    push r18
    swap r18
    rcall nibbletohex                   ; convert hi digit
    pop r18
    ; fall into nibbletohex to convert lo digit
nibbletohex:
    andi r18, 0x0F
    cpi r18, 10
    brlo 1f 
    addi r18, 'A'-':'
1:  ; less than 10
    addi r18, '0'
    rjmp write_r18


.section .text.printsp

; print null-terminated string in progmem, pointer in Z: 5 instructions
; clobbers r18 & Z
.global printsp_z
printsp_z_write:
    rcall write_r18
printsp_z:
    lpm r18, Z+                         ; read next char
    tst r18
    brne printsp_z_write
    ret


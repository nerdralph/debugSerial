; Ralph Doncaster 2020 MIT License open source
; print functions
; uses external write_r18 function

.macro addi Rd, K
    subi \Rd, -(\K)
.endm

.section .text.printu8b10

; print number in r19 as unsigned decimal (base 10): 13 instructions
; clobbers r18
.global printu8b10_r19
printu8b10_r19:
    clt                                 ; flag non-zero digit
    ldi r18, -1 + '0'                   ; hundreds
1:  inc r18
    subi r19, 100
    brcc 1b
    rcall skip_leading0

    ldi r18, 10 + '0'                   ; tens
2:  dec r18
    addi r19, 10
    brcs 2b
    rcall skip_leading0

    addi r19, '0'                       ; ones
    mov r18, r19
3:  rjmp write_r18

skip_leading0:
    brts 3b
    cpi r18, '0'
    brne 3b                             ; write_r18 leaves T set
    ret


.section .text.printu8b16

; print number in r19 as unsigned hex (base 16): 10 instructions
; clobbers r18
.global printu8b16_r19
printu8b16_r19:
    mov r18, r19
    swap r18
    rcall nibbletohex                   ; convert hi digit
    mov r18, r19
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
printsp_z_begin:
    rcall write_r18
printsp_z:
    lpm r18, Z+                         ; read next char
    tst r18
    brne printsp_z_begin
    ret


    .org $8000

main:
    lda #$04
    sta $00
    lda #$02
    sta $01
    jsr mul
    sta $02
    jmp end_loop

mul:
    lda #$0
    ldx $0
mul_loop:
    cpx #$0
    beq mul_ret
    dex 
    clc 
    adc $1
    jmp mul_loop
mul_ret:
    rts 

end_loop:
    nop 
    jmp end_loop
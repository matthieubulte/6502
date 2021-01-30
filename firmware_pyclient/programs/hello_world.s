vram .eq $7c40
prog_start .eq $8000


    .org prog_start
    
main:
    ldx #$0

print_loop:
    lda disp_str,X
    cmp #$0
    beq end_loop
    sta vram,X
    inx
    jmp print_loop

end_loop:
    nop 
    jmp end_loop

disp_str:
    .string "Hello World!"
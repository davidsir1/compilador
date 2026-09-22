.text
main:
    li t$0, 0
    li t$1, 10
loop:
    addi $t0, t$0, 1
    bne $t0, $t1, loop
    j fim
fim:
    li $v0, 10
    syscall

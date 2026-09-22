.data
    vetor: .word 1, 2, 3, 4, 5

.text
main:
    la $t0, vetor
    lw $t1, 0($t0)
    lw $t2, 4($t0)
    lw $t3, 8($t0)
    sw $t1, 12($t0)
    lw $t4, 16($sp)
    syscall

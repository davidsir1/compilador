.data
    valor1: .word 100
    valor2: .half 50
    buffer: .space 16

.text
.globl main
main:
    li $t0, 0xFF
    li $t1, -5
    and $t2, $t0, $t1
    or $t3, $t0, $t1
    xor $t4, $t0, $t1
    sll $t5, $t0, 2
    srl $t6, $t1, 1
    slt $t7, $t0, $t1
    syscall

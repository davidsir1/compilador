.text
main:
li $t0, 10
la $a0, mensagem
move $t1, $t0
add $t2, $t0, $t1
addi $t3, $t2, 5
sub $t4, $t3, $t1
addu $t5, $t4, $t0
subu $t6, $t5, $t0
mul $t7, $t6, $t1
div $t7, $t1
mflo $s0
mfhi $s1
and $s2, $s0, $s1
or $s3, $s2, $s1
xor $s4, $s3, $s0
nor $s5, $s4, $s1
sll $s6, $s5, 2
srl $s7, $s6, 1
slt $t0, $s6, $s7
lw $t1, mensagem
sw $t1, 4($sp)
lb $t2, 0($sp)
sb $t2, 1($sp)
lh $t3, 2($sp)
sh $t3, 6($sp)
beq $t0, $t1, fim
bne $t0, $t1, fim
blt $t0, $t1, fim
ble $t0, $t1, fim
bgt $t0, $t1, fim
bge $t0, $t1, fim
j fim
jal fim
jr $ra
syscall
nop
fim:

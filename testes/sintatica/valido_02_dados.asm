.data
.word 10, 20, 30
.half -2, 0x10
.byte 1, 2, 3
.space 16
.align 2
.ascii "texto # preservado"
.asciiz "fim"
.globl inicio
texto: .asciiz "com comentário" # comentário removido
valor: .word 0x2A
.text
inicio:
li $t0, 0x10
lw $t1, valor
sw $t0, 8($sp)
.data
extra: .word 99
.text
fim:
nop
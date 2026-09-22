# Programa com comentarios
.data
    msg: .asciiz "Valor #1" # comentario real
    valor: .word 10

.text
main:                     # inicio do programa
    li $t0, 5             # carrega 5
    # linha so com comentario
    li $t1, 10            # carrega 10
    add $t2, $t0, $t1     # soma
    syscall

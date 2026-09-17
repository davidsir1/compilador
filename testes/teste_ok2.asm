# Teste com strings e numeros
.data
    texto: .asciiz "Resultado #1: "
    valor: .word 0xFF
    neg:   .word -10

.text
main:
    li $t0, 0x10        # hexadecimal
    li $t1, -5          # negativo
    add $t2, $t0, $t1   # soma
    syscall

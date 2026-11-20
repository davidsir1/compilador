.data
    msg: .asciiz "Mensagem nao encerrada
    outro: .asciiz "escape \x invalido"

.text
main:
    li $v0, 10
    syscall

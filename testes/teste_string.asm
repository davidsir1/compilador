.data
    msg1: .asciiz "Ola mundo"
    msg2: .ascii  "Sem terminador"
    msg3: .asciiz  "Com escape \n e \t"
    msg4: .asciiz "Aspas: \" e barra: \\"
    msg5: .asciiz "Caractere nulo \0 no meio"
    msg6: .asciiz ""

.text
main:
    li $v0, 4
    syscall

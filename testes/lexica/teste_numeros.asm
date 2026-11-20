.data
    dec1: .word 0
    dec2: .word 15
    dec3: .word 2025
    neg1: .word -1
    neg2: .word -20
    hex1: .word 0x10
    hex2: .word 0xFF
    hex3: .word 0x10010000
    nhx:  .word -0x10

.text
main:
    li $t0, 10
    li $t1, -5
    li $t2, 0xFF
    li $t3, -0x20
    syscall

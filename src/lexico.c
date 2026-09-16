#include "lexico.h"
#include <stdio.h>

// Tabelas auxiliares

static const char* DIRETIVAS[] = {
    ".data", ".text", ".word", ".half", ".byte",
    ".space", ".ascii", ".asciz", ".globl", ".align"
};

static const char* INSTRUCOES[] = {
    // Transferência de dados
    "li", "la", "move", "lw", "sw", "lb", "sb", "lh", "sh",
    // Operações aritméticas
    "add", "addi", "addu", "sub", "subu", "mul", "div", "mflo", "mfhi",
    // Operações lógicas, deslocamento e comparação
    "and", "or", "xor", "nor", "sll", "srl", "slt",
    // Desvio e controle
    "beq", "bne", "blt", "ble", "bgt", "j", "jal", "jr", "syscall", "nop"
};

static const char* REGISTRADORES[] = {
    "$zero",
    "$v0", "$v1",
    "$a0", "$a1", "$a2", "$a3",
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9",
    "$s0", "$s1", "$s2", "$s3", "$s5", "$s6", "$s7",
    "$k0", "$k1",
    "$gp", "$sp", "$fp", "$ra",
    // Formas númericas
    "$0", "$1", "$2", "$3", "$4", "$5", "$6", "$7", "$8", "$9",
    "$10", "$11", "$12", "$13", "$14", "$15", "$16", "$17", "$18", "$19",
    "$20", "$21", "$22", "$23", "$24", "$25", "$26", "$27", "$28", "$29",
    "$30", "$31"
};

void AnaliseLexica(FILE *in, FILE *out) {
    TabelaSimbolos ts;
    // Inicializar tabela

    FILE* out_ts = fopen("saida.ts", "w");
    FILE* out_err = fopen("saida.err", "w");

    int linha = 1, coluna = 1;
    int c;

    while ((c = fgetc(in)) != '\0') {

    }

    fclose(out_ts);
    fclose(out_err);
}

#include "lexico.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static const char *DIRETIVAS[] = {
    ".data", ".text", ".word", ".half", ".byte", ".space", ".ascii",
    ".asciiz", ".globl", ".align"
};

static const char *INSTRUCOES[] = {
    "li", "la", "move", "lw", "sw", "lb", "sb", "lh", "sh",
    "add", "addi", "addu", "sub", "subu", "mul", "div", "mflo", "mfhi",
    "and", "or", "xor", "nor", "sll", "srl", "slt",
    "beq", "bne", "blt", "ble", "bgt", "bge", "j", "jal", "jr",
    "syscall", "nop"
};

static const char *REGISTRADORES[] = {
    "$zero", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9",
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
    "$k0", "$k1", "$gp", "$sp", "$fp", "$ra",
    "$0", "$1", "$2", "$3", "$4", "$5", "$6", "$7", "$8", "$9",
    "$10", "$11", "$12", "$13", "$14", "$15", "$16", "$17", "$18", "$19",
    "$20", "$21", "$22", "$23", "$24", "$25", "$26", "$27", "$28", "$29",
    "$30", "$31"
};

/* Cria um Token preenchido com sua categoria, lexema e posição inicial. */
static Token criar_token(const char *nome, const char *lexema, int linha, int coluna) {
    Token tk = {{0}, {0}, linha, coluna};
    snprintf(tk.nome, sizeof(tk.nome), "%s", nome);
    snprintf(tk.lexema, sizeof(tk.lexema), "%s", lexema);
    return tk;
}

/* Acrescenta um caractere ao lexema sem ultrapassar o tamanho máximo do Token. */
static void anexar_char(char *lexema, size_t *tamanho, int ch) {
    if (*tamanho < 99) {
        lexema[(*tamanho)++] = (char)ch;
        lexema[*tamanho] = '\0';
    }
}

/* Inicializa a TS com todas as diretivas, instruções e registradores reservados. */
void inicializar_tabela(TabelaSimbolos *ts) {
    size_t i;
    if (ts == NULL) return;
    ts->total = 0;
    for (i = 0; i < sizeof(DIRETIVAS) / sizeof(DIRETIVAS[0]); i++)
        inserir_simbolo(ts, DIRETIVAS[i], "diretiva", 0, 0);
    for (i = 0; i < sizeof(INSTRUCOES) / sizeof(INSTRUCOES[0]); i++)
        inserir_simbolo(ts, INSTRUCOES[i], "instrucao", 0, 0);
    for (i = 0; i < sizeof(REGISTRADORES) / sizeof(REGISTRADORES[0]); i++)
        inserir_simbolo(ts, REGISTRADORES[i], "registrador", 0, 0);
}

/* Procura um lexema na TS e retorna seu índice; retorna -1 quando não o encontra. */
int buscar_simbolo(TabelaSimbolos *ts, const char *lexema) {
    int i;
    if (ts == NULL || lexema == NULL) return -1;
    for (i = 0; i < ts->total; i++)
        if (strcmp(ts->entradas[i].lexema, lexema) == 0) return i;
    return -1;
}

/* Insere um símbolo único na TS ou atualiza a primeira ocorrência de um reservado. */
int inserir_simbolo(TabelaSimbolos *ts, const char *lexema, const char *categoria,
                    int linha, int coluna) {
    int indice;
    Simbolo *novo;
    if (ts == NULL || lexema == NULL || categoria == NULL) return -1;
    indice = buscar_simbolo(ts, lexema);
    if (indice >= 0) {
        if (ts->entradas[indice].primeiraLinha == 0 && linha > 0) {
            ts->entradas[indice].primeiraLinha = linha;
            ts->entradas[indice].primeiraColuna = coluna;
        }
        return 0;
    }
    if (ts->total >= MAX_SIMBOLOS) return -1;
    novo = &ts->entradas[ts->total++];
    snprintf(novo->lexema, sizeof(novo->lexema), "%s", lexema);
    snprintf(novo->categoria, sizeof(novo->categoria), "%s", categoria);
    novo->primeiraLinha = linha;
    novo->primeiraColuna = coluna;
    return 1;
}

/* Escreve a TS no formato CSV exigido para o arquivo .ts. */
void imprimir_tabela(TabelaSimbolos *ts, FILE *saida) {
    int i;
    if (ts == NULL || saida == NULL) return;
    fprintf(saida, "LEXEMA,CATEGORIA,LINHA,COLUNA\n");
    for (i = 0; i < ts->total; i++)
        fprintf(saida, "%s,%s,%d,%d\n", ts->entradas[i].lexema,
                ts->entradas[i].categoria, ts->entradas[i].primeiraLinha,
                ts->entradas[i].primeiraColuna);
}

/* Copia uma cadeia em minúsculas para consultas sem diferenciar maiúsculas/minúsculas. */
void converter_minusculas(const char *origem, char *destino) {
    size_t i;
    for (i = 0; origem[i] != '\0'; i++) destino[i] = (char)tolower((unsigned char)origem[i]);
    destino[i] = '\0';
}

/* Copia uma cadeia em maiúsculas para montar nomes de tokens, como INS_ADD. */
void converter_maiusculas(const char *origem, char *destino) {
    size_t i;
    for (i = 0; origem[i] != '\0'; i++) destino[i] = (char)toupper((unsigned char)origem[i]);
    destino[i] = '\0';
}

/*
 * Consome uma diretiva iniciada por '.', preserva o lexema original e consulta
 * sua versão normalizada na TS. Retorna DIR_<NOME> ou ERRO_DIRETIVA_INVALIDA.
 */
Token reconhecer_diretiva(FILE *in, int linha, int coluna, int primeiro_char, TabelaSimbolos *ts) {
    char lexema[100] = "", normalizado[100], nome[50];
    size_t tamanho = 0;
    int ch, indice;
    anexar_char(lexema, &tamanho, primeiro_char);
    while ((ch = fgetc(in)) != EOF) {
        if (!isalnum((unsigned char)ch) && ch != '_') { ungetc(ch, in); break; }
        anexar_char(lexema, &tamanho, ch);
    }
    converter_minusculas(lexema, normalizado);
    indice = buscar_simbolo(ts, normalizado);
    if (indice < 0 || strcmp(ts->entradas[indice].categoria, "diretiva") != 0)
        return criar_token("ERRO_DIRETIVA_INVALIDA", lexema, linha, coluna);
    snprintf(nome, sizeof(nome), "DIR_");
    converter_maiusculas(normalizado + 1, nome + 4);
    inserir_simbolo(ts, normalizado, "diretiva", linha, coluna);
    return criar_token(nome, lexema, linha, coluna);
}

/*
 * Consome um lexema iniciado por '$', valida-o contra os registradores da TS
 * e retorna REG ou ERRO_REGISTRADOR_INVALIDO sem alterar o texto original.
 */
Token reconhecer_registrador(FILE *in, int linha, int coluna, int primeiro_char, TabelaSimbolos *ts) {
    char lexema[100] = "", normalizado[100];
    size_t tamanho = 0;
    int ch, indice;
    anexar_char(lexema, &tamanho, primeiro_char);
    while ((ch = fgetc(in)) != EOF) {
        if (!isalnum((unsigned char)ch)) { ungetc(ch, in); break; }
        anexar_char(lexema, &tamanho, ch);
    }
    converter_minusculas(lexema, normalizado);
    indice = buscar_simbolo(ts, normalizado);
    if (indice < 0 || strcmp(ts->entradas[indice].categoria, "registrador") != 0)
        return criar_token("ERRO_REGISTRADOR_INVALIDO", lexema, linha, coluna);
    inserir_simbolo(ts, normalizado, "registrador", linha, coluna);
    return criar_token("REG", lexema, linha, coluna);
}

/*
 * Consome letras, dígitos e '_'. Instruções reservadas retornam INS_<NOME>;
 * os demais lexemas retornam ID e entram na TS apenas quando seguidos de ':'.
 */
Token reconhecer_identificador_ou_instrucao(FILE *in, int linha, int coluna,
                                             int primeiro_char, TabelaSimbolos *ts) {
    char lexema[100] = "", normalizado[100], nome[50];
    size_t tamanho = 0;
    int ch, indice;
    anexar_char(lexema, &tamanho, primeiro_char);
    while ((ch = fgetc(in)) != EOF) {
        if (!isalnum((unsigned char)ch) && ch != '_') { ungetc(ch, in); break; }
        anexar_char(lexema, &tamanho, ch);
    }
    converter_minusculas(lexema, normalizado);
    indice = buscar_simbolo(ts, normalizado);
    if (indice >= 0 && strcmp(ts->entradas[indice].categoria, "instrucao") == 0) {
        snprintf(nome, sizeof(nome), "INS_");
        converter_maiusculas(normalizado, nome + 4);
        inserir_simbolo(ts, normalizado, "instrucao", linha, coluna);
        return criar_token(nome, lexema, linha, coluna);
    }
    ch = fgetc(in);
    if (ch != EOF) {
        if (ch == ':') inserir_simbolo(ts, lexema, "identificador/rótulo", linha, coluna);
        ungetc(ch, in);
    }
    return criar_token("ID", lexema, linha, coluna);
}

/* Ponto de integração do AFD completo; os demais estados serão chamados daqui. */
void AnaliseLexica(FILE *in, FILE *out) {
    (void)in;
    (void)out;
    /* O loop principal do AFD será integrado pelo responsável pelos demais tokens. */
}

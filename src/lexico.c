#include "lexico.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

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
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
    "$k0", "$k1",
    "$gp", "$sp", "$fp", "$ra",
    // Formas númericas
    "$0", "$1", "$2", "$3", "$4", "$5", "$6", "$7", "$8", "$9",
    "$10", "$11", "$12", "$13", "$14", "$15", "$16", "$17", "$18", "$19",
    "$20", "$21", "$22", "$23", "$24", "$25", "$26", "$27", "$28", "$29",
    "$30", "$31"
};

// Funções

void inicializar_tabela(TabelaSimbolos* ts) {
    if (ts == NULL) return;

    ts->total = 0;

    // Diretivas
    for (int i = 0; i < sizeof(DIRETIVAS)/sizeof(DIRETIVAS[0]); i++) {
        inserir_simbolo(ts, DIRETIVAS[i], "diretiva", 0, 0);
    }
    // Instruções
    for (int i = 0; i < sizeof(INSTRUCOES)/sizeof(INSTRUCOES[0]); i++) {
        inserir_simbolo(ts, INSTRUCOES[i], "instrucao", 0, 0);
    }
    // Registradores
    for (int i = 0; i < sizeof(REGISTRADORES)/sizeof(REGISTRADORES[0]); i++) {
        inserir_simbolo(ts, REGISTRADORES[i], "registrador", 0, 0);
    }
}

int buscar_simbolo(TabelaSimbolos *ts, const char *lexema) {
    if (ts == NULL || lexema == NULL) return -1;

    for (int i = 0; i < ts->total; i++) {
        if (strcmp(ts->entradas[i].lexema, lexema) == 0)
            return i;
    }

    return -1; // Caso ele não encontre o simbolo
}

int inserir_simbolo(TabelaSimbolos *ts, const char *lexema, const char *categoria, int linha, int coluna) {
    if (ts == NULL || lexema == NULL || categoria == NULL) return -1;

    int indice = buscar_simbolo(ts, lexema);

    if (indice >= 0) {
        if (ts->entradas[indice].primeiraLinha == 0 &&
        ts->entradas[indice].primeiraColuna == 0 &&
        linha > 0 && coluna > 0) {
            ts->entradas[indice].primeiraLinha = linha;
            ts->entradas[indice].primeiraColuna = coluna;
        }
        return 0; // Já inseriu
    }

    // Verifica se o total de simbolos da tabela ultrapassou o limite definido
    if (ts->total >= MAX_SIMBOLOS) return -1;

    Simbolo* nova = &ts->entradas[ts->total];
    strncpy(nova->lexema, lexema, 100);
    nova->lexema[99] = '\0';
    strncpy(nova->categoria, categoria, 30);
    nova->categoria[29] = '\0';
    nova->primeiraLinha = linha;
    nova->primeiraColuna = coluna;

    ts->total++;
    return 1;
}

void imprimir_tabela(TabelaSimbolos *ts, FILE *saida) {
    if (ts == NULL || saida == NULL) return;

    fprintf(saida, "LEXEMA,CATEGORIA,LINHA,COLUNA\n");

    for (int i = 0; i < ts->total; i++) {
        Simbolo* s = &ts->entradas[i];
        fprintf(saida, "%s,%s,%d,%d\n", s->lexema, s->categoria, s->primeiraLinha, s->primeiraColuna);
    }
}

void converter_minusculas(const char *origem, char *destino) {
    int i;
    for (i = 0; origem[i] != '\0'; i++) {
        destino[i] = (char)tolower(origem[i]);
    }
    destino[i] = '\0';
}

void converter_maiusculas(const char *origem, char *destino) {
    int i;
    for (i = 0; origem[i] != '\0'; i++) {
        destino[i] = (char)toupper(origem[i]);
    }
    destino[i] = '\0';
}

static Token montar_token(const char* nome, const char* lexema, int linha, int coluna) {
    Token tk;
    strncpy(tk.nome, nome, 49);
    tk.nome[49] = '\0';
    strncpy(tk.lexema, lexema, 99);
    tk.lexema[99] = '\0';
    tk.linha = linha;
    tk.coluna = coluna;
    return tk;
}

Token reconhecer_diretiva(FILE *in, int linha, int coluna, int primeiro_char, TabelaSimbolos *ts) {
    Token tk;
    return tk;
}

Token reconhecer_registrador(FILE *in, int linha, int coluna, int primeiro_char, TabelaSimbolos *ts) {
    Token tk;
    return tk;
}

Token reconhecer_identificador_ou_instrucao(FILE *in, int linha, int coluna, int primeiro_char, TabelaSimbolos *ts) {
    Token tk;
    return tk;
}

Token reconhecer_numero(FILE *in, int linha, int coluna, int primeiro_char) {
    Token tk;
    return tk;
}

Token reconhecer_negativo(FILE *in, int linha, int coluna, int primeiro_char) {
    Token tk;
    return tk;
}

Token reconhecer_string(FILE *in, int linha, int coluna, int primeiro_char) {
    Token tk;
    return tk;
}

Token reconhecer_simbolo(int caracter, int linha, int coluna) {
    Token tk;
    return tk;
}

/*
 * Função principal para Analise Lexica
 */
void AnaliseLexica(FILE *in, FILE *out) {
    if (in == NULL || out == NULL) return;

    // Arquivos de saida da tabela de simbolos e erros
    FILE* saida_ts = fopen("saida.ts", "w");
    FILE* saida_err = fopen("saida.err", "w");
    if (saida_ts == NULL || saida_err == NULL) {
        printf("Erro: falha ao escrever os arquivos saida.ts ou saida.err.\n");
        if (saida_ts) fclose(saida_ts);
        if (saida_err) fclose(saida_err);
        return;
    }

    TabelaSimbolos ts;
    // Inicializar tabela
    inicializar_tabela(&ts);

    int linha = 1, coluna = 1, houve_erro = 0;
    int caracter;

    while ((caracter = fgetc(in)) != EOF) {
        // Ignorar espaços, tabulações
        if (caracter == ' ' || caracter == '\t') {coluna++; continue;}

        // Nova linha quando encontra o fim da linha do arquivo
        if (caracter == '\n') {linha++; coluna = 1; continue;}

        // Comentário
        if (caracter == '#') {
            // Essa parte tem que ler o que está após '#' e quando chegar no final '\n'
            // incrementar uma linha e atribuir coluna = 1

            continue;
        }

        Token tk;
        if (isalnum(caracter) || caracter == '_') { // Estado q0 -> q1

        } else if (caracter == '.') { // Estado q0 -> q2

        } else if (caracter == '$') { // Estado q0 -> q3

        } else if (isdigit(caracter)) { // Estado q0 -> q5

        } else if (caracter == '-') { // Estado q0 -> q4

        } else if (caracter == '"') { // Estado q0 -> q9

        } else if (caracter == ',' || caracter == ':' || caracter == '(' || caracter == ')') {
            // Estados q0 --','--> q13   q0 --':'--> q14   q0 --'('--> q15  q0 --')'--> q16

        } else { // q0 -> q12

        }

        // Escrever na saída .lex
        /*
         *
         */


        // Verificar se tiver erro, registrar no .err
        /*
         * será escrito no saida_err com a seguinte estrutura: <tipo_erro, lexema> linha coluna
         * e dize que houve erro: houve_erro = 1
         */
        if (strncmp(tk.nome, "ERRO_", 5) == 0) {

        }

        coluna += (int)strlen(tk.lexema);
    }

    // Escrever o EOF no fim da 'FILE* in' .lex

    imprimir_tabela(&ts, saida_ts);

    if (!houve_erro) {
        fprintf(saida_err, "Nenhum erro léxico encontrado.");
    }

    fclose(saida_ts);
    fclose(saida_err);
}

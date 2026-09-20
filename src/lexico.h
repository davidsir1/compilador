#ifndef LEXICO_H
#define LEXICO_H

#include <stdio.h>

#define MAX_SIMBOLOS 600

typedef struct {
    char nome[50];
    char lexema[100];
    int linha;
    int coluna;
} Token;

typedef struct {
    char lexema[100];
    char categoria[30];
    int primeiraLinha;
    int primeiraColuna;
} Simbolo;

typedef struct {
    Simbolo entradas[MAX_SIMBOLOS];
    int total;
} TabelaSimbolos;

void AnaliseLexica(FILE *in, FILE *out);
void inicializar_tabela(TabelaSimbolos *ts);
int buscar_simbolo(TabelaSimbolos *ts, const char *lexema);
int inserir_simbolo(TabelaSimbolos *ts, const char *lexema,
                    const char *categoria, int linha, int coluna);
void imprimir_tabela(TabelaSimbolos *ts, FILE *saida);
Token reconhecer_identificador_ou_instrucao(FILE *in, int linha, int coluna,
                                             int primeiro_char, TabelaSimbolos *ts);
Token reconhecer_diretiva(FILE *in, int linha, int coluna, int primeiro_char,
                           TabelaSimbolos *ts);
Token reconhecer_registrador(FILE *in, int linha, int coluna, int primeiro_char,
                             TabelaSimbolos *ts);
void converter_minusculas(const char *origem, char *destino);
void converter_maiusculas(const char *origem, char *destino);

#endif

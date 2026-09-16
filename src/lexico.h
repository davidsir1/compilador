#ifndef LEXICO_H
#define LEXICO_H

#include <stdio.h>

#define MAX_SIMBOLOS 600

typedef struct {
    char nome[50]; // nome da categoria do token
    char lexema[100]; // sequência de caracteres encontrada no programa
    int linha; // linha em que o lexema começa
    int coluna; // coluna em que o lexema começa
} Token;

typedef struct {
    char lexema[100];
    char categoria[30]; // diretiva, instrução, registrador, identificador/rótulo
    int primeiraLinha;
    int primeiraColuna;
} Simbolo;

typedef struct {
    Simbolo entradas[MAX_SIMBOLOS];
    int total;
} TabelaSimbolos;

void AnaliseLexica(FILE* in, FILE* out);

// Funções auxiliares
void inicializar_simbolo(TabelaSimbolos* ts, const char *lexema);

void buscar_simbolo(TabelaSimbolos* ts, const char* lexema);

int inserir_simbolo(TabelaSimbolos* ts, const char* lexema, const char* categoria, int linha, int coluna);

void imprimir_tabela(TabelaSimbolos* ts, FILE* saida);

#endif // LEXICO_H

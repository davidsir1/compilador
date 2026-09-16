#ifndef LEXICO_H
#define LEXICO_H

#include <stdio.h>

typedef struct {
    char nome[50]; // nome da categoria do token
    char lexema[100]; // sequência de caracteres encontrada no programa
    int linha; // linha em que o lexema começa
    int coluna; // coluna em que o lexema começa
} Token;

typedef struct {
    char lexema[100];
    char categoria[30];
    int primeiraLinha;
    int primeiraColuna;
} Simbolo;


void AnaliseLexia(FILE* in, FILE* out);

#endif // LEXICO_H

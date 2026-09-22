#ifndef PROCESSADOR_H
#define PROCESSADOR_H

#include <stdio.h>

#define VAZIA 1
#define NAO_VAZIA 0

void PreProcessamento(FILE* arquivo_entrada, FILE* arquivo_saida);

void remover_comentarios(char *linha);

void normalizar_linha(char* linha);

int esta_escapado(const char* linha, int pos);

#endif // PROCESSADOR_H

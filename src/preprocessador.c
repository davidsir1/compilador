#include "preprocessador.h"
#include <stdio.h>

void PreProcessamento(FILE *arquivo_entrada, FILE *arquivo_saida) {
    char linha[1024]; // ler a linha da entrada
    char* linha_processada; // processar linha da entrada e escrever na saida

    while (fgets(linha, sizeof(linha), arquivo_entrada)) {
        // Processar a linha
        /*
         * As funções devem estar abaixo dos comentários respectivos;
         * Verificar se tem comentários; Verificar se tem linhas vazias e normalizar
         * espaços e tabulações
         */
        // 1. Remoção dos comentários
        remover_comentarios(linha);

        // 2. Normalização de espaços e tabulações
        normalizar_linha(linha);

        // 3. Verificação de linhas vazias
        int vazia = 1;

        for (int i = 0; linha[i] != '\0'; i++) {
            if (linha[i] != ' ' && linha[i] != '\t' && linha[i] != '\n') {
                vazia = 0;
                break;
            }
        }

        if (vazia) {
            continue;
        }

        // 4. Após executar o pre processamento, escrever no arquivo de saída
        if (linha[0] != '\0') {
            fprintf(arquivo_saida, "%s", linha);
        }

        //printf("%s", linha); // Temporário
    }
}

char* remover_comentarios(char *linha)
{
    int dentro_de_string = 0;
    int barra_invertida = 0;
    int i;

    if (linha == 0) {
        return NULL;
    }

    for (i = 0; linha[i] != '\0'; i++) {
        if (linha[i] == '"' && (i == 0 || linha[i-1] != '\\')) {
            dentro_de_string = !dentro_de_string;
        }

        if (linha[i] == '#' && !dentro_de_string) {
            linha[i] = '\0';
            return linha;
        }
    }

    // Adicionar a quebra de linha no final

    return linha;
}

char* normalizar_linha(char *linha)
{
    int i = 0;
    int j = 0;
    int dentro_string = 0;
    int espaco_pendente = 0;

    while (linha[i] != '\0') {

        /* Dentro de uma string, preserva os caracteres */
        if (dentro_string) {
            linha[j++] = linha[i];

            if (linha[i] == '"' && (i == 0 || linha[i - 1] != '\\')) {
                dentro_string = 0;
            }

            i++;
            continue;
        }

        /* Início de uma string */
        if (linha[i] == '"') {

            /* Coloca o espaço que estava pendente antes da string */
            if (espaco_pendente && j > 0) {
                linha[j++] = ' ';
            }

            linha[j++] = linha[i];
            dentro_string = 1;
            espaco_pendente = 0;
            i++;
            continue;
        }

        /* Tabulação vira espaço */
        if (linha[i] == '\t') {
            espaco_pendente = 1;
            i++;
            continue;
        }

        /* Vários espaços viram apenas um */
        if (linha[i] == ' ') {
            espaco_pendente = 1;
            i++;
            continue;
        }

        /* Ignora quebras de linha */
        if (linha[i] == '\n' || linha[i] == '\r') {
            i++;
            continue;
        }

        /* Adiciona um único espaço entre elementos */
        if (espaco_pendente && j > 0) {
            linha[j++] = ' ';
        }

        linha[j++] = linha[i];
        espaco_pendente = 0;
        i++;
    }

    /* Remove espaço no final */
    if (j > 0 && linha[j - 1] == ' ') {
        j--;
    }

    linha[j++] = '\n';
    linha[j] = '\0';

    return linha;
}

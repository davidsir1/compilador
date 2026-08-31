#include <stdio.h>
#include "preprocessador.h"


int preprocessar(const char *arquivo_entrada, const char *arquivo_saida) {
    FILE* entrada = fopen(arquivo_entrada, "r");
    if (!entrada) {
        printf("Erro: falha em ler o arquivo de entrada %s.", arquivo_entrada);
        return -1;
    }

    FILE* saida = fopen(arquivo_saida, "w");
    if (!saida) {
        printf("Erro: falha em escrever o arquivo de saida %s.", arquivo_saida);
        return -1;
    }

    char linha[1024]; // ler a linha da entrada
    char* linha_processada; // processar linha da entrada e escrever na saida

    while (fgets(linha, sizeof(linha), entrada)) {
        // Processar a linha
        /*
         * As funções devem estar abaixo dos comentários respectivos;
         * Verificar se tem comentários; Verificar se tem linhas vazias e normalizar
         * espaços e tabulações
         */
        // 1. Remoção dos comentários

        // 2. Remoção de linha vazias

        // 3. Normalização de espaços e tabulações
        printf("%s", linha); // Temporário
    }

    fclose(entrada);
    fclose(saida);

    return 0;
}

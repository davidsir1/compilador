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
        remover_comentarios(linha);

        // 2. Remoção de linhas vazias
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

        // 3. Normalização de espaços e tabulações

        // 4. Após executar o pre processamento, escrever no arquivo de saída
        if (linha[0] != '\0') {
            fprintf(saida, "%s\n", linha);
        }

        printf("%s", linha); // Temporário
    }

    fclose(entrada);
    fclose(saida);

    return 0;
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

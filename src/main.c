#include <string.h>
#include <stdio.h>
#include "preprocessador.h"
#include "lexico.h"

void trocar_extensao(const char* origem, const char* nova_extensao, char* destino, int tam);

int main (int argc, char* argv[]) {

    // Formato de entrada: ./main.exe <arquivo_entrada.asm> <arquivo_saida.lex>
    // Para caso de sucesso, o argc deve ser igual à 3.
    if (argc == 1) {
        printf("Alerta: Voce nao passou argumentos.\n");
        printf("Uso: %s <arquivo_entrada.asm> <arquivo_saida.lex>\n", argv[0]);
        return 1;
    } else if (argc == 2) {
        printf("Erro: Voce nao passou o arquivo de saida.\n");
        return 1;
    } else if (argc >= 4) {
        printf("Erro: limite de argumentos ultrapassado.\n");
        return 1;
    }

    // Mudando no nome dos arquivos
    /*
     * Pega o nome do arquivo .asm e coloca a exntesão .pre
     */
    char nome_arquivo_pre[300];
    trocar_extensao(argv[1], ".pre", nome_arquivo_pre, 300);

    // Etapa do pre processamento
    FILE* entrada_asm = fopen(argv[1], "r");
    if (entrada_asm == NULL) {
        printf("Erro: falha em ler o arquivo %s\n.", argv[1]);
        return 1;
    }

    FILE* saida_pre = fopen(nome_arquivo_pre, "w");
    if (saida_pre == NULL) {
        printf("Erro: falha na escrita do arquivo %s.\n", argv[2]);
        fclose(saida_pre);
        return 1;
    }

    PreProcessamento(entrada_asm, saida_pre); // Mudar a saida para o <arquivo_saida.pre>

    fclose(entrada_asm);
    fclose(saida_pre);

    // Etapa da análise léxica
    FILE* entrada_pre = fopen(nome_arquivo_pre, "r");
    if (entrada_pre == NULL) {
        printf("Erro: falha ao ler o arquivo %s.\n", nome_arquivo_pre);
        return 1;
    }

    FILE* saida_lex = fopen(argv[2], "w");
    if (saida_lex == NULL) {
        printf("Erro: falha na escrita do arquivo %s.\n", argv[2]);
        fclose(saida_lex);
        return 1;
    }

    // Extrai o nome do argv[2] e guarda na variável nome_base_saida
    const char* ponto = strrchr(argv[2], '.');
    int base = ponto ? (ponto - argv[2]) : strlen(argv[2]);
    if (base >= sizeof(nome_base_saida)) base = sizeof(nome_base_saida) - 1;
    memcpy(nome_base_saida, argv[2], base);
    nome_base_saida[base] = '\0';

    AnaliseLexica(entrada_pre, saida_lex);

    fclose(entrada_pre);
    fclose(saida_lex);

    return 0;
}

/*
 * Função utilizada para pegar o nome do arquivo_entrada e colocar outro tipo
 * de extensão. No caso, .asm para .pre
 */
void trocar_extensao(const char* origem, const char* nova_extensao, char* destino, int tam) {
    /*
     * Exemplo: origem = "Exemplo01.asm"
     * strrchr -> asm
     */
    const char* ponto = strrchr(origem, '.');
    int base = ponto ? (ponto - origem) : strlen(origem);
    if (base >= tam) base = tam - 1;
    memcpy(destino, origem, base);
    destino[base] = '\0';
    strncat(destino, nova_extensao, tam - base - 1);
}

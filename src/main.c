#include <string.h>
#include <stdio.h>
#include "preprocessador.h"
#include "lexico.h"
#include "sintatico.h"

void trocar_extensao(const char* origem, const char* nova_extensao, char* destino, int tam);

int main (int argc, char* argv[]) {

    // Formato de entrada: ./main.exe <arquivo_entrada.asm> <arquivo_saida>
    // Para caso de sucesso, o argc deve ser igual à 3.
    if (argc == 1) {
        printf("Alerta: Voce nao passou argumentos.\n");
        printf("Uso: %s <arquivo_entrada.asm> <arquivo_saida>\n", argv[0]);
        return 1;
    } else if (argc == 2) {
        printf("Erro: Voce nao passou o arquivo de saida.\n");
        return 1;
    } else if (argc >= 4) {
        printf("Erro: limite de argumentos ultrapassado.\n");
        return 1;
    }

    // Nome de arquivos .pre, .lex, .dot e .sin.err
    char nome_arquivo_pre[300];
    char nome_arquivo_dot[300];
    char nome_arquivo_lex[300];
    char nome_arquivo_err[300];
    char nome_arquivo_lex_err[300];

    //trocar_extensao(argv[1], ".pre", nome_arquivo_pre, 300);
    snprintf(nome_arquivo_pre, sizeof(nome_arquivo_pre), "%s.pre", argv[2]);
    snprintf(nome_arquivo_lex, sizeof(nome_arquivo_lex), "%s.lex", argv[2]);
    snprintf(nome_arquivo_dot, sizeof(nome_arquivo_dot), "%s.dot", argv[2]);
    snprintf(nome_arquivo_err, sizeof(nome_arquivo_err), "%s.sin.err", argv[2]);
    snprintf(nome_arquivo_lex_err, sizeof(nome_arquivo_lex_err), "%s.err", argv[2]);

    /*
     * Etapa de pré processamento
     */
    FILE* entrada_asm = fopen(argv[1], "r");
    if (entrada_asm == NULL) {
        printf("Erro: falha em ler o arquivo %s\n.", argv[1]);
        return 1;
    }

    FILE* saida_pre = fopen(nome_arquivo_pre, "w");
    if (saida_pre == NULL) {
        printf("Erro: falha na escrita do arquivo %s.\n", nome_arquivo_pre);
        return 1;
    }

    PreProcessamento(entrada_asm, saida_pre); // Mudar a saida para o <arquivo_saida.pre>

    fclose(entrada_asm);
    fclose(saida_pre);


    /*
     * Etapa da análise léxica
     */
    FILE* entrada_pre = fopen(nome_arquivo_pre, "r");
    if (entrada_pre == NULL) {
        printf("Erro: falha ao ler o arquivo %s.\n", nome_arquivo_pre);
        return 1;
    }

    FILE* saida_lex = fopen(nome_arquivo_lex, "w");
    if (saida_lex == NULL) {
        printf("Erro: falha na escrita do arquivo %s.\n", nome_arquivo_lex);
        return 1;
    }

    size_t base = strlen(argv[2]);
    if (base >= sizeof(nome_base_saida)) {
        fprintf(stderr, "Erro: o prefixo de saída é muito longo.\n");
        fclose(saida_lex);
        fclose(entrada_pre);
        return 1;
    }
    memcpy(nome_base_saida, argv[2], base);
    nome_base_saida[base] = '\0';

    AnaliseLexica(entrada_pre, saida_lex);

    fclose(saida_lex);
    fclose(entrada_pre);


    /*
     * Etapa da análise léxica
     */
    FILE* saida_dot = fopen(nome_arquivo_dot, "w");
    if (saida_dot == NULL) {
        printf("Erro: falha na escrita do arquivo %s.\n", nome_arquivo_dot);
        return 1;
    }

    FILE* entrada_lex = fopen(nome_arquivo_lex, "r");
    if (entrada_lex == NULL) {
        printf("Erro: falha ao ler o arquivo %s.\n", nome_arquivo_lex);
        return 1;
    }

    FILE* saida_err = fopen(nome_arquivo_err, "w");
    if (saida_err == NULL) {
        printf("Erro: falha na escrita do arquivo %s.\n", nome_arquivo_err);
        fclose(saida_dot);
        fclose(entrada_lex);
        return 1;
    }

    FILE* entrada_err_lex = fopen(nome_arquivo_lex_err, "r");
    if (entrada_err_lex == NULL) {
        printf("Erro: falha ao ler o arquivo %s.\n", nome_arquivo_lex_err);
        fclose(saida_dot);
        fclose(entrada_lex);
        fclose(saida_err);
        return 2;
    }

    int erro_lexico = 0;
    char linha_err[512];
    while (fgets(linha_err, sizeof(linha_err), entrada_err_lex)) {
        if (strncmp(linha_err, "<ERRO_", 6) == 0) {
            erro_lexico = 1;
            break;
        }
    }
    fclose(entrada_err_lex);

    if (erro_lexico) {
        fprintf(saida_err, "Análise sintática não executada: existem erros léxicos.\n");
        fclose(saida_dot);
        fclose(entrada_lex);
        fclose(saida_err);
        return 1;
    }

    int resultado = AnaliseSintatica(entrada_lex, saida_dot, saida_err);

    fclose(saida_dot);
    fclose(entrada_lex);
    fclose(saida_err);

    return resultado < 0 ? 2 : resultado;
}

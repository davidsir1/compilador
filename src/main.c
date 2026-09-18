#include <stdio.h>
#include "preprocessador.h"
#include "lexico.h"

int main (int argc, char* argv[]) {

    // Formato de entrada: ./main.exe <arquivo_entrada.asm> <arquivo_saida.lex>
    // Para caso de sucesso, o argc deve ser igual à 3.
    if (argc == 1) {
        printf("Alerta: Voce nao passou argumentos.\n");
        return 1;
    } else if (argc == 2) {
        printf("Erro: Voce nao passou o arquivo de saida.\n");
        return 1;
    } else if (argc >= 4) {
        printf("Erro: limite de argumentos ultrapassado.\n");
        return 1;
    }

    FILE* entrada = fopen(argv[1], "r");
    FILE* saida = fopen(argv[2], "w");

    PreProcessamento(entrada, saida); // Mudar a saida para o <arquivo_saida.pre>

    // Chamar a função de analiselexica
    //AnaliseLexica(FILE *in, FILE *out);

    return 0;
}

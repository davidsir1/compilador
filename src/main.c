#include <stdio.h>

int main (int argc, char* argv[]) {

    if (argc == 1) {
        printf("Alerta: Voce nao passou argumentos.\n");
        return 1;
    } else if (argc >= 4) {
        printf("Erro: limite de argumentos ultrapassado.\n");
        return 1;
    }

    // Formato de entrada: ./main.exe <arquivo_entrada.asm> <arquivo_saida.pre>

    FILE* arquivo_entrada = fopen(argv[1], "r");
    FILE* arquivo_saida = fopen(argv[2], "w");



    fclose(arquivo_entrada);
    fclose(arquivo_saida);

    return 0;
}

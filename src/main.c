#include <stdio.h>
#include "preprocessador.h"

int main (int argc, char* argv[]) {

    if (argc == 1) {
        printf("Alerta: Voce nao passou argumentos.\n");
        return 1;
    } else if (argc >= 4) {
        printf("Erro: limite de argumentos ultrapassado.\n");
        return 1;
    }

    // Formato de entrada: ./main.exe <arquivo_entrada.asm> <arquivo_saida.pre>

    int res = preprocessar(argv[1], argv[2]);

    return 0;
}

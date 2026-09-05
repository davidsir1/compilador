#ifndef PROCESSADOR_H
#define PROCESSADOR_H

#define VAZIA 1
#define NAO_VAZIA 0

int preprocessar(const char* arquivo_entrada, const char* arquivo_saida);

char* remover_comentarios(char *linha);

char* normalizar_linha(char* linha);

#endif // PROCESSADOR_H

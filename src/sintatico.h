#ifndef SINTATICO_H
#define SINTATICO_H

#include <stdio.h>
#include "lexico.h"

typedef struct {
    Token* tokens; // vetor dinâmico de tokens
    int total; // quantos tokens válidos
    int capacidade; // capacidade alocada
    int atual; // indice do token corrent
    FILE* arvore; // arquivo .dot (NULL se for inválido)
    FILE* erros; // arquivo .sin.err
    int houve_erro; // 1 se houver erro sintático
} AnaliseEstado;

typedef struct NoArvore {
    int id; // identificado único do nó
    char rotulo[256]; // rótulo exibido no gráfico
    struct NoArvore* primeiro_filho;
    struct NoArvore* proximo_irmao;
} NoArvore;

// Função principal
int AnaliseSintatica(FILE* tokens, FILE* arvore, FILE* erros);

// Funções auxiliares
void liberar_arvore(NoArvore* raiz);

int carregar_tokens(FILE* in, AnaliseEstado* a);

int inserir_eol_e_eof(AnaliseEstado* a);

Token* token_atual(AnaliseEstado* a);

Token* token_espiar(AnaliseEstado* a, int deslocamento);

void avancar(AnaliseEstado* a);

int verificar(AnaliseEstado* a, const char* nome);

int erro(AnaliseEstado* a, const char* esperado);

int consumir(AnaliseEstado* a, const char *nome);

void trim(char* str);

void sicronizar_erro(AnaliseEstado* a);

// Funções da árvore sintática
NoArvore* novo_no(const char* rotulo);

void adicionar_filho(NoArvore* pai, NoArvore* filho);

NoArvore* no_terminal(Token* t);

void emitir_dot(FILE* out, NoArvore* raiz);

void liberar_tokens(AnaliseEstado* a);

NoArvore* novo_nao_terminal(const char* rotulo);

NoArvore* consumir_no(AnaliseEstado* a);

// Funções de anaĺise sintática não terminal
NoArvore* analise_programa(AnaliseEstado* a);
NoArvore* analise_secao(AnaliseEstado* a);
NoArvore* analise_secao_dados(AnaliseEstado* a);
NoArvore* analise_secao_codigo(AnaliseEstado* a);
NoArvore* analise_linha_dados(AnaliseEstado* a);
NoArvore* analise_linha_codigo(AnaliseEstado* a);
NoArvore* analise_rotulo(AnaliseEstado* a);
NoArvore* analise_diretiva_global(AnaliseEstado* a);
NoArvore* analise_diretiva_dados(AnaliseEstado* a);
NoArvore* analise_lista_inteiros(AnaliseEstado* a);
NoArvore* analise_instrucao(AnaliseEstado* a);
NoArvore* analise_diretiva_inteiros(AnaliseEstado* a);
NoArvore* analise_diretiva_string(AnaliseEstado* a);
NoArvore* analise_op_tres_registradores(AnaliseEstado* a);
NoArvore* analise_instrucao_tres_registradores(AnaliseEstado* a);
NoArvore* analise_op_dois_registradores(AnaliseEstado* a);
NoArvore* analise_instrucao_dois_registradores(AnaliseEstado* a);
NoArvore* analise_op_um_registrador(AnaliseEstado* a);
NoArvore* analise_instrucao_um_registrador(AnaliseEstado* a);
NoArvore* analise_instrucao_imediato(AnaliseEstado* a);
NoArvore* analise_op_deslocamento(AnaliseEstado* a);
NoArvore* analise_instrucao_deslocamento(AnaliseEstado* a);
NoArvore* analise_instrucao_li(AnaliseEstado* a);
NoArvore* analise_instrucao_la(AnaliseEstado* a);
NoArvore* analise_op_memoria(AnaliseEstado* a);
NoArvore* analise_endereco(AnaliseEstado* a);
NoArvore* analise_instrucao_memoria(AnaliseEstado* a);
NoArvore* analise_op_desvio(AnaliseEstado* a);
NoArvore* analise_instrucao_desvio(AnaliseEstado* a);
NoArvore* analise_op_salto(AnaliseEstado* a);
NoArvore* analise_instrucao_salto(AnaliseEstado* a);
NoArvore* analise_instrucao_sem_operando(AnaliseEstado* a);

#endif // SINTATICO_H

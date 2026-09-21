#ifndef LEXICO_H
#define LEXICO_H

#include <stdio.h>

#define MAX_SIMBOLOS 600

/*
 * Variável utilizada para derivar as saídas .ts e .err dentro da AnaliseLexica()
 * de acordo com as instruções do PDF, a função não pode ter diferentes parametros
 */
extern char nome_base_saida[255];

typedef struct {
    char nome[50]; // nome da categoria do token
    char lexema[100]; // sequência de caracteres encontrada no programa
    int linha; // linha em que o lexema começa
    int coluna; // coluna em que o lexema começa
} Token;

typedef struct {
    char lexema[100];
    char categoria[30]; // diretiva, instrução, registrador, identificador/rótulo
    int primeiraLinha;
    int primeiraColuna;
} Simbolo;

typedef struct {
    Simbolo entradas[MAX_SIMBOLOS];
    int total;
} TabelaSimbolos;

void AnaliseLexica(FILE* in, FILE* out);

// Funções auxiliares
/*
 * Escrever sobre as funções inicializar_tabela, buscar_simbolo, inserir_simbolo, imprimir_tabela
 */
void inicializar_tabela(TabelaSimbolos* ts);

int buscar_simbolo(TabelaSimbolos* ts, const char* lexema);

int inserir_simbolo(TabelaSimbolos* ts, const char* lexema, const char* categoria, int linha, int coluna);

void imprimir_tabela(TabelaSimbolos* ts, FILE* saida);

/*
 * Função auxiliar para continuar a leitura de um número.
 * lexema contém o prefixo acumulado
 * i é o tamanho atual do lexema (índice onde o próximo caracter é escrito)
 */
Token continuar_numero(FILE* in, int linha, int coluna, char* lexema, int i);

/*
 * Função que reconhece um identificador ou instrução (estado q1)
 * Recebe primeiro caractere (letra ou '_') já lido e devolve um Token preenchido
 * Se for rótulo (seguido de ':'), insere automaticamente na Tabela de Simbolos. Caso ocorra um erro,
 * ele retorna ERRO_INDETIFICADOR_INVALIDO ou ERRO_INSTRUCAO_INVALIDA
 */
Token reconhecer_identificador_ou_instrucao(FILE* in, int linha, int coluna, int primeiro_char, TabelaSimbolos* ts);

/*
 * Reconhece uma diretiva (estado q2)
 * Recebe o '.' já lido. Devolve Token DIR_<NOME> ou ERRO_DIRETIVA_INVALIDA
 */
Token reconhecer_diretiva(FILE* in, int linha, int coluna, int primeiro_char, TabelaSimbolos* ts);

/*
 * Reconhece um registrador (estado q3)
 * Recebe '$' já lido. Devolte Token REG ou ERRO_REGISTRADOR_INVALIDO
 */
Token reconhecer_registrador(FILE* in, int linha, int coluna, int primeiro_char, TabelaSimbolos* ts);

/*
 * Reconhece números decimais, hexadecimal e também negativos com a função reconhecer_negativo()
 * Recebe o primeiro digito já lido e devolve Token NUM_INT ou ERRO_NUMERO_INFORMADO.
 */
Token reconhecer_numero(FILE* in, int linha, int coluna, int primeiro_char);

/*
 * Reconhece número negativo (). Recebe o '-' já lido e devolve Token NUM_IN ou ERRO_CARACTERE_INVALIDO
 */
Token reconhecer_negativo(FILE* in, int linha, int coluna, int primeiro_char);

/*
 * Reconhece uma string delimitada por asapas ().
 * Recebe a '"' inicial e devolve Token STRING, ERRO_STRING_NAO_FECHADA ou ERRO_ESCAPE_INVALIDO
 */
Token reconhecer_string(FILE* in, int linha, int coluna, int primeiro_char);

/*
 * Monta um Token para um símbolo de um único caractere.
 * (',' ':' '(' ')')
 */
Token reconhecer_simbolo(int caracter, int linha, int coluna);

/* Converter conteúdo da string para minusculo */
void converter_minusculas(const char* origem, char* destino);

/* Converter conteúdo da string para maiusculo */
void converter_maiusculas(const char* origem, char* destino);

#endif // LEXICO_H

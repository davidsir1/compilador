#include "sintatico.h"
#include "lexico.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void trim(char* str) {
    char* p = str;
    while (*p == ' ' || *p == '\t') p++;

    if (p != str) memmove(str, p, strlen(p) + 1);

    int len = strlen(str);
    while (len > 0 && (str[len-1]) == ' ' || str[len-1] == '\t' || str[len-1] == '\n' || str[len-1] == '\r') {
        str[len-1] = '\0';
        len--;
    }
}

int carregar_tokens(FILE* in, AnaliseEstado* a) {
    char linha[512];
    a->capacidade = 256;
    a->total = 0;
    a->tokens = malloc(sizeof(Token) * a->capacidade);

    if (!a->tokens) return -1;

    while (fgets(linha, sizeof(linha), in)) {
        Token tk;

        // Estado: <NOME, LEXEMA> linha coluna
        char* abre = strchr(linha, '<');
        char* virgula = abre ? strchr(abre, ',') : NULL;
        char* fecha = virgula ? strchr(virgula, '>') : NULL;

        if (!abre || !virgula || !fecha) continue;

        int n_nome = (int)(virgula - abre - 1);
        int n_lex = (int)(fecha - virgula - 1);
        if (n_nome <= 0 || n_nome >= 50) continue;
        if (n_lex <= 0 || n_lex >= 50) continue;

        memcpy(tk.nome, abre + 1, n_nome); tk.nome[n_nome] = '\0';
        tk.nome[n_nome] = '\0';
        trim(tk.nome);

        memcpy(tk.lexema, virgula + 1, n_lex); tk.lexema[n_lex] = '\0';
        tk.lexema[n_lex] = '\0';
        trim(tk.lexema);

        // Linha e coluna após '>'
        if (sscanf(fecha + 1, "%d %d", &tk.linha, &tk.coluna) != 2) continue;

        // Cresce o vetor se necessário
        if (a->total >= a->capacidade) {
            a->capacidade *= 2;
            Token* novo = realloc(a->tokens, sizeof(Token) * a->capacidade);
            if (!novo) return -1;
            a->tokens = novo;
        }

        a->tokens[a->total++] = tk;
    }

    return 0;
}

int inserir_eol_e_eof(AnaliseEstado* a) {
    /* Implementação
     * 1. Percorrer a->tokens, comparando tk.linha com a linha anterior
     * 2. Quando mudar, inserir um Token "EOL" com a linha anterior
     *    e coluna = ultimo + strlen(lexema)
     * 3. Ao final, inserir EOL e depois TK_EOF se não existirem.
     * Usar função auxiliar 'inserir_token_na_posicao'
     */
    if (a->total == 0) return 0;

    int nova_cap = a->total * 2 + 10; // nova_cap = nova_capacidade
    Token* novo_vetor = malloc(sizeof(Token) * nova_cap);
    if (!novo_vetor) return -1;

    int novo_total = 0;
    int linha_anterior = -1;

    for (int i = 0; i < a->total; i++) {
        Token tk = a->tokens[i];

        if (linha_anterior != -1 && tk.linha != linha_anterior) {
            Token eol;
            strcpy(eol.nome, "EOL");
            strcpy(eol.lexema, "EOL");
            eol.linha = linha_anterior;
            eol.coluna = a->tokens[i-1].coluna + strlen(a->tokens[i-1].lexema);
            novo_vetor[novo_total++] = eol;
        }

        novo_vetor[novo_total++] = tk;
        linha_anterior = tk.linha;
    }

    if (strcmp(novo_vetor[novo_total-1].nome, "TK_EOF") != 0) {
        if (strcmp(novo_vetor[novo_total-1].nome, "EOL") != 0) {
            Token eol;
            strcpy(eol.nome, "EOL");
            strcpy(eol.lexema, "EOL");
            eol.linha = linha_anterior;
            eol.coluna = a->tokens[a->total-1].coluna + strlen(a->tokens[a->total-1].lexema);

            novo_vetor[novo_total++] = eol;
        }

        Token eof;
        strcpy(eof.nome, "TK_EOF");
        strcpy(eof.lexema, "EOF");
        eof.linha = linha_anterior + 1;
        eof.coluna = 1;
        novo_vetor[novo_total++] = eof;
    }

    free(a->tokens);
    a->tokens = novo_vetor;
    a->total = novo_total;
    a->capacidade = nova_cap;

    return 0;
}

/*
 * Funções auxiliares para AnaliseEstado
 */
Token* token_atual(AnaliseEstado* a) {
    if (a->atual >= a->total) return NULL;
    return &a->tokens[a->atual];
}

Token* token_espiar(AnaliseEstado* a, int deslocamento) {
    int indice = a->atual + deslocamento;
    if (indice < 0 || indice >= a->total) return NULL;
    return &a->tokens[indice];
}

void avancar(AnaliseEstado* a) {
    if (a->atual < a->total) a->atual++;
}

void liberar_tokens(AnaliseEstado *a) {
    if (a->tokens) {
        free(a->tokens);
        a->tokens = NULL;
    }
}

/*
 * Função que compara o nome do token corrente com 'nome'.
 * Devolve 1 se bater, 0 caso contrário
 */
int verificar(AnaliseEstado* a, const char* nome) {
    Token* t = token_atual(a);
    return t && strcmp(t->nome, nome) == 0;
}

/*
 * Função que registra erro sintático e devolve 0
 */
int erro(AnaliseEstado* a, const char* esperado) {
    Token* t = token_atual(a);
    if (!t) return 0;

    if (strcmp(t->nome, "EOL") == 0) {
        fprintf(a->erros, "Linha %d, coluna %d: Encontrado fim de linha (EOL); esperado %s.\n",
            t->linha, t->coluna, esperado);
    } else if (strcmp(t->nome, "TK_EOF") == 0) {
        fprintf(a->erros, "Linha %d, coluna %d: Encontrado TK_EOF; esperado %s.\n",
            t->linha, t->coluna, esperado);
    } else {
        fprintf(a->erros, "Linha %d, coluna %d: Encontrado %s(\"%s\"); esperado %s.\n",
            t->linha, t->coluna, t->nome, t->lexema, esperado);
    }

    a->houve_erro = 1;
    return 0;
}

void sicronizar_erro(AnaliseEstado *a) {
    while (token_atual(a) != NULL) {
        if (verificar(a, "EOL")) {
            avancar(a); // consome EOL
            break;
        }
        if (verificar(a, "TK_EOF")) {
            break;
        }
        avancar(a);
    }
}

/*
 * Consome o token corrente se for igual com 'nome'
 * Devolve 1 em sucesso, 0 em erro
 */
int consumir(AnaliseEstado* a, const char *nome) {
    if (verificar(a, nome)) {
        avancar(a);
        return 1;
    }
    return erro(a, nome);
}

static int g_proximo_id = 0;

NoArvore* novo_no(const char* rotulo) {
    // O uso do calloc no lugar do malloc é que ele inicializa todos os bytes com o valor zero
    NoArvore* n = calloc(1, sizeof(NoArvore));

    if (!n) return NULL;

    n->id = g_proximo_id++;
    strncpy(n->rotulo, rotulo, 255);
    n->rotulo[255] = '\0';
    return n;
}

void adicionar_filho(NoArvore* pai, NoArvore* filho) {
    if (!pai || !filho) return;
    if (!pai->primeiro_filho) {
        pai->primeiro_filho = filho;
    } else {
        NoArvore* atual = pai->primeiro_filho;
        while (atual->proximo_irmao) atual = atual->proximo_irmao;
        atual->proximo_irmao = filho;
    }
}

NoArvore* no_terminal(Token* t) {
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s\\n%s", t->nome, t->lexema);
    return novo_no(buffer);
}

void emitir_dot(FILE* out, NoArvore* no) {
    // Percorrer em profundidade em imprimir nós e arestas
    if (!no) return;
    fprintf(out, "  n%d [label=\"%s\"];\n", no->id, no->rotulo);

    NoArvore* filho = no->primeiro_filho;
    while (filho) {
        fprintf(out, "  n%d -> n%d;\n", no->id, filho->id);
        emitir_dot(out, filho);
        filho = filho->proximo_irmao;
    }
}

void liberar_arvore(NoArvore *raiz) {
    if (!raiz) return;
    liberar_arvore(raiz->primeiro_filho);
    liberar_arvore(raiz->proximo_irmao);
    free(raiz);
}

/*
 * Escrever as funções de análise sintática de não terminais
 */

NoArvore* analise_programa(AnaliseEstado* a) {
    NoArvore* no = novo_no("<programa>");

    return no;
}

/*
 * Função principal
 */
int AnaliseSintatica(FILE *tokens, FILE *arvore, FILE *erros) {
    AnaliseEstado a;
    memset(&a, 0, sizeof(a));
    a.arvore = arvore;
    a.erros = erros;
    a.atual = 0;

    if (carregar_tokens(tokens, &a) != 0) return -1;
    if (inserir_eol_e_eof(&a) != 0) { liberar_tokens(&a); return -1; }

    NoArvore* raiz = analise_programa(&a);

    if (!a.houve_erro && raiz) {
        fprintf(arvore, "diagrapph ArvoreSintatica {\n");
        fprintf(arvore, "   graph [ordering=out];\n");
        fprintf(arvore, "   node [shape=box, fontname=\"Arial\"];\n");
        fprintf(arvore, "   edge [arrowhead=none];\n");
        emitir_dot(arvore, raiz);
        fprintf(arvore, "}\n");
    } else {
        a.houve_erro = 1;
    }

    if (!a.houve_erro) {
        fprintf(erros, "Nenhum erro sintático encontrado.\n");
    }

    liberar_arvore(raiz);
    liberar_tokens(&a);

    return a.houve_erro ? 1 : 0;
}

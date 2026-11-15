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
    while (len > 0 && (str[len-1] == ' ' || str[len-1] == '\t' ||
                       str[len-1] == '\n' || str[len-1] == '\r')) {
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
        char* fecha = virgula ? strrchr(virgula, '>') : NULL;

        if (!abre || !virgula || !fecha) continue;

        int n_nome = (int)(virgula - abre - 1);
        int n_lex = (int)(fecha - virgula - 1);
        if (n_nome <= 0 || n_nome >= (int)sizeof(tk.nome)) continue;
        if (n_lex <= 0 || n_lex >= (int)sizeof(tk.lexema)) continue;

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
    if (a->total == 0) {
        free(a->tokens);
        a->tokens = NULL;
        a->tokens = calloc(1, sizeof(Token));
        if (!a->tokens) return -1;
        a->capacidade = a->total = 1;
        strcpy(a->tokens[0].nome, "TK_EOF");
        strcpy(a->tokens[0].lexema, "EOF");
        a->tokens[0].linha = 1;
        a->tokens[0].coluna = 1;
        return 0;
    }

    int nova_cap = a->total * 2 + 10; // nova_cap = nova_capacidade
    Token* novo_vetor = malloc(sizeof(Token) * nova_cap);
    if (!novo_vetor) return -1;

    int novo_total = 0;
    int linha_anterior = -1;

    for (int i = 0; i < a->total; i++) {
        Token tk = a->tokens[i];

        if (strcmp(tk.nome, "TK_EOF") == 0) {
            if (novo_total > 0 &&
                strcmp(novo_vetor[novo_total - 1].nome, "EOL") != 0) {
                Token eol;
                strcpy(eol.nome, "EOL");
                strcpy(eol.lexema, "EOL");
                eol.linha = novo_vetor[novo_total - 1].linha;
                eol.coluna = novo_vetor[novo_total - 1].coluna +
                             (int)strlen(novo_vetor[novo_total - 1].lexema);
                novo_vetor[novo_total++] = eol;
            }
        } else if (linha_anterior != -1 && tk.linha != linha_anterior &&
                   strcmp(novo_vetor[novo_total - 1].nome, "EOL") != 0) {
            Token eol;
            strcpy(eol.nome, "EOL");
            strcpy(eol.lexema, "EOL");
            eol.linha = linha_anterior;
            eol.coluna = a->tokens[i-1].coluna +
                         (int)strlen(a->tokens[i-1].lexema);
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
    a->erro_linha = 1;
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
    snprintf(buffer, sizeof(buffer), "%s\n%s", t->nome, t->lexema);
    return novo_no(buffer);
}

static void emitir_rotulo_dot(FILE* out, const char* rotulo) {
    for (const unsigned char* p = (const unsigned char*)rotulo; *p; p++) {
        switch (*p) {
            case '"': fputs("\\\"", out); break;
            case '\\': fputs("\\\\", out); break;
            case '\n': fputs("\\n", out); break;
            case '\r': fputs("\\r", out); break;
            case '\t': fputs("\\t", out); break;
            default:
                if (*p < 32) fprintf(out, "\\%03o", *p);
                else fputc(*p, out);
        }
    }
}

void emitir_dot(FILE* out, NoArvore* no) {
    if (!no) return;
    fprintf(out, "  n%d [label=\"", no->id);
    emitir_rotulo_dot(out, no->rotulo);
    fprintf(out, "\"];\n");

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

NoArvore* novo_nao_terminal(const char* rotulo) {
    return novo_no(rotulo);
}

NoArvore* consumir_no(AnaliseEstado* a) {
    Token* t = token_atual(a);
    if (!t) return NULL;
    NoArvore* no = no_terminal(t);
    avancar(a);
    return no;
}

static int eh_secao(AnaliseEstado* a) {
    return verificar(a, "DIR_DATA") || verificar(a, "DIR_TEXT");
}

static int eh_diretiva_dados(AnaliseEstado* a) {
    return verificar(a, "DIR_WORD") || verificar(a, "DIR_HALF") ||
           verificar(a, "DIR_BYTE") || verificar(a, "DIR_ASCII") ||
           verificar(a, "DIR_ASCIIZ") || verificar(a, "DIR_SPACE") ||
           verificar(a, "DIR_ALIGN");
}

static int eh_op_tres_registradores(AnaliseEstado* a) {
    return verificar(a, "INS_ADD") || verificar(a, "INS_ADDU") || verificar(a, "INS_SUB")
    || verificar(a, "INS_SUBU") || verificar(a, "INS_MUL") || verificar(a, "INS_AND")
    || verificar(a, "INS_OR") || verificar(a, "INS_XOR") || verificar(a, "INS_NOR")
    || verificar(a, "INS_SLT");
}

static int eh_op_dois_registradores(AnaliseEstado* a) {
    return verificar(a, "INS_MOVE") || verificar(a, "INS_DIV");
}

static int eh_op_um_registrador(AnaliseEstado* a) {
    return verificar(a, "INS_MFLO") || verificar(a, "INS_MFHI") || verificar(a, "INS_JR");
}

static int eh_op_deslocamento(AnaliseEstado* a) {
    return verificar(a, "INS_SLL") || verificar(a, "INS_SRL");
}

static int eh_op_memoria(AnaliseEstado* a) {
    return verificar(a, "INS_LW") || verificar(a, "INS_SW") || verificar(a, "INS_LB")
    || verificar(a, "INS_SB") || verificar(a, "INS_LH") || verificar(a, "INS_SH");
}

static int eh_op_desvio(AnaliseEstado* a) {
    return verificar(a, "INS_BEQ") || verificar(a, "INS_BNE") || verificar(a, "INS_BLT")
    || verificar(a, "INS_BLE") || verificar(a, "INS_BGT") || verificar(a, "INS_BGE");
}

static int eh_op_salto(AnaliseEstado* a) {
    return verificar(a, "INS_J") || verificar(a, "INS_JAL");
}

static int eh_instrucao_sem_operandos(AnaliseEstado* a) {
    return verificar(a, "INS_SYSCALL") || verificar(a, "INS_NOP");
}

static int consumir_eol(AnaliseEstado* a, NoArvore* no) {
    if (!verificar(a, "EOL")) {
        erro(a, "EOL");
        return 0;
    }
    adicionar_filho(no, consumir_no(a));
    return 1;
}

NoArvore* analise_rotulo(AnaliseEstado* a) {
    NoArvore* no = novo_nao_terminal("<rotulo>");
    if (!verificar(a, "ID")) {
        erro(a, "ID");
        return no;
    }
    adicionar_filho(no, consumir_no(a));
    if (!verificar(a, "SMB_COL")) {
        erro(a, "SMB_COL(:)");
        return no;
    }
    adicionar_filho(no, consumir_no(a));
    return no;
}

NoArvore* analise_diretiva_global(AnaliseEstado* a) {
    NoArvore* no = novo_nao_terminal("<diretiva_global>");
    if (!verificar(a, "DIR_GLOBL")) {
        erro(a, "DIR_GLOBL");
        return no;
    }
    adicionar_filho(no, consumir_no(a));
    if (!verificar(a, "ID")) {
        erro(a, "ID");
        return no;
    }
    adicionar_filho(no, consumir_no(a));
    return no;
}

NoArvore* analise_lista_inteiros(AnaliseEstado* a) {
    NoArvore* no = novo_nao_terminal("<lista_inteiros>");
    if (!verificar(a, "NUM_INT")) {
        erro(a, "NUM_INT");
        return no;
    }
    adicionar_filho(no, consumir_no(a));
    while (verificar(a, "SMB_COM")) {
        adicionar_filho(no, consumir_no(a));
        if (!verificar(a, "NUM_INT")) {
            erro(a, "NUM_INT após vírgula");
            return no;
        }
        adicionar_filho(no, consumir_no(a));
    }
    return no;
}

NoArvore* analise_diretiva_dados(AnaliseEstado* a) {
    NoArvore* no = novo_nao_terminal("<diretiva_dados>");
    if (verificar(a, "DIR_WORD") || verificar(a, "DIR_HALF") ||
        verificar(a, "DIR_BYTE")) {
        adicionar_filho(no, analise_diretiva_inteiros(a));
        adicionar_filho(no, analise_lista_inteiros(a));
    } else if (verificar(a, "DIR_ASCII") || verificar(a, "DIR_ASCIIZ")) {
        adicionar_filho(no, analise_diretiva_string(a));
        if (!verificar(a, "STRING")) {
            erro(a, "STRING");
            return no;
        }
        adicionar_filho(no, consumir_no(a));
    } else if (verificar(a, "DIR_SPACE") || verificar(a, "DIR_ALIGN")) {
        adicionar_filho(no, consumir_no(a));
        if (!verificar(a, "NUM_INT")) {
            erro(a, "NUM_INT");
            return no;
        }
        adicionar_filho(no, consumir_no(a));
    } else {
        erro(a, "diretiva de dados");
    }
    return no;
}

NoArvore* analise_linha_dados(AnaliseEstado* a) {
    a->erro_linha = 0;
    NoArvore* no = novo_nao_terminal("<linha_dados>");
    if (verificar(a, "ID")) {
        adicionar_filho(no, analise_rotulo(a));
        if (a->erro_linha) {
            return no;
        }
        if (eh_diretiva_dados(a)) {
            adicionar_filho(no, analise_diretiva_dados(a));
        }
    } else if (eh_diretiva_dados(a)) {
        adicionar_filho(no, analise_diretiva_dados(a));
    } else if (verificar(a, "DIR_GLOBL")) {
        adicionar_filho(no, analise_diretiva_global(a));
    } else {
        erro(a, "rótulo, diretiva de dados ou .globl");
        return no;
    }

    if (!a->erro_linha) {
        consumir_eol(a, no);
    }
    return no;
}

NoArvore* analise_linha_codigo(AnaliseEstado* a) {
    a->erro_linha = 0;
    NoArvore* no = novo_nao_terminal("<linha_codigo>");
    if (verificar(a, "ID")) {
        adicionar_filho(no, analise_rotulo(a));
        if (a->erro_linha) {
            return no;
        }
        if (verificar(a, "EOL")) {
            consumir_eol(a, no);
            return no;
        }
        Token* t = token_atual(a);
        if (!t || strncmp(t->nome, "INS_", 4) != 0) {
            erro(a, "instrução ou EOL após rótulo");
            return no;
        }
        int erro_antes = a->houve_erro;
        adicionar_filho(no, analise_instrucao(a));
        if (a->erro_linha) {
            consumir_eol(a, no);
        }
        return no;
    } else if (verificar(a, "DIR_GLOBL")) {
        adicionar_filho(no, analise_diretiva_global(a));
        if (!a->erro_linha) {
            consumir_eol(a, no);
        }
        return no;
    } else {
        Token* t = token_atual(a);
        if (!t || strncmp(t->nome, "INS_", 4) != 0) {
            erro(a, "rótulo, instrução ou .globl");
            return no;
        }
        adicionar_filho(no, analise_instrucao(a));
        if (a->erro_linha) {
            consumir_eol(a, no);
        }
        return no;
    }

    NoArvore* instrucao = novo_nao_terminal("<instrucao>");
    while (token_atual(a) && !verificar(a, "EOL") &&
           !verificar(a, "TK_EOF")) {
        adicionar_filho(instrucao, consumir_no(a));
    }
    adicionar_filho(no, instrucao);
    consumir_eol(a, no);
    return no;
}

NoArvore* analise_secao_dados(AnaliseEstado* a) {
    NoArvore* no = novo_nao_terminal("<secao_dados>");
    adicionar_filho(no, consumir_no(a));
    consumir_eol(a, no);
    while (token_atual(a) && !eh_secao(a) && !verificar(a, "TK_EOF")) {
        int posicao = a->atual;
        int linha = token_atual(a)->linha;
        adicionar_filho(no, analise_linha_dados(a));
        Token* proximo = token_atual(a);
        if (a->atual == posicao ||
            (proximo && strcmp(proximo->nome, "TK_EOF") &&
             proximo->linha <= linha)) {
            sicronizar_erro(a);
        }
    }
    return no;
}

NoArvore* analise_secao_codigo(AnaliseEstado* a) {
    NoArvore* no = novo_nao_terminal("<secao_codigo>");
    adicionar_filho(no, consumir_no(a));
    consumir_eol(a, no);
    while (token_atual(a) && !eh_secao(a) && !verificar(a, "TK_EOF")) {
        int posicao = a->atual;
        int linha = token_atual(a)->linha;
        adicionar_filho(no, analise_linha_codigo(a));
        Token* proximo = token_atual(a);
        if (a->atual == posicao ||
            (proximo && strcmp(proximo->nome, "TK_EOF") &&
             proximo->linha <= linha)) {
            sicronizar_erro(a);
        }
    }
    return no;
}

NoArvore* analise_secao(AnaliseEstado* a) {
    NoArvore* no = novo_nao_terminal("<secao>");
    adicionar_filho(no, verificar(a, "DIR_DATA")
        ? analise_secao_dados(a)
        : analise_secao_codigo(a));
    return no;
}

NoArvore* analise_programa(AnaliseEstado* a) {
    NoArvore* no = novo_nao_terminal("<programa>");
    if (!eh_secao(a)) {
        erro(a, "DIR_DATA ou DIR_TEXT no início do programa");
        return no;
    }
    while (eh_secao(a))
        adicionar_filho(no, analise_secao(a));
    if (verificar(a, "TK_EOF"))
        adicionar_filho(no, consumir_no(a));
    else
        erro(a, "TK_EOF");
    return no;
}

NoArvore* analise_diretiva_inteiros(AnaliseEstado* a) {
    NoArvore* no = novo_nao_terminal("<diretiva_inteiros>");
    if (verificar(a, "DIR_WORD") || verificar(a, "DIR_HALF") || verificar(a, "DIR_BYTE")) {
        adicionar_filho(no, consumir_no(a));
    } else {
        erro(a, ".word, .half ou .byte");
    }
    return no;
}

NoArvore* analise_diretiva_string(AnaliseEstado* a) {
    NoArvore* no = novo_nao_terminal("<diretiva_string>");
    if (verificar(a, "DIR_ASCII") || verificar(a, "DIR_ASCIIZ")) {
        adicionar_filho(no, consumir_no(a));
    } else {
        erro(a, ".ascii ou .asciiz");
    }
    return no;
}

NoArvore* analise_op_tres_registradores(AnaliseEstado* a) {
    NoArvore* no = novo_nao_terminal("<op_tres_registradores>");
    adicionar_filho(no, consumir_no(a));
    return no;
}

NoArvore* analise_instrucao_tres_registradores(AnaliseEstado* a) {
    NoArvore* no = novo_nao_terminal("<instrucao_tres_registradores>");

    adicionar_filho(no, analise_op_tres_registradores(a));
    if (!verificar(a, "REG")) {
        erro(a, "REG");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    if (!verificar(a, "SMB_COM")) {
        erro(a, "SMB_COM(\",\")");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    if (!verificar(a, "REG")) {
        erro(a, "REG");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    if (!verificar(a, "SMB_COM")) {
        erro(a, "SMB_COM(\",\")");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    if (!verificar(a, "REG")) {
        erro(a, "REG");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    return no;
}

NoArvore* analise_op_dois_registradores(AnaliseEstado* a) {
    NoArvore* no = novo_nao_terminal("<op_dois_registradores>");
    adicionar_filho(no, consumir_no(a));
    return no;
}

NoArvore* analise_instrucao_dois_registradores(AnaliseEstado* a) {
    NoArvore* no = novo_nao_terminal("<instrucao_dois_registradores>");

    adicionar_filho(no, analise_op_dois_registradores(a));
    if (!verificar(a, "REG")) {
        erro(a, "REG");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    if (!verificar(a, "SMB_COM")) {
        erro(a, "SMB_COM(\",\")");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    if(!verificar(a, "REG")) {
        erro(a, "REG");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    return no;
}

NoArvore* analise_op_um_registrador(AnaliseEstado* a) {
    NoArvore* no = novo_nao_terminal("<op_um_registrador>");
    adicionar_filho(no, consumir_no(a));
    return no;
}

NoArvore* analise_instrucao_um_registrador(AnaliseEstado* a) {
    NoArvore* no = novo_nao_terminal("<instrucao_um_registrador>");

    adicionar_filho(no, analise_op_um_registrador(a));
    if (!verificar(a, "REG")) {
        erro(a, "REG");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    return no;
}

NoArvore* analise_instrucao_imediato(AnaliseEstado* a) {
    NoArvore* no = novo_nao_terminal("<instrucao_imediato>");

    adicionar_filho(no, consumir_no(a));
    if (!verificar(a, "REG")) {
        erro(a, "REG");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    if (!verificar(a, "SMB_COM")) {
        erro(a, "SMB_COM(\",\")");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    if (!verificar(a, "REG")) {
        erro(a, "REG");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    if (!verificar(a, "SMB_COM")) {
        erro(a, "SMB_COM(\",\")");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    if (!verificar(a, "NUM_INT")) {
        erro(a, "NUM_INT");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    return no;
}

NoArvore* analise_op_deslocamento(AnaliseEstado* a) {
    NoArvore* no = novo_nao_terminal("<op_deslocamento>");
    adicionar_filho(no, consumir_no(a));
    return no;
}

NoArvore* analise_instrucao_deslocamento(AnaliseEstado* a) {
    NoArvore* no = novo_nao_terminal("<instrucao_deslocamento>");

    adicionar_filho(no, analise_op_deslocamento(a));
    if (!verificar(a, "REG")) {
        erro(a, "REG");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    if (!verificar(a, "SMB_COM")) {
        erro(a, "SMB_COM(\",\")");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    if (!verificar(a, "REG")) {
        erro(a, "REG");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    if (!verificar(a, "SMB_COM")) {
        erro(a, "SMB_COM(\",\")");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    if (!verificar(a, "NUM_INT")) {
        erro(a, "NUM_INT");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    return no;
}

NoArvore* analise_instrucao_li(AnaliseEstado* a) {
    NoArvore* no = novo_nao_terminal("<instrucao_li>");

    adicionar_filho(no, consumir_no(a));
    if (!verificar(a, "REG")) {
        erro(a, "REG");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    if (!verificar(a, "SMB_COM")) {
        erro(a, "SMB_COM(\",\")");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    if (!verificar(a, "NUM_INT")) {
        erro(a, "NUM_INT");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    return no;
}

NoArvore* analise_instrucao_la(AnaliseEstado* a) {
    NoArvore* no = novo_nao_terminal("<instrucao_la>");

    adicionar_filho(no, consumir_no(a));
    if (!verificar(a, "REG")) {
        erro(a, "REG");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    if (!verificar(a, "SMB_COM")) {
        erro(a, "SMB_COM(\",\")");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    if (!verificar(a, "ID")) {
        erro(a, "ID");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    return no;
}

NoArvore* analise_op_memoria(AnaliseEstado* a) {
    NoArvore* no = novo_nao_terminal("<op_memoria>");
    adicionar_filho(no, consumir_no(a));
    return no;
}

NoArvore* analise_endereco(AnaliseEstado* a) {
    NoArvore* no = novo_nao_terminal("<endereco>");

    if (verificar(a, "ID")) {
        adicionar_filho(no, consumir_no(a));
    } else if (verificar(a, "NUM_INT")) {
        adicionar_filho(no, consumir_no(a));
        if (!verificar(a, "SMB_OPA")) {
            erro(a, "SMB_OPA(\"(\")");
            return no;
        }
        adicionar_filho(no, consumir_no(a));
        if (!verificar(a, "REG")) {
            erro(a, "REG");
            return no;
        }
        adicionar_filho(no, consumir_no(a));
        if (!verificar(a, "SMB_CPA")) {
            erro(a, "SMB_CPA(\")\")");
            return no;
        }
        adicionar_filho(no, consumir_no(a));
    } else {
        erro(a, "ID ou NUM_INT");
    }

    return no;
}

NoArvore* analise_instrucao_memoria(AnaliseEstado* a) {
    NoArvore* no = novo_nao_terminal("<instrucao_memoria>");
    adicionar_filho(no, analise_op_memoria(a));
    if (!verificar(a, "REG")) {
        erro(a, "REG");
        return no;
    }
    adicionar_filho(no, consumir_no(a));
    if (!verificar(a, "SMB_COM")) {
        erro(a, "SMB_COM(\",\")");
        return no;
    }
    adicionar_filho(no, consumir_no(a));
    adicionar_filho(no, analise_endereco(a));
    return no;
}

NoArvore* analise_op_desvio(AnaliseEstado* a) {
    NoArvore* no = novo_nao_terminal("<op_desvio>");
    adicionar_filho(no, consumir_no(a));
    return no;
}

NoArvore* analise_instrucao_desvio(AnaliseEstado* a) {
    NoArvore* no = novo_nao_terminal("<instrucao_desvio>");

    adicionar_filho(no, analise_op_desvio(a));
    if (!verificar(a, "REG")) {
        erro(a, "REG");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    if (!verificar(a, "SMB_COM")) {
        erro(a, "SMB_COM(\",\")");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    if (!verificar(a, "REG")) {
        erro(a, "REG");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    if (!verificar(a, "SMB_COM")) {
        erro(a, "SMB_COM(\",\")");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    if (!verificar(a, "ID")) {
        erro(a, "ID");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    return no;
}

NoArvore* analise_op_salto(AnaliseEstado* a) {
    NoArvore* no = novo_nao_terminal("<op_salto>");
    adicionar_filho(no, consumir_no(a));
    return no;
}

NoArvore* analise_instrucao_salto(AnaliseEstado* a) {
    NoArvore* no = novo_nao_terminal("<instrucao_salto>");

    adicionar_filho(no, analise_op_salto(a));
    if (!verificar(a, "ID")) {
        erro(a, "ID");
        return no;
    }

    adicionar_filho(no, consumir_no(a));
    return no;
}

NoArvore* analise_instrucao_sem_operandos(AnaliseEstado* a) {
    NoArvore* no = novo_nao_terminal("<instrucao_sem_operandos>");
    adicionar_filho(no, consumir_no(a));
    return no;
}

NoArvore* analise_instrucao(AnaliseEstado* a) {
    NoArvore* no = novo_nao_terminal("<instrucao>");
    if (eh_op_tres_registradores(a)) {
        adicionar_filho(no, analise_instrucao_tres_registradores(a));
    } else if (eh_op_dois_registradores(a)) {
        adicionar_filho(no, analise_instrucao_dois_registradores(a));
    } else if (eh_op_um_registrador(a)) {
        adicionar_filho(no, analise_instrucao_um_registrador(a));
    } else if (verificar(a, "INS_ADDI")) {
        adicionar_filho(no, analise_instrucao_imediato(a));
    } else if (eh_op_deslocamento(a)) {
        adicionar_filho(no, analise_instrucao_deslocamento(a));
    } else if (verificar(a, "INS_LI")) {
        adicionar_filho(no, analise_instrucao_li(a));
    } else if (verificar(a, "INS_LA")) {
        adicionar_filho(no, analise_instrucao_la(a));
    } else if (eh_op_memoria(a)) {
        adicionar_filho(no, analise_instrucao_memoria(a));
    } else if (eh_op_desvio(a)) {
        adicionar_filho(no, analise_instrucao_desvio(a));
    } else if (eh_op_salto(a)) {
        adicionar_filho(no, analise_instrucao_salto(a));
    } else if (eh_instrucao_sem_operandos(a)) {
        adicionar_filho(no, analise_instrucao_sem_operandos(a));
    } else {
        erro(a, "instrução válida");
    }

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
    g_proximo_id = 0;

    if (carregar_tokens(tokens, &a) != 0) {
        liberar_tokens(&a);
        return -1;
    }
    if (inserir_eol_e_eof(&a) != 0) { liberar_tokens(&a); return -1; }

    NoArvore* raiz = analise_programa(&a);

    if (!a.houve_erro && raiz) {
        fprintf(arvore, "digraph ArvoreSintatica {\n");
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

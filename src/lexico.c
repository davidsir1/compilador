#include "lexico.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// Tabelas auxiliares

static const char* DIRETIVAS[] = {
    ".data", ".text", ".word", ".half", ".byte",
    ".space", ".ascii", ".asciiz", ".globl", ".align"
};

static const char* INSTRUCOES[] = {
    // Transferência de dados
    "li", "la", "move", "lw", "sw", "lb", "sb", "lh", "sh",
    // Operações aritméticas
    "add", "addi", "addu", "sub", "subu", "mul", "div", "mflo", "mfhi",
    // Operações lógicas, deslocamento e comparação
    "and", "or", "xor", "nor", "sll", "srl", "slt",
    // Desvio e controle
    "beq", "bne", "blt", "ble", "bgt", "j", "jal", "jr", "syscall", "nop"
};

static const char* REGISTRADORES[] = {
    "$zero",
    "$v0", "$v1",
    "$a0", "$a1", "$a2", "$a3",
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9",
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
    "$k0", "$k1",
    "$gp", "$sp", "$fp", "$ra",
    // Formas númericas
    "$0", "$1", "$2", "$3", "$4", "$5", "$6", "$7", "$8", "$9",
    "$10", "$11", "$12", "$13", "$14", "$15", "$16", "$17", "$18", "$19",
    "$20", "$21", "$22", "$23", "$24", "$25", "$26", "$27", "$28", "$29",
    "$30", "$31"
};

// Funções
/*
 * Função utilizada para montar o token
 */
static Token montar_token(const char* nome, const char* lexema, int linha, int coluna) {
    Token tk;
    strncpy(tk.nome, nome, 49);
    tk.nome[49] = '\0';
    strncpy(tk.lexema, lexema, 99);
    tk.lexema[99] = '\0';
    tk.linha = linha;
    tk.coluna = coluna;
    return tk;
}

void inicializar_tabela(TabelaSimbolos* ts) {
    if (ts == NULL) return;

    ts->total = 0;

    // Diretivas
    for (int i = 0; i < sizeof(DIRETIVAS)/sizeof(DIRETIVAS[0]); i++) {
        inserir_simbolo(ts, DIRETIVAS[i], "diretiva", 0, 0);
    }
    // Instruções
    for (int i = 0; i < sizeof(INSTRUCOES)/sizeof(INSTRUCOES[0]); i++) {
        inserir_simbolo(ts, INSTRUCOES[i], "instrucao", 0, 0);
    }
    // Registradores
    for (int i = 0; i < sizeof(REGISTRADORES)/sizeof(REGISTRADORES[0]); i++) {
        inserir_simbolo(ts, REGISTRADORES[i], "registrador", 0, 0);
    }
}

int buscar_simbolo(TabelaSimbolos *ts, const char *lexema) {
    if (ts == NULL || lexema == NULL) return -1;

    for (int i = 0; i < ts->total; i++) {
        if (strcmp(ts->entradas[i].lexema, lexema) == 0)
            return i;
    }

    return -1; // Caso ele não encontre o simbolo
}

int inserir_simbolo(TabelaSimbolos *ts, const char *lexema, const char *categoria, int linha, int coluna) {
    if (ts == NULL || lexema == NULL || categoria == NULL) return -1;

    int indice = buscar_simbolo(ts, lexema);

    if (indice >= 0) {
        if (ts->entradas[indice].primeiraLinha == 0 &&
        ts->entradas[indice].primeiraColuna == 0 &&
        linha > 0 && coluna > 0) {
            ts->entradas[indice].primeiraLinha = linha;
            ts->entradas[indice].primeiraColuna = coluna;
        }
        return 0; // Já inseriu
    }

    // Verifica se o total de simbolos da tabela ultrapassou o limite definido
    if (ts->total >= MAX_SIMBOLOS) return -1;

    Simbolo* nova = &ts->entradas[ts->total];
    strncpy(nova->lexema, lexema, 100);
    nova->lexema[99] = '\0';
    strncpy(nova->categoria, categoria, 30);
    nova->categoria[29] = '\0';
    nova->primeiraLinha = linha;
    nova->primeiraColuna = coluna;

    ts->total++;
    return 1;
}

void imprimir_tabela(TabelaSimbolos *ts, FILE *saida) {
    if (ts == NULL || saida == NULL) return;

    fprintf(saida, "LEXEMA,CATEGORIA,LINHA,COLUNA\n");

    for (int i = 0; i < ts->total; i++) {
        Simbolo* s = &ts->entradas[i];
        fprintf(saida, "%s,%s,%d,%d\n", s->lexema, s->categoria, s->primeiraLinha, s->primeiraColuna);
    }
}

void converter_minusculas(const char *origem, char *destino) {
    int i;
    for (i = 0; origem[i] != '\0'; i++) {
        destino[i] = (char)tolower(origem[i]);
    }
    destino[i] = '\0';
}

void converter_maiusculas(const char *origem, char *destino) {
    int i;
    for (i = 0; origem[i] != '\0'; i++) {
        destino[i] = (char)toupper(origem[i]);
    }
    destino[i] = '\0';
}

Token continuar_numero(FILE* in, int linha, int coluna, char* lexema, int i) {
    int caracter;
    int hex = 0; // flag para dizer se é hexadecimal

    // Fase 1: dígitos decimais q0 --> q5
    while ((caracter = fgetc(in)) != EOF) {
        if (isdigit(caracter)) {
            if (i < 99) lexema[i++] = caracter;
        } else if ((caracter == 'x' || caracter == 'X') && // q5 --> q6
        ((i == 1 && lexema[0] == '0') ||
        (i == 2 && lexema[0] == '-' && lexema[1] == '0'))) { // '0x' ou '-0x' são hexadecimais
            if (i < 99) lexema[i++] = caracter;
            hex = 1;
            break;
        } else {
            break;
        }
    }

    // Fase 2: hexadecimal
    if (hex) {
        int valido = 0; // 0 - ainda sem dígito, 1 = ok, -1 = inválido
        while ((caracter = fgetc(in)) != EOF) {
            if (isxdigit(caracter)) {
                if (i < 99) lexema[i++] = caracter;
                if (valido == 0) valido = 1;
            } else if (isalnum(caracter)) {
                // Letra diferente de A, B, C, D, E e F: inválido. Continua lendo
                if (i < 99) lexema[i++] = caracter;
                valido = -1;
            } else {
                break;
            }
        }

        lexema[i] = '\0';
        if (caracter != EOF) ungetc(caracter, in);

        if (valido != 1) {
            return montar_token("ERRO_NUMERO_MALFORMADO", lexema, linha, coluna);
        }
        return montar_token("NUM_INT", lexema, linha, coluna);
    }

    // Fim: apenas decimal
    lexema[i] = '\0';
    if (caracter != EOF) ungetc(caracter, in);
    return montar_token("NUM_INT", lexema, linha, coluna);
}

Token reconhecer_diretiva(FILE *in, int linha, int coluna, int primeiro_char, TabelaSimbolos *ts) {
    char lexema[100];
    char normalizado[100];
    char nome[50]; // Variável utilizada para montar o token DIR_*
    int i = 0;
    int caracter;

    lexema[i++] = (char)primeiro_char;

    // Letras, digitos e '_' (etapa de acumular)
    while ((caracter = fgetc(in)) != EOF) {
        if (isalnum(caracter) || caracter == '_') {
            if (i < 99) lexema[i++] = (char)caracter;
        } else {
            break;
        }
    }

    lexema[i] = '\0';
    if (caracter != EOF) ungetc(caracter, in);

    // Normaliza para consulta na tabela de simbolos
    converter_minusculas(lexema, normalizado);
    int indice = buscar_simbolo(ts, normalizado);

    if (indice < 0 || strcmp(ts->entradas[indice].categoria, "diretiva") != 0) {
        return montar_token("ERRO_DIRETIVA_INVALIDA", lexema, linha, coluna);
    }

    // Monta o nome do token: DIR_ + nome em maiúsculas sem o ponto
    char maiusculo[100];
    converter_maiusculas(normalizado + 1, maiusculo); // + 1 é pra ignorar o '.'
    snprintf(nome, sizeof(nome), "DIR_%s", maiusculo);

    // atualiza a linha/coluna da primeira ocorrência da diretiva
    inserir_simbolo(ts, normalizado, "diretiva", linha, coluna);

    return montar_token(nome, lexema, linha, coluna);
}

Token reconhecer_registrador(FILE *in, int linha, int coluna, int primeiro_char, TabelaSimbolos *ts) {
    char lexema[100];
    char normalizado[100];
    int i = 0;
    int caracter;

    // acumula o '$'
    lexema[i++] = (char)primeiro_char;

    // Continua lendo letras e digitos
    while ((caracter = fgetc(in)) != EOF) {
        if (isalnum(caracter)) {
            if (i < 99) lexema[i++] = (char)caracter;
        } else {
            break;
        }
    }

    lexema[i] = '\0';
    if (caracter != EOF) ungetc(caracter, in);

    // Normaliza para consulta na tabela de simbolos
    converter_minusculas(lexema, normalizado);
    int indice = buscar_simbolo(ts, normalizado);

    if (indice < 0 || strcmp(ts->entradas[indice].categoria, "registrador") != 0) {
        return montar_token("ERRO_REGISTRADOR_INVALIDO", lexema, linha, coluna);
    }

    // atualiza a linha/coluna da primeira ocorrência do registrador
    inserir_simbolo(ts, normalizado, "registrador", linha, coluna);

    return montar_token("REG", lexema, linha, coluna);
}

Token reconhecer_identificador_ou_instrucao(FILE *in, int linha, int coluna, int primeiro_char, TabelaSimbolos *ts) {
    char lexema[100];
    char normalizado[100];
    char maiusculo[100];
    char nome[50];
    int i = 0;
    int caracter;

    lexema[i++] = (char)primeiro_char;

    while ((caracter = fgetc(in)) != EOF) {
        if (isalnum(caracter) || caracter == '_') {
            if (i < 99) lexema[i++] = (char)caracter;
        } else {
            break;
        }
    }

    lexema[i] = '\0';
    if (caracter != EOF) ungetc(caracter, in);

    // Normaliza para consultar na tabela de simbolos
    converter_minusculas(lexema, normalizado);
    int indice = buscar_simbolo(ts, normalizado);

    // Se for uma instrução, monta o nome INS_*
    if (indice >= 0 && strcmp(ts->entradas[indice].categoria, "instrucao") == 0) {
        converter_maiusculas(lexema, maiusculo);
        snprintf(nome, sizeof(nome), "INS_%s", maiusculo);

        inserir_simbolo(ts, normalizado, "instrucao", linha, coluna);

        return montar_token(nome, lexema, linha, coluna);
    }

    // caso contrário, é um identificador ID
    // Se o próximo caracter for ':', é declaração de rótulo
    if (caracter == ':') {
        inserir_simbolo(ts, lexema, "identificador/rotulo", linha, coluna);
    }

    // Se não for ':', é só referência, não insere nada
    return montar_token("ID", lexema, linha, coluna);
}

Token reconhecer_numero(FILE *in, int linha, int coluna, int primeiro_char) {
    char lexema[100];
    int i = 0;

    lexema[i++] = (char)primeiro_char;

    return continuar_numero(in, linha, coluna, lexema, i);
}

Token reconhecer_negativo(FILE *in, int linha, int coluna, int primeiro_char) {
    char lexema[100];
    int i = 0;
    int caracter;

    lexema[i++] = '-';

    caracter = fgetc(in);

    if (caracter == EOF) {
        lexema[i] = '\0';
        return montar_token("ERRO_CARACTER_INVALIDO", lexema, linha, coluna);
    }

    if (!isalnum(caracter)) {
        ungetc(caracter, in);
        lexema[i] = '\0';
        return montar_token("ERRO_CARACTER_INVALIDO", lexema, linha, coluna);
    }

    lexema[i++] = (char)caracter;
    return continuar_numero(in, linha, coluna, lexema, i);
}

Token reconhecer_string(FILE *in, int linha, int coluna, int primeiro_char) {
    char lexema[100];
    int i = 0;
    int caracter;
    int tem_escape_invalido = 0; // Flag utilizada para verificar escape

    lexema[i++] = '"';

    while ((caracter = fgetc(in)) != EOF) {
        // Fechamento de string
        if (caracter == '"') {
            if (i < 99) lexema[i++] = '"';
            lexema[i] = '\0';

            if (tem_escape_invalido) {
                return montar_token("ERRO_ESCAPE_INVALIDO", lexema, linha, coluna);
            }

            return montar_token("STRING", lexema, linha, coluna);
        }

        // Quebra de linha antes de fechar -> erro
        if (caracter == '\n') {
            lexema[i] = '\0';
            ungetc(caracter, in); // devolve o \n para o traço de repetição contar a linha
            return montar_token("ERRO_STRING_NAO_FECHADA", lexema, linha, coluna);
        }

        // Inicio de escape
        if (caracter == '\\') {
            if (i < 99) lexema[i++] = '\\';

            int caracter2 = fgetc(in);

            if (caracter2 == EOF) {
                lexema[i] = '\0';
                return montar_token("ERRO_STRING_NAO_FECHADA", lexema, linha, coluna);
            }

            if (caracter2 == '\n') {
                // '\' no fim da linha -> string não fechada
                lexema[i] = '\0';
                ungetc(caracter2, in);
                return montar_token("ERRO_STRING_NAO_FECHADA", lexema, linha, coluna);
            }

            if (i < 99) lexema[i++] = (char)caracter2;

            // verifica se é escape válido
            if (!(caracter2 == 'n' || caracter2 == 't' || caracter2 == '"' || caracter2 == '\\' || caracter2 == '0'))
                tem_escape_invalido = 1;

            continue;
        }

        // Caractere comum dentro da string
        if (i < 99) lexema[i++] = (char)caracter;
    }

    // EOF antes de fechar a string
    lexema[i] = '\0';
    return montar_token("ERRO_STRING_NAO_FECHADA", lexema, linha, coluna);
}

Token reconhecer_simbolo(int caracter, int linha, int coluna) {
    Token tk;

    tk.lexema[0] = (char)caracter;
    tk.lexema[1] = '\0';
    tk.linha = linha;
    tk.coluna = coluna;

    switch (caracter) {
        case ',': strcpy(tk.nome, "SMB_COM"); break;
        case ':': strcpy(tk.nome, "SMB_COL"); break;
        case '(': strcpy(tk.nome, "SMB_OPA"); break;
        case ')': strcpy(tk.nome, "SMB_CPA"); break;
        default: strcpy(tk.nome, "ERRO_CARACTER_INVALIDO"); break;
    }
    return tk;
}

/*
 * Função principal para Analise Lexica
 */
void AnaliseLexica(FILE *in, FILE *out) {
    if (in == NULL || out == NULL) return;

    // Arquivos de saida da tabela de simbolos e erros
    FILE* saida_ts = fopen("saida.ts", "w");
    FILE* saida_err = fopen("saida.err", "w");
    if (saida_ts == NULL || saida_err == NULL) {
        printf("Erro: falha ao escrever os arquivos saida.ts ou saida.err.\n");
        if (saida_ts) fclose(saida_ts);
        if (saida_err) fclose(saida_err);
        return;
    }

    // Inicializar a tabela
    TabelaSimbolos ts;
    inicializar_tabela(&ts);

    int linha = 1, coluna = 1, houve_erro = 0;
    int caracter;

    while ((caracter = fgetc(in)) != EOF) {
        // Ignorar espaços, tabulações
        if (caracter == ' ' || caracter == '\t') {coluna++; continue;}

        // Nova linha quando encontra o fim da linha do arquivo
        if (caracter == '\n') {linha++; coluna = 1; continue;}

        // Comentário
        if (caracter == '#') {
            // Essa parte tem que ler o que está após '#' e quando chegar no final '\n'
            // incrementar uma linha e atribuir coluna = 1
            while ((caracter = fgetc(in)) != EOF) {}
            linha += 1;
            coluna = 1;
            continue;
        }

        Token tk;
        if (isalpha(caracter) || caracter == '_') { // Estado q0 -> q1
            tk = reconhecer_identificador_ou_instrucao(in, linha, coluna, caracter, &ts);
        } else if (caracter == '.') { // Estado q0 -> q2
            tk = reconhecer_diretiva(in, linha, coluna, caracter, &ts);
        } else if (caracter == '$') { // Estado q0 -> q3
            tk = reconhecer_registrador(in, linha, coluna, caracter, &ts);
        } else if (isdigit(caracter)) { // Estado q0 -> q5
            tk = reconhecer_numero(in, linha, coluna, caracter);
        } else if (caracter == '-') { // Estado q0 -> q4
            tk = reconhecer_negativo(in, linha, coluna, caracter);
        } else if (caracter == '"') { // Estado q0 -> q9
            tk = reconhecer_string(in, linha, coluna, caracter);
        } else if (caracter == ',' || caracter == ':' || caracter == '(' || caracter == ')') {
            // Estados q0 --','--> q13   q0 --':'--> q14   q0 --'('--> q15  q0 --')'--> q16
            tk = reconhecer_simbolo(caracter, linha, coluna);
        } else { // q0 -> q12

        }

        // Escrever na saída .lex
        /*
         * A saída do lexema deve ser: <token, lexema> linha coluna
         */
        fprintf(out, "<%s, %s> %d %d\n", tk.nome, tk.lexema, tk.linha, tk.coluna);

        // Verificar se tiver erro, registrar no .err
        /*
         * será escrito no saida_err com a seguinte estrutura: <tipo_erro, lexema> linha coluna
         * e dize que houve erro: houve_erro = 1
         */
        if (strncmp(tk.nome, "ERRO_", 5) == 0) {

        }

        coluna += (int)strlen(tk.lexema);
    }

    // Escrever o EOF no fim da 'FILE* in' .lex

    imprimir_tabela(&ts, saida_ts);

    if (!houve_erro) {
        fprintf(saida_err, "Nenhum erro léxico encontrado.");
    }

    fclose(saida_ts);
    fclose(saida_err);
}

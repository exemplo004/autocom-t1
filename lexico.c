#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexico.h"

typedef struct {
    const char * lexema;
    const char * token;
} PalavraReservada;

// Tabela com as 10 diretivas no documento
static const PalavraReservada TABELA_DIRETIVAS[] = {
    {".data",   "DIR_DATA"},
    {".text",   "DIR_TEXT"},
    {".word",   "DIR_WORD"},
    {".half",   "DIR_HALF"},
    {".byte",   "DIR_BYTE"},
    {".space",  "DIR_SPACE"},
    {".ascii",  "DIR_ASCII"},
    {".asciiz", "DIR_ASCIIZ"},
    {".globl",  "DIR_GLOBL"},
    {".align",  "DIR_ALIGN"}
};
#define QTD_DIRETIVAS 10 // Não acho que precise do item / sizeof(item) pra conseguir a quantidade, mas fica aqui um possível bug

static const PalavraReservada TABELA_INSTRUCOES[] = {
    {"li", "INS_LI"}, {"la", "INS_LA"}, {"move", "INS_MOVE"},
    {"lw", "INS_LW"}, {"sw", "INS_SW"}, {"lb", "INS_LB"}, {"sb", "INS_SB"},
    {"lh", "INS_LH"}, {"sh", "INS_SH"},

    {"add", "INS_ADD"}, {"addi", "INS_ADDI"}, {"addu", "INS_ADDU"},
    {"sub", "INS_SUB"}, {"subu", "INS_SUBU"}, {"mul", "INS_MUL"},
    {"div", "INS_DIV"}, {"mflo", "INS_MFLO"}, {"mfhi", "INS_MFHI"},

    {"and", "INS_AND"}, {"or", "INS_OR"}, {"xor", "INS_XOR"}, {"nor", "INS_NOR"},
    {"sll", "INS_SLL"}, {"srl", "INS_SRL"}, {"slt", "INS_SLT"},

    {"beq", "INS_BEQ"}, {"bne", "INS_BNE"}, {"blt", "INS_BLT"}, {"ble", "INS_BLE"},
    {"bgt", "INS_BGT"}, {"bge", "INS_BGE"}, {"j", "INS_J"}, {"jal", "INS_JAL"},
    {"jr", "INS_JR"}, {"syscall", "INS_SYSCALL"}, {"nop", "INS_NOP"}
};
#define QTD_INSTRUCOES (int)(sizeof(TABELA_INSTRUCOES) / sizeof(TABELA_INSTRUCOES[0]))

static const char * TABELA_NOMES_REGISTRADORES[] = {
    "zero",
    "v0", "v1",
    "a0", "a1", "a2", "a3",
    "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9",
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
    "k0", "k1",
    "gp", "sp", "fp", "ra"
};
#define QTD_NOMES_REGISTRADORES (int)(sizeof(TABELA_NOMES_REGISTRADORES) / sizeof(TABELA_NOMES_REGISTRADORES[0]))

static void ParaMinusculo(const char * origem, char * destino)
{
    int i;
    for (i = 0; origem[i] != '\0'; i++) {
        destino[i] = (char)tolower((unsigned char)origem[i]);
    }
    destino[i] = '\0';
}

// Busca linear simples
static const char * BuscarDiretiva(const char * lexemaMinusculo)
{
    for (int i = 0; i < QTD_DIRETIVAS; i++) {
        if (strcmp(lexemaMinusculo, TABELA_DIRETIVAS[i].lexema) == 0) {
            return TABELA_DIRETIVAS[i].token;
        }
    }
    return NULL;
}

static const char * BuscarInstrucao(const char * lexemaMinusculo)
{
    for (int i = 0; i < QTD_INSTRUCOES; i++) {
        if (strcmp(lexemaMinusculo, TABELA_INSTRUCOES[i].lexema) == 0) {
            return TABELA_INSTRUCOES[i].token;
        }
    }
    return NULL;
}

// lexema aqui já vem SEM o '$' (ex.: "t0", "31").
static int RegistradorValido(const char * lexemaSemCifrao)
{
    if (lexemaSemCifrao[0] == '\0') return 0;

    // Caso 1: forma numérica ($0 a $31)
    int souNumero = 1;
    for (const char * p = lexemaSemCifrao; *p != '\0'; p++) {
        if (!isdigit((unsigned char)*p)) { souNumero = 0; break; }
    }
    if (souNumero) {
        int valor = atoi(lexemaSemCifrao);
        return (valor >= 0 && valor <= 31);
    }

    // Caso 2: nome de registrador (comparação sem diferenciar letra maiúscula
    char minusculo[100];
    ParaMinusculo(lexemaSemCifrao, minusculo);
    for (int i = 0; i < QTD_NOMES_REGISTRADORES; i++) {
        if (strcmp(minusculo, TABELA_NOMES_REGISTRADORES[i]) == 0) return 1;
    }
    return 0;
}

#define MAX_SIMBOLOS 4096
static Simbolo tabelaSimbolos[MAX_SIMBOLOS];
static int totalSimbolos = 0;

static int BuscarSimbolo(const char * lexema)
{
    for (int i = 0; i < totalSimbolos; i++) {
        if (strcmp(tabelaSimbolos[i].lexema, lexema) == 0) return i;
    }
    return -1;
}

// Insere um símbolo novo, ou atualiza a posição de um símbolo
static void InserirSimbolo(const char * lexema, const char * categoria, int linha, int coluna)
{
    int idx = BuscarSimbolo(lexema);

    if (idx == -1) {
        if (totalSimbolos < MAX_SIMBOLOS) {
            strncpy(tabelaSimbolos[totalSimbolos].lexema, lexema, sizeof(tabelaSimbolos[0].lexema) - 1);
            tabelaSimbolos[totalSimbolos].lexema[sizeof(tabelaSimbolos[0].lexema) - 1] = '\0';

            strncpy(tabelaSimbolos[totalSimbolos].categoria, categoria, sizeof(tabelaSimbolos[0].categoria) - 1);
            tabelaSimbolos[totalSimbolos].categoria[sizeof(tabelaSimbolos[0].categoria) - 1] = '\0';

            tabelaSimbolos[totalSimbolos].primeiraLinha = linha;
            tabelaSimbolos[totalSimbolos].primeiraColuna = coluna;
            totalSimbolos++;
        }
    } else if (tabelaSimbolos[idx].primeiraLinha == 0
            && tabelaSimbolos[idx].primeiraColuna == 0
            && linha != 0) {
        // Símbolo já existia com posição zerada
        tabelaSimbolos[idx].primeiraLinha = linha;
        tabelaSimbolos[idx].primeiraColuna = coluna;
    }
    // Se já existia com posição != 0, é uma reocorrência
}

// Carrega a Tabela de Símbolos com todas as instruções, diretivas e registradores previstos, com posição 0,0
void InicializarTabelaSimbolos()
{
    totalSimbolos = 0;

    for (int i = 0; i < QTD_DIRETIVAS; i++) {
        InserirSimbolo(TABELA_DIRETIVAS[i].lexema, "diretiva", 0, 0);
    }
    for (int i = 0; i < QTD_INSTRUCOES; i++) {
        InserirSimbolo(TABELA_INSTRUCOES[i].lexema, "instrução", 0, 0);
    }

    char buf[10];
    for (int i = 0; i < QTD_NOMES_REGISTRADORES; i++) {
        sprintf(buf, "$%s", TABELA_NOMES_REGISTRADORES[i]);
        InserirSimbolo(buf, "registrador", 0, 0);
    }
    for (int n = 0; n <= 31; n++) {
        sprintf(buf, "$%d", n);
        InserirSimbolo(buf, "registrador", 0, 0);
    }
}

typedef struct {
    char tipo[40];
    char lexema[100];
    int linha;
    int coluna;
} ErroLexico;

#define MAX_ERROS 4096
ErroLexico tabelaErros[MAX_ERROS];
int totalErros = 0;

void RegistrarErro(char * tipo, char * lexema, int linha, int coluna)
{
    if (totalErros >= MAX_ERROS) return;
    char * s_tipo = strcat(tipo, "\0");
    char * s_lexema = strcat(lexema, "\0");

    strcpy(tabelaErros[totalErros].tipo, s_tipo);
    strcpy(tabelaErros[totalErros].lexema, s_lexema);

    tabelaErros[totalErros].linha = linha;
    tabelaErros[totalErros].coluna = coluna;
    totalErros++;
}

// Cursores (ou controladores) globais utilizados para a leitura dos arquivos, principalmente por LerChar()
// É int mas na verdade é char. Confia em mim.
int g_linha = 1;         // Número da linha
int g_coluna = 1;        // Número da coluna
int g_linhaAntes = 1;    // Linha salva ANTES do último LerChar
int g_colunaAntes = 1;   // Coluna salva ANTES do último LerChar
int g_temDevolucao = 0;  // 1 se há um caractere "devolvido" pendente
int g_charDevolvido = 0;

// Lê um caractere, verificando antes se houve devolução
static int LerChar(FILE * in)
{
    int c;

    if (g_temDevolucao) {
        g_temDevolucao = 0;
        c = g_charDevolvido;
    } else {
        c = getc(in);
    }

    // Salva onde estávamos no caso de devolução
    g_linhaAntes = g_linha;
    g_colunaAntes = g_coluna;

    if (c == '\n') {
        g_linha++;
        g_coluna = 1;
    } else if (c != EOF) {
        g_coluna++;
    }
    return c;
}

static void DevolverChar(int c)
{
    if (c == EOF) return;
    g_temDevolucao = 1;
    g_charDevolvido = c;
    g_linha = g_linhaAntes;
    g_coluna = g_colunaAntes;
}

// Consome espaços, tabs, quebras de linha e comentários (iniciados por '#'), usando DevolverChar() para evitar caracteres inúteis
static void PularEspacosComentarios(FILE * in)
{
    int c;
    while ((c = LerChar(in)) != EOF) {
        if (c == ' ' || c == '\t' || c == '\n') {
            continue;
        } else if (c == '#') {
            while ((c = LerChar(in)) != '\n' && c != EOF) {
                // Esgota o comentário e não gera token
            }
            continue;
        } else {
            DevolverChar(c);
            break;
        }
    }
}

static int LerIdentificadorOuInstrucao(FILE * in, int primeiroChar, int linha, int coluna, Token *tOut)
{
    char lexema[100];
    int idx = 0;
    lexema[idx++] = (char)primeiroChar;

    int c;
    while (idx < 99 && (isalnum((unsigned char)(c = LerChar(in))) || c == '_')) {
        lexema[idx++] = (char)c;
    }
    DevolverChar(c);
    lexema[idx] = '\0';

    char lexemaMinusculo[100];
    ParaMinusculo(lexema, lexemaMinusculo);

    const char * tokenInstrucao = BuscarInstrucao(lexemaMinusculo);
    if (tokenInstrucao != NULL) {

        strcpy(tOut->nome, tokenInstrucao);
        strcpy(tOut->lexema, lexema); // mantém a grafia original no .lex
        tOut->linha = linha;
        tOut->coluna = coluna;
        InserirSimbolo(lexemaMinusculo, "instrução", linha, coluna);
    } else {
        strcpy(tOut->nome, "ID");
        strcpy(tOut->lexema, lexema);
        tOut->linha = linha;
        tOut->coluna = coluna;
        // rótulos são sensíveis a maiúsculas/minúsculas (seção 2.1), por isso o lexema
        InserirSimbolo(lexema, "identificador/rótulo", linha, coluna);
    }
    return 1;
}

static int LerDiretiva(FILE * in, int linha, int coluna, Token *tOut)
{
    char lexema[100];
    int idx = 0;
    lexema[idx++] = '.';

    int c;
    while (idx < 99 && isalpha((unsigned char)(c = LerChar(in)))) {
        lexema[idx++] = (char)c;
    }
    DevolverChar(c);
    lexema[idx] = '\0';

    char lexemaMinusculo[100];
    ParaMinusculo(lexema, lexemaMinusculo);

    const char * tokenDiretiva = BuscarDiretiva(lexemaMinusculo);
    if (tokenDiretiva != NULL) {
        strcpy(tOut->nome, tokenDiretiva);
        strcpy(tOut->lexema, lexema);
        tOut->linha = linha;
        tOut->coluna = coluna;
        InserirSimbolo(lexemaMinusculo, "diretiva", linha, coluna);
        return 1;
    } else {
        RegistrarErro("ERRO_DIRETIVA_INVALIDA_OU_MALFORMADA", lexema, linha, coluna);
        return 0;
    }
}

static int LerRegistrador(FILE * in, int linha, int coluna, Token *tOut)
{
    char lexema[100];
    int idx = 0;
    lexema[idx++] = '$';

    int c;
    while (idx < 99 && isalnum((unsigned char)(c = LerChar(in)))) {
        lexema[idx++] = (char)c;
    }
    DevolverChar(c);
    lexema[idx] = '\0';

    if (RegistradorValido(lexema + 1)) { // +1 pra pular o '$' na checagem
        strcpy(tOut->nome, "REG");
        strcpy(tOut->lexema, lexema);
        tOut->linha = linha;
        tOut->coluna = coluna;

        char lexemaMinusculo[100];
        ParaMinusculo(lexema, lexemaMinusculo);
        InserirSimbolo(lexemaMinusculo, "registrador", linha, coluna);
        return 1;
    } else {
        RegistrarErro("ERRO_REGISTRADOR_INVALIDO", lexema, linha, coluna);
        return 0;
    }
}

static int ContinuarNumero(FILE * in, char lexema[100], int idx, int linha, int coluna, Token *tOut)
{
    int primeiroDigito = lexema[idx - 1];
    int c;
    int ehHex = 0;

    if (primeiroDigito == '0') {
        c = LerChar(in);
        if (c == 'x' || c == 'X') {
            if (idx < 98) lexema[idx++] = (char)c;
            ehHex = 1;
        } else {
            DevolverChar(c);
        }
    }

    if (ehHex) {
        int qtdDigitosHex = 0;
        while (isxdigit((unsigned char)(c = LerChar(in)))) {
            if (idx < 98) lexema[idx++] = (char)c;
            qtdDigitosHex++;
        }
        if (isalnum((unsigned char)c) || c == '_') {
            // algo tipo "G" em "0xG1" grudado no número -> malformado
            while ((isalnum((unsigned char)c) || c == '_') && idx < 98) {
                lexema[idx++] = (char)c;
                c = LerChar(in);
            }
            DevolverChar(c);
            lexema[idx] = '\0';
            RegistrarErro("ERRO_NUMERO_MALFORMADO", lexema, linha, coluna);
            return 0;
        }
        DevolverChar(c);
        if (qtdDigitosHex == 0) {
            // só 0x sem nenhum hexa depois (igual o Brasil)
            lexema[idx] = '\0';
            RegistrarErro("ERRO_NUMERO_MALFORMADO", lexema, linha, coluna);
            return 0;
        }
    } else {
        while (isdigit((unsigned char)(c = LerChar(in)))) {
            if (idx < 98) lexema[idx++] = (char)c;
        }
        if (isalpha((unsigned char)c) || c == '_') {
            // dígitos decimais colados a letras, ex.: 2valor
            while ((isalnum((unsigned char)c) || c == '_') && idx < 98) {
                lexema[idx++] = (char)c;
                c = LerChar(in);
            }
            DevolverChar(c);
            lexema[idx] = '\0';
            RegistrarErro("ERRO_IDENTIFICADOR_MALFORMADO", lexema, linha, coluna);
            return 0;
        }
        DevolverChar(c);
    }

    lexema[idx] = '\0';
    strcpy(tOut->nome, "NUM_INT");
    strcpy(tOut->lexema, lexema);
    tOut->linha = linha;
    tOut->coluna = coluna;
    return 1;
}

static int LerNumero(FILE * in, int primeiroDigito, int linha, int coluna, Token *tOut)
{
    char lexema[100];
    lexema[0] = (char)primeiroDigito;
    return ContinuarNumero(in, lexema, 1, linha, coluna, tOut);
}

// Chamada quando o dispatcher já consumiu um '-'
static int LerNumeroComSinal(FILE * in, int linha, int coluna, Token *tOut)
{
    int prox = LerChar(in);
    if (!isdigit((unsigned char)prox)) {
        DevolverChar(prox);
        RegistrarErro("ERRO_CARACTERE_INVALIDO", "-", linha, coluna);
        return 0;
    }

    char lexema[100];
    lexema[0] = '-';
    lexema[1] = (char)prox;
    return ContinuarNumero(in, lexema, 2, linha, coluna, tOut);
}

static int LerString(FILE * in, int linha, int coluna, Token *tOut)
{
    char lexema[512];
    int idx = 0;
    lexema[idx++] = '"';

    int c;
    while (1) {
        c = LerChar(in);

        if (c == '"') {
            lexema[idx++] = '"';
            break;
        }
        if (c == '\n' || c == EOF) {
            // string não fechada antes da quebra de linha OU do fim do arquivo
            lexema[idx] = '\0';
            if (c == '\n') {
                RegistrarErro("ERRO_STRING_NAO_FECHADA", lexema, linha, coluna);
            } else {
                RegistrarErro("ERRO_STRING_NAO_FECHADA", lexema, linha, coluna);
            }
            return 0;
        }
        if (c == '\\') {
            int esc = LerChar(in);
            if (esc == 'n' || esc == 't' || esc == '"' || esc == '\\' || esc == '0') {
                if (idx < 508) { lexema[idx++] = '\\'; lexema[idx++] = (char)esc; }
                continue;
            }
            // sequência de escape inválida: registra o erro mas continua tentando fechar a string normalmente
            char seqInvalida[4];
            seqInvalida[0] = '\\';
            seqInvalida[1] = (esc == EOF) ? '?' : (char)esc;
            seqInvalida[2] = '\0';
            RegistrarErro("ERRO_SEQUENCIA_ESCAPE_INVALIDA", seqInvalida, g_linha, g_coluna - 1);

            if (esc == EOF) {
                lexema[idx] = '\0';
                RegistrarErro("ERRO_STRING_NAO_FECHADA", lexema, linha, coluna);
                return 0;
            }
            if (esc == '\n') {
                lexema[idx] = '\0';
                RegistrarErro("ERRO_STRING_NAO_FECHADA", lexema, linha, coluna);
                return 0;
            }
            if (idx < 508) { lexema[idx++] = '\\'; lexema[idx++] = (char)esc; }
            continue;
        }

        if (idx < 510) lexema[idx++] = (char)c;
    }
    lexema[idx] = '\0';

    strcpy(tOut->nome, "STRING");
    strncpy(tOut->lexema, lexema, sizeof(tOut->lexema) - 1);
    tOut->lexema[sizeof(tOut->lexema) - 1] = '\0';
    tOut->linha = linha;
    tOut->coluna = coluna;

    return 1;
}

static void MontarSimbolo(Token *tOut, const char * nome, const char * lexema, int linha, int coluna)
{
    strcpy(tOut->nome, nome);
    strcpy(tOut->lexema, lexema);
    tOut->linha = linha;
    tOut->coluna = coluna;
}

// Esta função é o estado inicial do autômato
static Token ProximoToken(FILE * in)
{
    Token t;

    while (1) {
        PularEspacosComentarios(in);

        int linha = g_linha;
        int coluna = g_coluna;
        int c = LerChar(in);

        if (c == EOF) {
            strcpy(t.nome, "TK_EOF");
            strcpy(t.lexema, "EOF");
            t.linha = linha;
            t.coluna = coluna;
            return t;
        }

        int sucesso;

        if (isalpha((unsigned char)c) || c == '_') {
            sucesso = LerIdentificadorOuInstrucao(in, c, linha, coluna, &t);
        } else if (c == '.') {
            sucesso = LerDiretiva(in, linha, coluna, &t);
        } else if (c == '$') {
            sucesso = LerRegistrador(in, linha, coluna, &t);
        } else if (isdigit((unsigned char)c)) {
            sucesso = LerNumero(in, c, linha, coluna, &t);
        } else if (c == '-') {
            sucesso = LerNumeroComSinal(in, linha, coluna, &t);
        } else if (c == '"') {
            sucesso = LerString(in, linha, coluna, &t);
        } else if (c == ',') {
            MontarSimbolo(&t, "SMB_COM", ",", linha, coluna);
            sucesso = 1;
        } else if (c == ':') {
            MontarSimbolo(&t, "SMB_COL", ":", linha, coluna);
            sucesso = 1;
        } else if (c == '(') {
            MontarSimbolo(&t, "SMB_OPA", "(", linha, coluna);
            sucesso = 1;
        } else if (c == ')') {
            MontarSimbolo(&t, "SMB_CPA", ")", linha, coluna);
            sucesso = 1;
        } else {
            char bufferChar[2] = { (char)c, '\0' };
            RegistrarErro("ERRO_CARACTERE_INVALIDO", bufferChar, linha, coluna);
            sucesso = 0;
        }

        if (sucesso) return t;
        // Senão, volta ao topo do while e tenta reconhecer o próximo token
    }
}

static char arquivoTS[256] = "saida.ts";
static char arquivoERR[256] = "saida.err";

void ConfigurarArquivosAuxiliares(const char * nomeArquivoLex)
{
    strncpy(arquivoTS, nomeArquivoLex, sizeof(arquivoTS) - 4);
    arquivoTS[sizeof(arquivoTS) - 4] = '\0';
    strncpy(arquivoERR, nomeArquivoLex, sizeof(arquivoERR) - 5);
    arquivoERR[sizeof(arquivoERR) - 5] = '\0';

    // troca a extensão (o que vier depois do último '.') por .ts / .err; se não houver '.' no nome, só concatena no final
    char * ponto;

    ponto = strrchr(arquivoTS, '.');
    if (ponto != NULL) *ponto = '\0';
    strcat(arquivoTS, ".ts");

    ponto = strrchr(arquivoERR, '.');
    if (ponto != NULL) *ponto = '\0';
    strcat(arquivoERR, ".err");
}

static void GravarTabelaSimbolos(void)
{
    FILE * ts = fopen(arquivoTS, "w");
    if (ts == NULL) {
        perror("Erro ao criar arquivo .ts");
        return;
    }

    fprintf(ts, "LEXEMA,CATEGORIA,LINHA,COLUNA\n");
    for (int i = 0; i < totalSimbolos; i++) {
        fprintf(ts, "%s,%s,%d,%d\n",
                tabelaSimbolos[i].lexema,
                tabelaSimbolos[i].categoria,
                tabelaSimbolos[i].primeiraLinha,
                tabelaSimbolos[i].primeiraColuna);
    }
    fclose(ts);
}

static void GravarErros(void)
{
    FILE * err = fopen(arquivoERR, "w");
    if (err == NULL) {
        perror("Erro ao criar arquivo .err");
        return;
    }

    if (totalErros == 0) {
        fprintf(err, "Nenhum erro léxico encontrado.\n");
    } else {
        for (int i = 0; i < totalErros; i++) {
            fprintf(err, "<%s, %s> %d %d\n",
                    tabelaErros[i].tipo,
                    tabelaErros[i].lexema,
                    tabelaErros[i].linha,
                    tabelaErros[i].coluna);
        }
    }
    fclose(err);
}

void AnaliseLexica(FILE * in, FILE * out)
{
    // Reinicia todo estado global do módulo a cada chamada, pra ser seguro
    g_linha = 1;
    g_coluna = 1;
    totalErros = 0;
    InicializarTabelaSimbolos();

    Token t;
    do {
        t = ProximoToken(in);
        fprintf(out, "<%s, %s> %d %d\n", t.nome, t.lexema, t.linha, t.coluna);
    } while (strcmp(t.nome, "TK_EOF") != 0);

    GravarTabelaSimbolos();
    GravarErros();
}

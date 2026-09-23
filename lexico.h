#ifndef LEXICO_H
#define LEXICO_H

#include <stdio.h>

// Estrutura de um Token
typedef struct {
    char nome[50];      // nome da categoria do token, ex: "INS_LI"
    char lexema[100];   // sequência de caracteres encontrada no programa
    int linha;          // linha em que o lexema começa (1-based)
    int coluna;         // coluna em que o lexema começa (1-based)
} Token;

// Estrutura de uma entrada da tabela de símbolos
typedef struct {
    char lexema[100];
    char categoria[30];
    int primeiraLinha;
    int primeiraColuna;
} Simbolo;

void ConfigurarArquivosAuxiliares(const char *nomeArquivoLex);
void AnaliseLexica(FILE *in, FILE *out);

#endif

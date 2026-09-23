#include <stdio.h>
#include "preprocessador.h"
#include "lexico.h"

int main (int argc, char ** argv)
{
    // Checando se o usuário inseriu os parâmetros adequadamente
    if (argc != 4)
    {
        printf("Numero incorreto de argumentos\n"
               "Uso: main.exe <arquivo_entrada.asm> <arquivo_saida.pre> <arquivo_saida.lex>\n");
        return 1;
    }

    // Arquivos de entrada e saída
    char * in = argv[1];
    char * outPre = argv[2];
    char * outLex = argv[3];

    // Checando se o arquivo de entrada existe
    FILE * arquivo = fopen(in, "r");
    if (arquivo == NULL)
    {
        printf("O arquivo %s nao foi encontrado.\n", in);
        return 1;
    }
    fclose(arquivo);

    PreProcessamento(in, outPre);

    FILE * entradaLexico = fopen(outPre, "r");
    if (entradaLexico == NULL)
    {
        printf("Naum foi possihvel abrir o arquivo preh-processado %s.\n", outPre);
        return 1;
    }

    FILE * saidaLexico = fopen(outLex, "w");
    if (saidaLexico == NULL)
    {
        printf("Nao foi possivel criar o arquivo de saida %s.\n", outLex);
        fclose(entradaLexico);
        return 1;
    }

    ConfigurarArquivosAuxiliares(outLex);
    AnaliseLexica(entradaLexico, saidaLexico);

    fclose(entradaLexico);
    fclose(saidaLexico);

    return 0;
}
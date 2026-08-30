#include <stdio.h>
#include "preprocessador.h"

int main (int argc, char ** argv)
{
    // Checando se o usuário inseriu os parâmetros adequadamente
    if (argc != 3)
    {
        printf("Numero incorreto de argumentos\n"
               "Uso: main.exe <arquivo_entrada.asm> <arquivo_saida.pre>\n");
        return 1;
    }

    // Arquivos de entrada e saída
    char * in = argv[1];
    char * out = argv[2];

    // Checando se o arquivo de entrada existe
    FILE * arquivo = fopen(in, "r");
    if (arquivo == NULL)
    {
        printf("O arquivo %s nao foi encontrado.\n", in);
        return 1;
    }
    fclose(arquivo);

    Preprocessar(in, out);

    return 0;
}
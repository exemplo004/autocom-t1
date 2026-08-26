#include <stdio.h>
#include "preprocessador.h"

int main (int argc, char ** argv)
{
    // Checando se o usuário inseriu os parâmetros adequadamente
    if (argc < 2 || argc > 3)
    {
        printf("Nuhmero incorreto de argumentos\n"
            "Usage: prep.exe entrada [saihda]\n"
            "\tentrada: o nome do arquivo a ser compilado\n"
            "\t[saihda]: o nome do arquivo onde o resultado serah armazenado\n");
        return 1;
    }

    // Arquivos de entrada e saída
    char * in = argv[1];
    char * out = NULL;

    // Checando se o nome de arquivo no parâmetro 1 existe
    if (!fopen(in, "r"))
    {
        printf("%s naum eh um arquivo vahlido ou sua localizassaum estah incorreta.\n", argv[1]);
        return 1;
    }
    else // Carrega arquivo de saída de acordo com a entrada do usuário
    {
        out = argc == 3 ? argv[2] : "saida.txt";
    }

    Preprocessar(in, out);

    return 0;
}
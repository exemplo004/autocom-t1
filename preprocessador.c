#include <stdio.h>
#include <stdlib.h>
#include "preprocessador.h"

void RemoverComentarios(FILE * fp)
{
    // TODO
}

void RemoverLinhasVazias(char * fn)
{
    FILE * ref = fopen(fn, "r");
    FILE * buf = fopen(ARQUIVO_TEMPORARIO, "w");
    
    if (ref == NULL || buf == NULL) {
        perror("Erro ao abrir arquivo");
        return;
    }
    
    int prev = '\n';
    int cur = getc(ref);
    int i = 0;

    // Loop até o fim do arquivo de referência
    while (cur != EOF)
    {
        // Evita escrever quando percebe dois '\n' seguidos
        if (prev != '\n' || cur != '\n') { putc(cur, buf); }

        prev = cur;
        cur = getc(ref); i++;
    }

    FecharESubstituir(ref, buf);
}

void RemoverTabEspacos(char * fn)
{
    FILE * ref = fopen(fn, "r");
    FILE * buf = fopen("buffer", "w");

    char prev = '\n';
    char cur = getc(ref);
    int i = 0;

    // Loop até o fim do arquivo de referência
    while (cur != EOF)
    {
        // TODO: impedir esse processo quando dentro de uma string (dentro das aspas da .asciiz) [dá pra usar fseek()]
        if (prev != ' ' && prev != '\n' || cur != ' ') // A'C'+B'
                                                       // MAPA DE KARNAUGH PRA QUEM LEMBRA KAKAKAKA
        {
            putc(cur, buf);
        }

        prev = cur;
        cur = getc(ref); i++;
    }

    // Removendo a referência antiga e substituindo por uma nova
    // TODO: transformar essa parte em uma função, já que teremos que usar no fim de todo processo
    fclose(ref); fclose(buf);
    remove("ref");
    rename("buffer", "ref");
}

void RemoverQuebraDeLinha(FILE * fp)
{
    // TODO
}

void Preprocessar(char * inp, char * saida)
{
    char cmd[256];
    sprintf(cmd, "cp %s ref", inp); // Só funciona no Linux! Acho que o Windows tem sua própria versão ~kristhian
    system(cmd);
    // RemoverComentarios(outp);
    RemoverLinhasVazias(ARQUIVO_REFERENCIA);
    RemoverTabEspacos(ARQUIVO_REFERENCIA);
    // RemoverQuebraDeLinha(outp);
}

// ---- daqui pra frente, acho que essas funções seriam úteis para os trabalhos seguintes, na conclusão efetiva do compilador
void ReservarDiretivas()
{
    
}

void ReservarStrings()
{

}
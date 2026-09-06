#include <stdio.h>
#include <stdlib.h>
#include "preprocessador.h"
#include <string.h>

void FecharESubstituir(FILE * old, FILE * new)
{
    fclose(old); fclose(new);
    remove(ARQUIVO_REFERENCIA);
    rename(ARQUIVO_TEMPORARIO, ARQUIVO_REFERENCIA);
}



// Troca os '\r' do Windows por '\n'
void normalizarQuebraDeLinha(char * fn)
{
    FILE * ref = fopen(fn, "r");
    FILE * buf = fopen(ARQUIVO_TEMPORARIO, "w");

    if (ref == NULL || buf == NULL) {
        perror("Erro ao abrir arquivo");
        return;
    }

    int cur = getc(ref);

    // Loop até o fim do arquivo de referência
    while (cur != EOF)
    {
        // Se for o '\r' do windows ignora. O '\n' que vem depois já é serve pra quebrar a linha
        if (cur != '\r') { 
            putc(cur, buf); 
        }

        cur = getc(ref);
    }

    FecharESubstituir(ref, buf);
}

void RemoverComentarios(char * fn)
{
    FILE * ref = fopen(fn, "r");
    FILE * buf = fopen(ARQUIVO_TEMPORARIO, "w");

    if (ref == NULL || buf == NULL) return;

    int cur = getc(ref);
    int dentro_de_string = 0;
    int dentro_de_comentario = 0;

    while (cur != EOF)
    {
        // liga ou desliga flag se achar aspas duplas
        if (cur == '"' && dentro_de_comentario == 0) {
            dentro_de_string = !dentro_de_string;
        }

        // se for # e não tiver na string, começou um comentario
        if (cur == '#' && dentro_de_string == 0) {
            dentro_de_comentario = 1;
        }

        if (dentro_de_comentario == 1) {
            // se a linha acabar, reseta o comentario
            if (cur == '\n') {
                dentro_de_comentario = 0;
                putc(cur, buf); // salva o enter pra não ficar tudo em uma linha só
            }
        } else {
            // salva a letra normal
            putc(cur, buf);
        }

        cur = getc(ref);
    }

    FecharESubstituir(ref, buf);
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
    int inString = 0; // Bool para verificar se o cursor está dentro de uma string

    // Loop até o fim do arquivo de referência
    while (cur != EOF)
    {
        // se for um tab fora da string, transforma ele num espaço normal
        if (cur == '\t' && inString == 0) {
            cur = ' ';
        }

        // Se achar aspas duplas, inverte a flag
        if (cur == '"') {
            inString = !inString;
        }
        
        // Somente discrimina espaços se não estiver dentro de string
        if (!inString)
        {
            if (prev != ' ' && prev != '\n' || cur != ' ') // A'C'+B' MAPA DE KARNAUGH PRA QUEM LEMBRA KAKAKAKA
            {
                putc(cur, buf);
            }
        }
        // Como está em uma string, sobrescreve sem discriminação
        else { putc(cur, buf); }
        
        prev = cur;
        cur = getc(ref);
    }

    FecharESubstituir(ref, buf);
}

void Preprocessar(char * inp, char * saida)
{
    char cmd[256];
    sprintf(cmd, "copy %s ref", inp); // Alterado para 'copy' pra funcionar no Windows
    system(cmd);
    normalizarQuebraDeLinha(ARQUIVO_REFERENCIA);
    RemoverComentarios(ARQUIVO_REFERENCIA);
    RemoverLinhasVazias(ARQUIVO_REFERENCIA);
    RemoverTabEspacos(ARQUIVO_REFERENCIA);

    // copia o arquivo temporário 'ref' para o nome final que o usuário pediu
    sprintf(cmd, "copy ref %s", saida);
    system(cmd);
    remove("ref");
}

// ---- daqui pra frente, acho que essas funções seriam úteis para os trabalhos seguintes, na conclusão efetiva do compilador
void ReservarDiretivas()
{
    
}

void ReservarStrings()
{

}
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

// Caminha todos os caracteres de uma string para a esquerda, adicionando um novo caractere ao final da string, antes do '\n'
// Será utilizada para verificar se o cursor está dentro de uma string
void caminhar(char * s, char novo)
{
    for (int i = 0; i < strlen(s) - 1; i++)
    {
        s[i] = s[i + 1];
    }

    s[strlen(s) -1] = novo;
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

    // Os últimos 9 caracteres do arquivo de referência serão analisados para definir se estamos numa string ou não
    char last9[10] = "123456789";
    char prev = last9[7];
    char cur = getc(ref);
    int i = 0;

    // Bool para ferificar se o cursor se encontra dentro de uma string ou não
    int inString = 0;

    // Loop até o fim do arquivo de referência
    while (cur != EOF)
    {
        // Caminha os últimos 9 caracteres e verifica se são ".asciiz \""
        caminhar(last9, cur);
        if (!strcmp(last9, ".asciiz \"")) inString = 1;
        
        // Somente discrimina espaços e tabs se não estiver dentro de string
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
        cur = getc(ref); i++;

        // No caso de estar dentro de uma string, verifica se o próximo caractere é um '"', fechando saindo assim de dentro da string
        if (inString) { inString = (cur == '"')? 0 : 1; }
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
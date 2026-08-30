#ifndef PREPROCESSADOR_H
#define PREPROCESSADOR_H

#define ARQUIVO_REFERENCIA "ref"
#define ARQUIVO_TEMPORARIO "buffer"

void Preprocessar(char * entrada, char * saida);
void RemoverLinhasVazias(char * referencia);
void RemoverTabEspacos(char * referencia);
#endif
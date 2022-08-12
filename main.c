#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//importando imagelib.h
#include "imagelib.h"
//importando string.h
#include <stdio.h>

#define TAM_DICT 4096
#define NUM_SIMB 256

typedef unsigned short int ui16;

typedef struct alocaInfo
{
    ui16 *base;
    int tam;
    ui16 *proxAloca;
} alocaInfo;

void iniciaAloca(alocaInfo *aloc, int tam)
{
    aloc->base = malloc(tam);
    aloc->tam = tam;
    aloc->proxAloca = aloc->base;
}

ui16 *aloca(alocaInfo *aloc, int len)
{
    ui16 *ret = aloc->proxAloca;
    aloc->proxAloca += len;
    return ret;
}

void decodifica(ui16 *in, int n)
{
    struct
    {
        ui16 *seq;
        int tam;
    } dicionario[TAM_DICT];

    alocaInfo aInfo;
    ui16 *marca;
    int posDict;
    int anterior;
    int i;
    iniciaAloca(&aInfo, TAM_DICT * TAM_DICT * sizeof(ui16));

    marca = aInfo.proxAloca;
    for (i = 0; i < NUM_SIMB; i++)
    {
        dicionario[i].seq = aloca(&aInfo, 1);
        dicionario[i].seq[0] = i;
        dicionario[i].tam = 1;
    }
    posDict = NUM_SIMB;
    anterior = in[0];
    printf("[%d]-", anterior);
    i = 1;
    while (i < n)
    {
        int simbolo = in[i++];
        if (posDict == TAM_DICT)
        {
            aInfo.proxAloca = marca;
            for (int i = 0; i < NUM_SIMB; i++)
            {
                dicionario[i].seq = aloca(&aInfo, 1);
                dicionario[i].seq[0] = i;
                dicionario[i].tam = 1;
            }
            posDict = NUM_SIMB;
        }
        else
        {
            int tam = dicionario[anterior].tam;
            dicionario[posDict].tam = tam + 1;
            dicionario[posDict].seq = aloca(&aInfo, tam + 1);
            for (int k = 0; k < tam; k++)
            {
                dicionario[posDict].seq[k] = dicionario[anterior].seq[k];
            }
            if (simbolo == posDict)
            {
                dicionario[posDict++].seq[tam] = dicionario[anterior].seq[0];
            }
            else
            {
                dicionario[posDict++].seq[tam] = dicionario[simbolo].seq[0];
            }
        }
        for (int k = 0; k < dicionario[anterior].tam; k++)
        {
            printf("[%d]-", dicionario[simbolo].seq[k]);
        }
        anterior = simbolo;
    }
    free(aInfo.base);
    puts("");
}

void codifica(int *in, int n)
{
    ui16 dicionario[TAM_DICT][NUM_SIMB];
    int posDict;
    int simbolo;
    int corrente = in[0];

    memset(dicionario, 0, sizeof(dicionario));
    posDict = NUM_SIMB;

    int i = 1;

    while (i < n)
    {
        simbolo = in[i++];
        ui16 prox = dicionario[corrente][simbolo];
        if (prox != 0)
            corrente = prox;
        else
        {
            printf("[%d]", corrente);
            if (posDict < TAM_DICT)
                dicionario[corrente][simbolo] = posDict++;
            else
            {
                memset(dicionario, 0, sizeof(dicionario));
                posDict = NUM_SIMB;
            }
            corrente = simbolo;
        }
    }
    printf("[%d]\n", corrente);
}
//my functions
void readFile(char *header, char *filename, int *nL, int *nC, char *dataImage)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo %s\n", filename);
        exit(1);
    }
    fread(header, 1, sizeof(header), file);
    
    

    
    fclose(file);
}


int main(int argc, char *argv[])
{
    //atividade////
    char *header = malloc(7);   //header do arquivo
    char *filename = argv[1];   //argv[1] é o nome do arquivo passado por linha de comando
    int *nL,*nC;                //nL é o numero de linhas e nC é o numero de colunas
    char *dataImage;            //dataImage é o vetor que armazena a imagem compactada em base 64

    readFile(header,filename,nL,nC,dataImage);

    //exibe informações do arquivo
    printf("\n\n\tHeader: %s", header);
    printf("\tNumero de linhas: %d\n", *nL);
    printf("\tNumero de colunas: %d\n", *nC);

    

    ///////////////
    /*
    ui16 in[10] = {4096, 39, 126, 126, 256, 258, 260, 259, 257, 126};
    int in2[16] = {39, 39, 126, 126, 39, 39, 126, 126, 39, 39, 126, 126, 39, 39, 126, 126};
    codifica(in2, 16);
    decodifica(in, 10);
    */
    
    return 0;
}
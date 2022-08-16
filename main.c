#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//importando imagelib.h
#include "imagelib.h"

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
static char tabelaConversao[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};

int base2Int(char c)
{
    for (int i = 0; i < 64; i++)    //Percorre a tabela
    {
        if (tabelaConversao[i] == c)
            return i;                   //Retorna o indice da tabela de conversao
    }
    return -1;
}

int base2bin( char p1, char p2){
    p1 = base2Int(p1);  //passa o primeiro caracter para inteiro
    p2 = base2Int(p2);  //passa o segundo caracter para inteiro
    p1 = (p1 << 6) | p2; //concatena p1 ,deslocado 6 bits para a esquerda, com p2    
    return p1;
}

void readFile(char *header, char *filename, int *nL, int *nC, int *dataImage)
{
    puts("Reading file...");
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo %s\n", filename);
        exit(1);
    }

    //ler o cabecalho
    fread(header, 1, sizeof(header), file);
    //ler o numero de linhas e colunas com fscan
    fscanf(file, "%d %d", nL, nC);

    //ler o conteudo da imagem
    dataImage = malloc(*nL * *nC * sizeof(int));
    
    char *auxLine;      
    //auxLine = malloc(sizeof(char)*(*nL)*(*nC));
    auxLine = calloc(sizeof(char), (*nL)*(*nC));
    dataImage = calloc(sizeof(int), (*nL)*(*nC));

    int qntdPixels = 0;
    for (int i = 0; (i+2) <= 10;i+=2) //*nL * *nC; i+=2)
    {
        fscanf(file, "%c", &auxLine[i]);
        dataImage[i] = base2bin(auxLine[i],auxLine[i+1]);
        qntdPixels++;
        printf("%d, ", dataImage[i]);     
    }
    printf("\n");
    printf("PIXELS %d\n\t\t", qntdPixels);
    fclose(file);
}

void printFile(char *header, char *filename, int *nL, int *nC, int *dataImage){
    char *fileName = malloc(sizeof(char)*(strlen(filename)+strlen(".out")));
    FILE *file = fopen("Resultado", "wt");
    if (file == NULL)
    {
        printf("Erro ao salvar o arquivo %s\n", filename);
        exit(1);
    }
    fwrite(header, 1, sizeof(header), file);
    fprintf(file, "%d %d\n", *nL, *nC);
    
    for (int i = 0; i < *nL ; i++)
    {
        for (int j = 0; j < *nC; j++)
        {
            fprintf(file, "%d", dataImage[i*(*nC)+j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}


int main(int argc, char *argv[])
{
    //atividade////
    char *header = malloc(7);   //header do arquivo
    char *filename = argv[1];   //argv[1] é o nome do arquivo passado por linha de comando
    int *nL = malloc(1);        //numero de linhas      
    int *nC = malloc(1);        //numero de colunas
    int *dataImage;             //dataImage é o vetor que armazena a imagem compactada em base 64

    readFile(header,filename,nL,nC,dataImage);

    //printFile(header,filename,nL,nC,dataImage);   //para Testes

    //exibe informações do arquivo
    printf("\n\n\t\tHeader: %s", header);
    printf("\t\tNumero de linhas: %d\n", *nL);
    printf("\t\tNumero de colunas: %d\n", *nC);

    printf("\t\tCabacalo: %d\n", *header);

    //Descodifica a imagem
   


    /////////////// exemplo de uso decodificador ///////////////
    /*
    ui16 in[10] = {4096, 39, 126, 126, 256, 258, 260, 259, 257, 126};
    int in2[16] = {39, 39, 126, 126, 39, 39, 126, 126, 39, 39, 126, 126, 39, 39, 126, 126};
    codifica(in2, 16);
    decodifica(in, 10);
    */

   //testess
    /*
    */
    return 0;
}
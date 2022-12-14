/*-------------------------------------------------
  | Unifal - Universidade Federal de Alfenas.
  | BACHARELADO EM CIENCIA DA COMPUTACAO.
  | Trabalho..: Descompactador do formato LZW Base64.
  | Disciplina: Processamento de Imagens
  | Professor.: Luiz Eduardo da Silva
  | Aluno.....: João Vitor Fonseca
  | Data......: 17/08/2022
  -------------------------------------------------*/


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

void decodifica(ui16 *in, int n,int nL, int nC,char *name)
{
    //remove a extensao do nome do arquivo
    char *ext = ".l64";
    name = strtok(name, ext);
    //concatena a extensao do arquivo com o nome do arquivo
    strcat(name, ".pgm");    

    FILE * arq;
    arq = fopen(name,"w");
    fprintf(arq,"P2\n");
    fprintf(arq,"%d %d\n",nC,nL);
    fprintf(arq,"%d\n",255);

    int auxCount = 0;


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
    //printf("[%d]-", anterior);
    fprintf(arq,"%d ",anterior);

    auxCount++;
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
            //printf("[%d]-", dicionario[simbolo].seq[k]);
            fprintf(arq,"%d ",dicionario[simbolo].seq[k]);            
            auxCount++;
            if(auxCount>=nC){
                fprintf(arq,"\n");
                auxCount = 0;
            }
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
    printf("Erro ao converter caracter %c\n", c);
    return 0;
}

int base2bin( char p1, char p2){
    int mask = 00111111;
    int x = base2Int(p1);  //passa o primeiro caracter para inteiro
    int y = base2Int(p2);  //passa o segundo caracter para inteiro

    // p1 = p1 & mask;    //faz o AND com a mascara
    // p2 = p2 & mask;    //faz o AND com a mascara

     //concatena p1 ,deslocado 6 bits para a esquerda, com p2   e aplica mascara de 12 bits

    x = x << 6;
    return (x|y);

    //return (((p1 << 6) | p2)&(0000111111111111));
}

void readFile(char *header, char *filename, int *nL, int *nC,ui16 *in, char * img_name)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo %s\n", filename);
        exit(1);
    }

    //ler o cabecalho
    fscanf(file, "%s", header);

    //ler o numero de linhas e colunas com fscan
    fscanf(file, "%d %d", nL, nC);

    //ignora quebra de linha

    //ler o conteudo da imagem
    char aux1;
    char aux2;      
    
    in = malloc(*nL * *nC * sizeof(ui16));
    
    int qntdPixels = 0; 
    int x = 0;
    //enquanto
    while(!feof(file))
    {
        fscanf(file, "%c", &aux1);
        if(aux1 == '\n'){
            continue;
        }

        fscanf(file, "%c", &aux2);
        if(aux2 == '\n'){
            continue;
        }

        in[x] = base2bin(aux1,aux2);
        //teste
        //printf("Letras: [%c, %c] \t base2int (%d,%d)\t ----> \tbase2Bin %d\n ",aux1,aux2,base2Int(aux1),base2Int(aux2),in[x]);
        x+=1;
        qntdPixels++;
    }

    //Prepara nomedo arquivo de saida
    

    decodifica(in,qntdPixels,*nL,*nC,img_name);
    
    //printf("\n\t\tPIXELS %d\n\t\t", qntdPixels);
    fclose(file);
}




int main(int argc, char *argv[])
{
    //atividade////
    char *header = malloc(7);   //header do arquivo
    char *filename = argv[1];   //argv[1] é o nome do arquivo passado por linha de comando
    int *nL = malloc(1);        //numero de linhas      
    int *nC = malloc(1);        //numero de colunas

    ui16 *in;
    in = malloc(1);

    //tratar name
    int iAux1 = 0;
    while(filename[iAux1] != '\0')
    {
        iAux1++;
    }
    if(!(filename[iAux1-4] == '.' && filename[iAux1-3] == 'l' && filename[iAux1-2] == '6' && filename[iAux1-1] == '4')){
        filename[iAux1] = '.';                       // :)
        filename[iAux1+1] = 'l';
        filename[iAux1+2] = '6';
        filename[iAux1+3] = '4';
        filename[iAux1+4] = '\0';
    }
    readFile(header,filename,nL,nC,in,filename);   
    //exibe informações do arquivo
    printf("\t\tHeader: %s", header);
    printf("\n\t\tNumero de linhas: %d\n", *nL);
    printf("\t\tNumero de colunas: %d\n", *nC);
    //Descodifica a imagem
   /*mostra a variável com dados da ima
   for(int i =0;i<10;i++){
    printf("%d, ", dataImage[i]);
   
   printf("\n\n\t\t TESTE \n");
   for(int i =0;i<100;i++){
    printf("%d, ", in[i]);
    }}*/


    /////////////// exemplo de uso decodificador ///////////////
    /*
    ui16 in4[10] = {126, 39, 126, 126, 256, 258, 260, 259, 257, 126};
    int in2[16] = {39, 39, 126, 126, 39, 39, 126, 126, 39, 39, 126, 126, 39, 39, 126, 126};
    //codifica(in2, 16);
    decodifica(in4, 10);
    */

   //testess
    /*
   for (int i = 0; i < 100; i++)
   {
    printf("%d, ", in[i]);
    }
    */
   
    return 0;
}
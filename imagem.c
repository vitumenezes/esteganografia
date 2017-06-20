#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "imagem.h"

Imagem leitura(char *nomeArquivo){

  FILE *arq;
  Imagem img;
  unsigned char file_tipo[2];

  arq = fopen(nomeArquivo, "r");

  if(!arq)
    printf("Não foi possivel abrir o arquivo %s  \n", nomeArquivo);
  else{
    fscanf(arq, "%2s", file_tipo);
    file_tipo[2] = '\0';

    if(strcmp(file_tipo, "P6") == 0){
      
      printf("============= Imagem PPM P6 ============\n");
      Imagem img; //declaracao da struct de retorno
      strcpy(img.tipo, file_tipo);

      //ignora comentários existentes no arquivo imagem
      int comment = getc(arq);
      while(comment == '#'){
         while((comment = getc(arq)) != '\n');
      }

      //leitura das informacoes do cabecalho
      fscanf(arq, "%d %d %d", &img.width, &img.height, &img.RGB); 

      //verifica se a densidade realmente eh 255
      if(img.RGB != 255){
        printf("ERRO: a densidade do RGB deve ser 255. Sua imagem tem: %d\n", img.RGB);
        exit(1);
      }

      //alocacao dos pixels
      img.pixel = malloc(sizeof(unsigned char)*(3 * img.width * img.height));

      //quebra linha para acessar os pixels
      while (fgetc(arq) != '\n');

      //leitura e armazenamento no vetor PIXEL
      fread(img.pixel, sizeof(unsigned char), 3 * img.width * img.height, arq);
      
      printf("Leitura da imagem PPM feita com sucesso!\n");
          
    } else if(strcmp(file_tipo, "BM") == 0){

      printf("============== Imagem BMP =============\n");
      int i, j, counter = 0; //variavel counter serve para ser incrementada a cada laço do for, assim o indice do vetor pixels sempre sera um novo
      int offset; //variável para armazenar o offset
      fseek(arq, 10, SEEK_SET); //fseek para o offset
      fread(&offset, sizeof(int), 1, arq); //armazena offset na variavel local
      fseek(arq, 18, SEEK_SET); //fseek para largura da imagem
      fread(&img.width, sizeof(int), 1, arq); //armazena largura
      fread(&img.height, sizeof(int), 1, arq); //armazena altura
      fseek(arq, offset, SEEK_SET); //fseek para pixels da imagem

      img.pixel = malloc(3 * img.width * img.height); //alocacao dos pixels

      if((img.width*3) %4 == 0){

        //leitura e armazenamento no vetor PIXEL
        fread(img.pixel, sizeof(unsigned char), (3 * img.width * img.height), arq);
        printf("Leitura da imagem BMP feita com sucesso! offset=%d\n", offset);
      
      } else {

        //numero de bytes 0 adicionados em cada linha
        int num_zeros = 4-((img.width*3)%4);

        //laço para percorrer cada linha e cada elemento de cada coluna da linha
        for(i = 1; i <= img.height; i++){
          for(j = 1; j <= (img.width*3); j++){
            fread(&img.pixel[counter], sizeof(unsigned char), 1, arq);
            counter++; //incrementacao apos armazenamento no vetor pixels
          }
          fseek(arq, num_zeros, SEEK_CUR); //pula os bytes 0 acrescentados para deixar a linha multipla de 4
        }

        //printf("IMAGEM NÃO MULTIPLA ===== %d %d %d\n", num_zeros, img.width, img.height);
        printf("Leitura da imagem BMP feita com sucesso!\n");

      }
    }
  }

  fclose(arq);
  return img;
}

void gravar(char *nomeArquivo, Imagem img){

  FILE *arq;

  arq = fopen(nomeArquivo, "w");

  fwrite(img.pixel, sizeof(unsigned char), 3*img.width*img.height, arq);

}
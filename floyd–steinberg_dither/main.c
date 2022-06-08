#include "imagem.h"
#include "cores.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>

typedef struct pallete {
    Cor* array;
    int size;
} Pallete;

Pallete* create_kmeans_pallete(char* nome_input){
    Pallete* pallete = (Pallete*)malloc(sizeof(Pallete));
    pallete->array = (Cor*)malloc(sizeof(Cor) * 32);
    pallete->size = 32;

    FILE* input = fopen(nome_input, "r");
    
    // Aloca grafo completo, e todos os seus componentes
    // Leitura do arquivo (input.txt)
    int b,g,r;
    for(int i = 0; i < pallete->size; i++){
        fscanf(input, "%d,%d,%d", &b, &g,&r);
        pallete->array[i] = criaCor(r/255,g/255,b/255);
    }

    return pallete;
}
// Função auxiliar que encontra a cor mais próxima, 
Cor nearest_pallete_color(float r1, float g1, float b1, Pallete* pallete){
    float min_dist = 1000.0;
    Cor nearest_color;

    for(int i = 0; i < pallete->size; i++){
        float r2, g2, b2;
        r2 = pallete->array[i].canais[0];
        g2 = pallete->array[i].canais[1];
        b2 = pallete->array[i].canais[2];

        float dist = (r1 - r2) * (r1 - r2) + (g1 - g2) * (g1 - g2) + (b1 - b2) * (b1 - b2); // distância RGB
        
        if(dist < min_dist) {
            min_dist = dist;
            nearest_color = pallete->array[i];
        }
    }

    return nearest_color;
}

#define KERNEL_SIZE 4 // 2 ou 4

Imagem* dither(Imagem* input,int width, Pallete* pallete){
    Imagem* output = criaImagem(input->largura, input->altura, 1);
    Imagem* cinza = criaImagem(input->largura, input->altura, 1); // Conversão para cinza, 8bpp
    float pixel_antigo[3];
    float quant_erro[3];

    for(int y = 0; y < input->altura; y++){
        for(int x = 0; x < input->largura; x++){
            Cor cor_aux = nearest_pallete_color(output->dados[0][y][x],output->dados[1][y][x],output->dados[2][y][x],pallete);
            for (int i = 0; i<3;i++){
                pixel_antigo[i] = output->dados[i][y][x];
                output->dados[i][y][x] = cor_aux.canais[i];
                quant_erro[i] = pixel_antigo[i] - output->dados[i][y][x];
                output->dados[i][y][x+1] = output->dados[i][y][x+1] +  quant_erro[i] * 7 / 16;
                output->dados[i][y+1][x-1] = output->dados[i][y+1][x-1] +  quant_erro[i] * 3 / 16;
                output->dados[i][y+1][x] = output->dados[i][y+1][x] +  quant_erro[i] * 5 / 16;
                output->dados[i][y+1][x+1] = output->dados[i][y+1][x+1] +  quant_erro[i] / 16;
            }
        }
    }

    return output;
}

int main(int argc, char* argv[]){
    char* entrada_arquivo = argv[1];
    char* saida_arquivo = argv[2];
    char* kmeans_arquivo = argv[3];
    
    Imagem* entrada = abreImagem(entrada_arquivo, 3); // Abre imagem
    Imagem* dithered = dither(entrada,32,create_kmeans_pallete(kmeans_arquivo)); // Processa imagem
    salvaImagem(dithered, saida_arquivo); // Salva imagem

    return 0;
}
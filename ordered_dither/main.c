#include "imagem.h"
#include "cores.h"
#include <stdio.h>
#include <stdio.h>

// Ordered dither
float nearest_pallete_color(float c){
    if(c <= 0.5) return 0.0;
    else return 1.0;
}

#define KERNEL_SIZE 4 // 2 ou 4

Imagem* dither(Imagem* input){
    Imagem* output = criaImagem(input->largura, input->altura, 1);
    Imagem* cinza = criaImagem(input->largura, input->altura, 1); // Conversão para cinza, 8bpp
    if(input->n_canais == 3){
        RGBParaCinza(input, cinza);
    }

    float kernel2x2[2][2] = {
        {-0.25, 0.25},
        {0.5, 0.0}
    };

    float kernel4x4[4][4] = {
        {0.0000, 0.5000, 0.1250, 0.6250},
        {0.7500, 0.2500, 0.8750, 0.3750},
        {0.1875, 0.6875, 0.0625, 0.5625},
        {0.9375, 0.4375, 0.8125, 0.3125}
    };

    float* kernel;

    if(KERNEL_SIZE == 2) {
        for(int y = 0; y < input->altura; y++){
            for(int x = 0; x < input->largura; x++){
                output->dados[0][y][x] = nearest_pallete_color(cinza->dados[0][y][x] + 0.5 * kernel2x2[y % 2][x % 2]);
            }
        }
    }

    if(KERNEL_SIZE == 4) {
        for(int y = 0; y < input->altura; y++){
            for(int x = 0; x < input->largura; x++){
                output->dados[0][y][x] = nearest_pallete_color(cinza->dados[0][y][x] + 0.5 * kernel4x4[y % 2][x % 2]);
            }
        }
    }

    return output;
}

int main(int argc, char* argv[]){
    char* entrada_arquivo = argv[1];
    char* saida_arquivo = argv[2];

    Imagem* entrada = abreImagem(entrada_arquivo, 3); // Abre imagem
    Imagem* dithered = dither(entrada); // Processa imagem
    salvaImagem(dithered, saida_arquivo); // Salva imagem

    return 0;
}
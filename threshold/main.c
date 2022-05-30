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
        RGBParaCinza(input, output);
    }

    for(int y = 0; y < input->altura; y++){
        for(int x = 0; x < input->largura; x++){
            output->dados[0][y][x] = nearest_pallete_color(output->dados[0][y][x]);
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
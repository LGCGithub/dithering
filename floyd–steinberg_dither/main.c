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
    float pixel_antigo;
    float quant_erro;
    
    if(input->n_canais == 3){
        RGBParaCinza(input, output);
    }

    for(int y = 0; y < input->altura; y++){
        for(int x = 0; x < input->largura; x++){
            pixel_antigo = output->dados[0][y][x];
            output->dados[0][y][x] = nearest_pallete_color(pixel_antigo);
            quant_erro = pixel_antigo - output->dados[0][y][x];
            output->dados[0][y][x+1] = output->dados[0][y][x+1] +  quant_erro * 7 / 16;
            output->dados[0][y+1][x-1] = output->dados[0][y+1][x-1] +  quant_erro * 3 / 16;
            output->dados[0][y+1][x] = output->dados[0][y+1][x] +  quant_erro * 5 / 16;
            output->dados[0][y+1][x+1] = output->dados[0][y+1][x+1] +  quant_erro / 16;
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
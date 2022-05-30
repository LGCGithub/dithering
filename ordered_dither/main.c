#include "imagem.h"
#include "cores.h"

#include <stdio.h>
#include <stdlib.h>

// Função auxiliar que encontra a cor mais próxima, 
float nearest_pallete_color(float c, int bpp){
    float pow = 1.0;
    for(int i = 0; i < bpp; i++) pow *= 2;

    float gap = 1.0 / pow;
    float gray_gap = 1.0 / (pow - 1);

    float atual = 0.0;
    float next = gap;
    float gray = 0.0;

    for(int i = 0; i < pow; i++){
        if(atual <= c && c <= next) return gray;
        atual += gap;
        next += gap;
        gray += gray_gap;
    }
    
    if(c <= 0.0) return 0.0;
    if(c >= 1.0) return 1.0;
}

unsigned short bitwise_xor(unsigned short a, unsigned short b){
    return a ^ b;
}

unsigned int bit_interleave(unsigned short a, unsigned short b){ // 
    unsigned int n = 0; // 16 bits, 8 + 8 bits
    for(int i = 0; i < 16; i++) {
        n |= (a & 1U << i) << i | (b & 1U << i) << (i + 1);
    }
    return n;
}

static unsigned char lookup[16] = {
0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf, };

// bruxaria com bits
unsigned int bit_reverse(unsigned int a){
    return (lookup[a&0b1111] << 4) | lookup[a>>4];
}

Imagem* dither(Imagem* input, int width, int bpp){
    Imagem* output = criaImagem(input->largura, input->altura, 1);
    Imagem* cinza = criaImagem(input->largura, input->altura, 1); // Conversão para cinza, 8bpp
    if(input->n_canais == 3){
        RGBParaCinza(input, cinza);
    }

    float** kernel;
    if(width == 1) {
        kernel = (float**)malloc(sizeof(float));
        kernel[0] = (float*)malloc(sizeof(float));
        kernel[0][0] = 0.5;
    } else {
        kernel = (float**)malloc(sizeof(float*) * width);
        for(int i = 0; i < width; i++) kernel[i] = (float*)malloc(sizeof(float) * width);

        for(int y = 0; y < width; y++){
            for(int x = 0; x < width; x++){
                kernel[y][x] = bit_reverse(bit_interleave(bitwise_xor(y, x), y)) / ((128 / (width / 2)) / width) / (float)(width * width);
            }
        }
    }

    int pow = 1;

    for(int i = 0; i < bpp; i++) pow *= 2;

    float r = 1.0 / pow;

    for(int y = 0; y < input->altura; y++){
        for(int x = 0; x < input->largura; x++){
            output->dados[0][y][x] = nearest_pallete_color(cinza->dados[0][y][x] + r * (kernel[y % width][x % width] - 0.5), bpp);
        }
    }

    return output;
}

int main(int argc, char* argv[]){
    char* entrada_arquivo = argv[1];
    char* saida_arquivo = argv[2];
    int width = atoi(argv[3]);
    int bpp = atoi(argv[4]);

    Imagem* entrada = abreImagem(entrada_arquivo, 3); // Abre imagem
    Imagem* dithered = dither(entrada, width, bpp); // Processa imagem
    salvaImagem(dithered, saida_arquivo); // Salva imagem

    printf("Sucesso\n");

    return 0;
}
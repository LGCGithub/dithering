#include "imagem.h"
#include "cores.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct pallete {
    Cor* array;
    int size;
} Pallete;

Pallete* create_random_pallete(Imagem* img, int size){
    Pallete* pallete = (Pallete*)malloc(sizeof(Pallete));
    pallete->array = (Cor*)malloc(sizeof(Cor) * size);
    pallete->size = size;

    for(int i = 0; i < size; i++){
        int x = rand() % img->largura;
        int y = rand() % img->altura;

        float r, g, b;
        r = img->dados[0][y][x];
        g = img->dados[1][y][x];
        b = img->dados[2][y][x];

        pallete->array[i] = criaCor(r, g, b);
    }

    return pallete;
}

Pallete* create_ega_pallete(){
    Pallete* pallete = (Pallete*)malloc(sizeof(Pallete));
    pallete->array = (Cor*)malloc(sizeof(Cor) * 64);
    pallete->size = 64;

    int i = 0;
    for(int r = 0; r < 4; r++){
        for(int g = 0; g < 4; g++){
            for(int b = 0; b < 4; b++){
                pallete->array[i++] = criaCor(r / 3.0, g / 3.0, b / 3.0); 
            }
        }
    }

    return pallete;
}

Pallete* create_k_means_pallete(Imagem* img, int k, int interacoes){
    Pallete* pallete = (Pallete*)malloc(sizeof(Pallete));
    pallete->array = (Cor*)malloc(sizeof(Cor) * k);
    pallete->size = k;

    // Inicia os valores aleatorios
    for(int i = 0; i < k; i++){

        float r, g, b;
        r = rand() / (float)RAND_MAX;
        g = rand() / (float)RAND_MAX;
        b = rand() / (float)RAND_MAX;

        pallete->array[i] = criaCor(r, g, b);
    }

    Imagem* buffer = criaImagem(img->largura, img->altura, 1); // Buffer de id's

    Pallete* new_pallete = (Pallete*)malloc(sizeof(Pallete));
    new_pallete->array = (Cor*)malloc(sizeof(Cor) * k);
    new_pallete->size = k;


    for(int inter = 0; inter < interacoes; inter++){
        for(int y = 0; y < img->altura; y++){
            for(int x = 0; x < img->largura; x++){
                float r1, g1, b1;
                r1 = img->dados[0][y][x];
                g1 = img->dados[1][y][x];
                b1 = img->dados[2][y][x];

                float min_dist = 1000.0;
                // Calcula a cor da paleta mais próxima
                for(int i = 0; i < k; i++){
                    float r2, g2, b2;
                    r2 = pallete->array[i].canais[0];
                    g2 = pallete->array[i].canais[1];
                    b2 = pallete->array[i].canais[2];

                    float dist = (r1 - r2) * (r1 - r2) + (g1 - g2) * (g1 - g2) + (b1 - b2) * (b1 - b2); // distancia RGB

                    if(dist < min_dist){
                        buffer->dados[0][y][x] = i;
                        min_dist = dist;
                    }
                }
            }
        }

        //for(int y = 0; y < img->altura; y++){
        //    for(int x = 0; x < img->largura; x++){
        //        printf("%lf ", buffer->dados[0][y][x]);
        //    }
        //    printf("\n");
        //}

        
        // Paleta atualizada
    
        for(int i = 0; i < k; i++){
            float r, g, b;
            r = 0;
            g = 0;
            b = 0;

            new_pallete->array[i] = criaCor(r, g, b);
        }

        int* aux = (int*)malloc(sizeof(int) * k);
        for(int i = 0; i < k; i++) aux[i] = 0;

        for(int y = 0; y < img->altura; y++){
            for(int x = 0; x < img->largura; x++){
                int index = buffer->dados[0][y][x];

                new_pallete->array[index].canais[0] += img->dados[0][y][x];
                new_pallete->array[index].canais[1] += img->dados[1][y][x];
                new_pallete->array[index].canais[2] += img->dados[2][y][x];

                aux[index]++;
            }
        }

        for(int i = 0; i < k; i++) {
            new_pallete->array[i].canais[0] /= aux[i];
            new_pallete->array[i].canais[1] /= aux[i];
            new_pallete->array[i].canais[2] /= aux[i];
        }


        // atualizar paleta
        for(int i = 0; i < k; i++){
            pallete->array[i].canais[0] = new_pallete->array[i].canais[0];
            pallete->array[i].canais[1] = new_pallete->array[i].canais[1];
            pallete->array[i].canais[2] = new_pallete->array[i].canais[2];
        }

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

Imagem* dither(Imagem* input, int width, Pallete* pallete){
    Imagem* output = criaImagem(input->largura, input->altura, 3);

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

    // Task: Descobrir como calcular essa valor para paletas aleatorias
    float r = 0.25; 
    // noise magnitude
    // higher => less color banding, noise more evident
    // lower => more color banding, noise less evident
    // middle => ideal

    for(int y = 0; y < input->altura; y++){
        for(int x = 0; x < input->largura; x++){
            float noise = r * (kernel[y % width][x % width] - 0.5);

            float r, g, b;
            r = input->dados[0][y][x] + noise;
            g = input->dados[1][y][x] + noise;
            b = input->dados[2][y][x] + noise;

            Cor color = nearest_pallete_color(r, g, b, pallete);

            output->dados[0][y][x] = color.canais[0];
            output->dados[1][y][x] = color.canais[1];
            output->dados[2][y][x] = color.canais[2];
        }
    }

    return output;
}

int main(int argc, char* argv[]){
    srand(time(NULL));

    char* entrada_arquivo = argv[1]; // nome do arquivo de entrada
    char* saida_arquivo = argv[2]; // nome do arquivo de saida
    int width = atoi(argv[3]); // tamanho do kernel
    int mode = atoi(argv[4]); // modo de dither
    int size = atoi(argv[5]); // tamanho da paleta
    int inter; // numero de interações

    if(mode == 3) inter = atoi(argv[6]);

    Imagem* entrada = abreImagem(entrada_arquivo, 3); // Abre imagem

    Pallete* pallete = NULL;

    if(mode == 1) pallete = create_ega_pallete(); // 64 cores, fixa
    if(mode == 2) pallete = create_random_pallete(entrada, size);
    if(mode == 3) pallete = create_k_means_pallete(entrada, size, inter);

    Imagem* dithered = dither(entrada, width, pallete); // Processa imagem
    salvaImagem(dithered, saida_arquivo); // Salva imagem

    printf("Sucesso\n");

    return 0;
}
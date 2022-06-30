#include "imagem.h"
#include "cores.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <limits.h>

typedef struct pallete {
    Cor* array;
    int size;
} Pallete;

typedef struct node {
    struct node* next;
    struct node* prev;
    Cor value;
} Node;

typedef struct lista
{
    Node* inicio;
    Node* fim;
    unsigned long int size;
}Lista;

// Função que retorna um unsigned long int aleatorio
unsigned long int randLI(){
    // 32 bits
    unsigned long int num = 0;
    unsigned long int pow = 1;

    for(int i = 0; i < 32; i++){
        num += pow * (rand() % 2);
        pow *= 2;
    }

    return num;
}

// Checa se duas cores são iguais
int coresIguais(Cor a, Cor b){
    if(a.canais[0] == b.canais[0] && a.canais[1] == b.canais[1] && a.canais[2] == b.canais[2]) return 1;
    return 0;
}

/* Arrange the N elements of ARRAY in random order.
   Only effective if N is much smaller than RAND_MAX;
   if this may not be the case, use a better random
   number generator. */
// Versão modificada, original feita por Ben Pfaff

void shuffle(Cor *array, size_t n)
{
    if (n > 1) {
        size_t i;
        for (i = 0; i < n - 1; i++) {
        size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
        Cor t = array[j];
        array[j] = array[i];
        array[i] = t;
        }
    }
}

Lista* create_list(){
    Lista* new = (Lista*)malloc(sizeof(Lista));
    new->inicio = NULL;
    new->fim = NULL;
    new->size = 0;
    return new;
}

Cor pop(Lista* lista){
    if(lista->inicio == NULL) {
        return criaCor(-1.0, -1.0, -1.0);
    }
    else if(lista->inicio == lista->fim){
        lista->size = 0;

        Node* unico = lista->inicio;

        lista->inicio = NULL;
        lista->fim = NULL;

        Cor value = unico->value;

        free(unico);

        return value;
    } else {
        lista->size--;

        Node* fim = lista->fim;
        Node* penultimo = fim->prev;
        Cor value = fim->value;

        lista->fim = penultimo;
        penultimo->next = NULL;

        free(fim);

        return value;
    }
}

void push(Lista* lista, Cor value){
    Node* new = (Node*)malloc(sizeof(Node));
    new->next = NULL;
    new->prev = NULL;
    new->value = value;

    if(lista->inicio == NULL){
        lista->inicio = new;
        lista->fim = new;
        lista->size = 1;
    }
    else if(lista->inicio == lista->fim){
        new->prev = lista->inicio;
        lista->inicio->next = new;
        lista->fim = new;
        lista->size = 2;
    } else {
        new->prev = lista->fim;
        lista->fim->next = new;
        lista->fim = new;
        lista->size++;
    }
}

Pallete* create_random_pallete(Imagem* img, int size){
    Pallete* pallete = (Pallete*)malloc(sizeof(Pallete));
    pallete->array = (Cor*)malloc(sizeof(Cor) * size);
    pallete->size = 0;

    Lista* cores_unicas = create_list();

    // Listar cores unicas

    for(int y = 0; y < img->altura; y++){
        for(int x = 0; x < img->largura; x++){
            Cor corAtual = criaCor(img->dados[0][y][x], img->dados[1][y][x], img->dados[2][y][x]);
            int not_in_list = 1;
            Node* atual = cores_unicas->inicio;

            while(atual != NULL){
                if(coresIguais(corAtual, atual->value)) {
                    not_in_list = 0;
                    break;
                }
                atual = atual->next;
            } 

            if(not_in_list) push(cores_unicas, corAtual);
        }
    }

    // Se a quantidade de cores unicas na imagem for menor que a quantidade máxima de cores da paleta, copia todas para a paleta
    if(cores_unicas->size <= size){
        Node* atual = cores_unicas->inicio;
        for(int k = 0; k < cores_unicas->size; k++) {
            pallete->array[k] = atual->value;
            atual = atual->next;
        }
        pallete->size = cores_unicas->size;
    } else {
        Cor* cores_unicas_aux = (Cor*)malloc(sizeof(Cor) * cores_unicas->size);
        
        Node* atual = cores_unicas->inicio;
        for(int i = 0; i < cores_unicas->size; i++){
            cores_unicas_aux[i] = atual->value;
            atual = atual->next;
        }

        // Embaralha o array para criar uma ordem de cores aleatorias
        shuffle(cores_unicas_aux, cores_unicas->size);

        for(int k = 0; k < size; k++){
            pallete->array[k] = cores_unicas_aux[k];
        }

        pallete->size = size;
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
    pallete = create_random_pallete(img, k);
    k = pallete->size;

    Imagem* buffer = criaImagem(img->largura, img->altura, 1); // Buffer de id's

    Pallete* new_pallete = (Pallete*)malloc(sizeof(Pallete));
    new_pallete->array = (Cor*)malloc(sizeof(Cor) * k);
    new_pallete->size = pallete->size;

    int* cores_unicas = (int*)malloc(sizeof(int) * k); 

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
            new_pallete->array[i] = criaCor(0.0, 0.0, 0.0);
        }

        for(int i = 0; i < k; i++) cores_unicas[i] = 0;

        for(int y = 0; y < img->altura; y++){
            for(int x = 0; x < img->largura; x++){
                int index = buffer->dados[0][y][x];

                new_pallete->array[index].canais[0] += img->dados[0][y][x];
                new_pallete->array[index].canais[1] += img->dados[1][y][x];
                new_pallete->array[index].canais[2] += img->dados[2][y][x];

                cores_unicas[index]++;
            }
        }

        for(int i = 0; i < k; i++) {
            new_pallete->array[i].canais[0] /= cores_unicas[i];
            new_pallete->array[i].canais[1] /= cores_unicas[i];
            new_pallete->array[i].canais[2] /= cores_unicas[i];
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

Imagem* dither(Imagem* input,int width, Pallete* pallete){
    Imagem* output = criaImagem(input->largura, input->altura, 3);
    Imagem* erro = criaImagem(input->largura, input->altura, 3);
    Imagem* erro1pb = criaImagem(input->largura, input->altura, 1);
    Imagem* erro2pb = criaImagem(input->largura, input->altura, 1);
    Imagem* erro3pb = criaImagem(input->largura, input->altura, 1);
    float pixel_antigo[3];
    float quant_erro[3];
    //memcpy(input,&output,sizeof input);
    for(int y = 0; y < input->altura; y++){
        for(int x = 0; x < input->largura; x++){
            for (int i = 0; i<3;i++){
                output->dados[i][y][x] = input->dados[i][y][x];
            }}}
    for(int y = 0; y < input->altura; y++){
        for(int x = 0; x < input->largura; x++){
            Cor cor_aux = nearest_pallete_color(output->dados[0][y][x],output->dados[1][y][x],output->dados[2][y][x],pallete);
            for (int i = 0; i<3;i++){
                pixel_antigo[i] = output->dados[i][y][x];
                output->dados[i][y][x] = cor_aux.canais[i];
                quant_erro[i] = pixel_antigo[i] - output->dados[i][y][x];
                if (x+1 < input->largura)
                    output->dados[i][y][x+1] = output->dados[i][y][x+1] +  quant_erro[i] * 7.0 / 16.0;
                if (y+1 < input->altura){
                    output->dados[i][y+1][x-1] = output->dados[i][y+1][x-1] +  quant_erro[i] * 3.0 / 16.0;
                    output->dados[i][y+1][x] = output->dados[i][y+1][x] +  quant_erro[i] * 5.0 / 16.0;
                    output->dados[i][y+1][x+1] = output->dados[i][y+1][x+1] +  quant_erro[i] / 16.0;
                }
            }
        }
    }
    return output;
}
void salvaPallete(Pallete* pallete, char* arquivo){
    int largura = pow(pallete->size,0.5);
    int altura = largura;
    while (largura*altura < pallete->size) altura++;
    Imagem* imgPaleta = criaImagem(largura*8+1, altura*8 +1, 3);
    int coresColocadas = 0;
    for (int y = 1;y < altura*8;y += 8){
        for(int x = 1;x<largura*8; x+=8){
            for(int j = y;j <y+7;j++){
                for (int i = x;i<x+7;i++){
                    imgPaleta->dados[0][j][i] = pallete->array[coresColocadas].canais[0];
                    imgPaleta->dados[1][j][i] = pallete->array[coresColocadas].canais[1];
                    imgPaleta->dados[2][j][i] = pallete->array[coresColocadas].canais[2];
                }
            }
            coresColocadas++;
            if (coresColocadas >= pallete->size){
                break;
            }
        }
        if (coresColocadas >= pallete->size){
                break;
            }
    }
    for (int y = 0;y < altura*8+1;y ++){
        for(int x = 0;x<largura*8+1; x++){
            if(!(y%8) || !(x%8)){
                imgPaleta->dados[0][y][x] = 0;
                imgPaleta->dados[1][y][x] = 0;
                imgPaleta->dados[2][y][x] = 0;
            }
        }
    }

    salvaImagem(imgPaleta, arquivo);
}

int main(int argc, char* argv[]){
    srand(time(NULL));

    char* entrada_arquivo = argv[1]; // nome do arquivo de entrada
    char* saida_arquivo = argv[2]; // nome do arquivo de saida
    char* saida_paleta = argv[3];
    int width = atoi(argv[4]); // tamanho do kernel
    int mode = atoi(argv[5]); // modo de dither
    int size = atoi(argv[6]); // tamanho da paleta
    int inter; // numero de interações

    if(mode == 3) inter = atoi(argv[7]);

    Imagem* entrada = abreImagem(entrada_arquivo, 3); // Abre imagem

    Pallete* pallete = NULL;

    if(mode == 1) pallete = create_ega_pallete(); // 64 cores, fixa
    if(mode == 2) pallete = create_random_pallete(entrada, size);
    if(mode == 3) pallete = create_k_means_pallete(entrada, size, inter);

    salvaPallete(pallete, saida_paleta);
    Imagem* dithered = dither(entrada, width, pallete); // Processa imagem
    salvaImagem(dithered, saida_arquivo); // Salva imagem

    printf("Sucesso\n");

    return 0;
}
#include "imagem.h"
#include "cores.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Struct que guarda as informações relativa a uma paleta
// O vetor de cores representa as cores da paleta, e o valor inteiro size o tamanho do vetor.
typedef struct pallete {
    Cor* array;
    int size;
} Pallete;

// Nó de uma lista duplamente encadeada. 
// Utilizo na hora de calcular quantas cores únicas existem na imagem, 
// numa lista que funciona como uma espécie de "vetor com tamanho dinâmico"
// Faço isso pois não sei quantas cores únicas existem na imagem, logo não posso alocar um valor fixo.
typedef struct node {
    struct node* next;
    struct node* prev;
    Cor value;
} Node;

// Struct auxiliar que aponta para o inicio e fim de uma lista encadeada.
// Também guarda o tamanho da lista.
typedef struct lista
{
    Node* inicio;
    Node* fim;
    unsigned long int size;
}Lista;

// Função auxiliar que retorna um unsigned long int aleatorio
// A função rand consegue gerar apenas valores até RAND_MAX, mas essa função consegue gerar valores até 2^32 - 1.
// Eu ia utilizar ela pra calcular valores aleatorios melhores, mas acabei não usando.
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

// Checa se duas cores são iguais, retorna 1 se true e 0 se false
int coresIguais(Cor a, Cor b){
    if(a.canais[0] == b.canais[0] && a.canais[1] == b.canais[1] && a.canais[2] == b.canais[2]) return 1;
    return 0;
}

/* Arrange the N elements of ARRAY in random order.
   Only effective if N is much smaller than RAND_MAX;
   if this may not be the case, use a better random
   number generator. */
// Versão modificada, original feita por Ben Pfaff
// Função que embaralha um array. Utilizo para gerar um array "aleatorio" de cores.
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

// Função que cria uma lista encadeada vazia.
Lista* create_list(){
    Lista* new = (Lista*)malloc(sizeof(Lista));
    new->inicio = NULL;
    new->fim = NULL;
    new->size = 0;
    return new;
}

// Função que remove o ultimo elemento da lista encadeada e retorna o seu valor (cor).
// Não utilizei essa função, mas fiz por completude.
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

// Função que adiciona um elemento no final da lista encadeada. Utilizo para criar uma lista de cores únicas da imagem.
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

// Versão melhorada/alternativa. A paleta não é mais completamente aleatoria, mas sim é uma parte das cores que já existem na imagem.
// Essa função é lenta.
Pallete* create_random_pallete(Imagem* img, int size){
    Pallete* pallete = (Pallete*)malloc(sizeof(Pallete));
    pallete->array = (Cor*)malloc(sizeof(Cor) * size);
    pallete->size = 0;

    // Lista de cores únicas na imagem.
    Lista* cores_unicas = create_list();

    // Passa pela imagem, adicionando na lista toda cor que aparecer pela primeira vez.
    for(int y = 0; y < img->altura; y++){
        for(int x = 0; x < img->largura; x++){
            Cor corAtual = criaCor(img->dados[0][y][x], img->dados[1][y][x], img->dados[2][y][x]);
            int not_in_list = 1;
            Node* atual = cores_unicas->inicio; // Inicio da lista encadeada.

            // Varre a lista encadeada, buscando por uma cor igual.
            // Se a cor já estiver na lista, pula para o próximo pixel.
            while(atual != NULL){
                if(coresIguais(corAtual, atual->value)) {
                    not_in_list = 0; // Se encontrar uma cor igual/repetida, quebra o loop
                    break;
                }
                atual = atual->next;
            } 

            // Se a cor não apareceu na lista, é a primeira vez que ela aparece na imagem.
            // Ela é adicionada na lista de cores únicas.
            if(not_in_list) push(cores_unicas, corAtual);
        }
    }

    // Caso o número de cores da imagem seja menor que o número de cores da paleta, trata essa caso.
    // Se a quantidade de cores unicas na imagem for menor que a quantidade máxima de cores da paleta, copia todas para a paleta.
    if(cores_unicas->size <= size){
        Node* atual = cores_unicas->inicio;
        for(int k = 0; k < cores_unicas->size; k++) {
            pallete->array[k] = atual->value;
            atual = atual->next;
        }
        pallete->size = cores_unicas->size;
    } else { // Caso contrário, adiciona uma parte das cores únicas na paleta.
        Cor* cores_unicas_aux = (Cor*)malloc(sizeof(Cor) * cores_unicas->size); // Vetor auxiliar
        
        // Para selecionar cores aleatorias, eu copio a lista de cores únicas para um vetor e embaralho ele.
        // As primeiras cores são selecionadas para fazer parte da paleta.

        // Copia a lista para um vetor
        Node* atual = cores_unicas->inicio;
        for(int i = 0; i < cores_unicas->size; i++){
            cores_unicas_aux[i] = atual->value;
            atual = atual->next;
        }

        // Embaralha o vetor para criar uma ordem de cores aleatorias
        shuffle(cores_unicas_aux, cores_unicas->size);

        // Passa as primeiras cores para a paleta
        for(int k = 0; k < size; k++){
            pallete->array[k] = cores_unicas_aux[k];
        }

        pallete->size = size;
    }
        
    return pallete;
}

// Cria uma paleta EGA com 64 cores.
Pallete* create_ega_pallete(){
    Pallete* pallete = (Pallete*)malloc(sizeof(Pallete));
    pallete->array = (Cor*)malloc(sizeof(Cor) * 64);
    pallete->size = 64;

    // A paleta EGA é basicamente todas as combinações de cores que 
    // podem ser formadas com os valores 0.000, 0.333, 0.666, 1.000 para o R, G, e o B
    // No total, existem 64 combinações possíveis

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

// Cria uma paleta utilizando o método k-means para selecionar as cores.
Pallete* create_k_means_pallete(Imagem* img, int k, int interacoes){
    Pallete* pallete = (Pallete*)malloc(sizeof(Pallete));
    pallete->array = (Cor*)malloc(sizeof(Cor) * k);
    pallete->size = k;

    // Inicia os valores aleatorios, utiliza a função já criada.
    pallete = create_random_pallete(img, k);
    k = pallete->size;

    Imagem* buffer = criaImagem(img->largura, img->altura, 1); // Buffer de id's

    // Paleta auxiliar, temporaria, somente para calcular a nova paleta entre as interações.
    Pallete* new_pallete = (Pallete*)malloc(sizeof(Pallete));
    new_pallete->array = (Cor*)malloc(sizeof(Cor) * k);
    new_pallete->size = pallete->size;

    // Vetor auxiliar que guarda quantas cores estão em cada cluster. Utilizo para fazer a média.
    int* cores_unicas = (int*)malloc(sizeof(int) * k); 

    // Para cada interação:
    // Associa um ID para cada pixel, que informa qual a cor da paleta mais próxima com ele.
    // Soma todas as cores com o mesmo ID e divide pelo número de cores com o mesmo ID, criando uma nova paleta
    // Copia a nova paleta para a paleta original.
    for(int inter = 0; inter < interacoes; inter++){

        // Associa um ID para cada pixel, que informa qual a cor da paleta mais próxima com ele.
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

        for(int i = 0; i < k; i++){
            new_pallete->array[i] = criaCor(0.0, 0.0, 0.0);
        }

        for(int i = 0; i < k; i++) cores_unicas[i] = 0;

        // Soma todas as cores com o mesmo ID e divide pelo número de cores com o mesmo ID, criando uma nova paleta
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


        // Copia a nova paleta para a paleta original.
        for(int i = 0; i < k; i++){
            pallete->array[i].canais[0] = new_pallete->array[i].canais[0];
            pallete->array[i].canais[1] = new_pallete->array[i].canais[1];
            pallete->array[i].canais[2] = new_pallete->array[i].canais[2];
        }

    }

    return pallete;
}

// Função auxiliar que encontra a cor mais próxima, no espaço RGB. 
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

// Bruxaria com bits para calcular o kernel.
unsigned short bitwise_xor(unsigned short a, unsigned short b){
    return a ^ b;
}

// Bruxaria com bits para calcular o kernel.
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

// Bruxaria com bits para calcular o kernel.
unsigned int bit_reverse(unsigned int a){
    return (lookup[a&0b1111] << 4) | lookup[a>>4];
}

// Função principal, que realiza o dithering em uma imagem de entrada.
Imagem* dither(Imagem* input, int width, Pallete* pallete){
    Imagem* output = criaImagem(input->largura, input->altura, 3);

    // Cria o kernel
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
    // Vetor que guarda o quão espalhado estão as cores no espaço RGB.
    // Representa a "magnitude" do ruido que será adicionado na imagem.
    float r[3]= {0.0, 0.0, 0.0};
    
    // Calcula a distancia média entre as cores, em 1d, para cada canal.
    // A distancia média é o quanto um valor tem que mudar para cair no dominio de outra cor. 
    for(int k = 0; k < pallete->size; k++){
        for(int i = 0; i < 3; i++) r[i] += pallete->array[k].canais[i] / (pallete->size * 2);
    }

    // Aplica o ruido na imagem e quantiza com base na paleta.
    for(int y = 0; y < input->altura; y++){
        for(int x = 0; x < input->largura; x++){
            float noise[3];
            for(int i = 0; i < 3; i++)  noise[i] = r[i] * (kernel[y % width][x % width] - 0.5);
            
            float r, g, b;
            r = input->dados[0][y][x] + noise[0];
            g = input->dados[1][y][x] + noise[1];
            b = input->dados[2][y][x] + noise[2];

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
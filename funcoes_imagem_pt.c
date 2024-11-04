#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" // Para carregar as imagens
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h" // Para salvar as imagens

// Estrutura para armazenar a imagem
typedef struct {
    int largura;
    int altura;
    int canais;
    unsigned char *pixels;
} Imagem;

/** @brief Carrega a imagem
 *
 * @param nomeDaImagem Nome do arquivo da imagem a ser carregado
 *
 * @return Retorna um ponteiro para a imagem carregada, ou NULL se não carregar
 */
Imagem *carregarImagem(const char *nomeDaImagem) {
    int largura, altura, canais;

    // Carrega os dados da imagem
    unsigned char *dadosDaImagem = stbi_load(nomeDaImagem, &largura, &altura, &canais, 0);
    if (!dadosDaImagem) {
        printf("Erro ao carregar a imagem: %s\n", nomeDaImagem);
        return NULL;
    }

    // Aloca memoria para a imagem
    Imagem *imagem = (Imagem *)malloc(sizeof(Imagem));
    if (!imagem) {
        printf("Erro ao alocar memoria para a imagem.\n");
        stbi_image_free(dadosDaImagem);
        return NULL;
    }

    imagem->largura = largura;
    imagem->altura = altura;
    imagem->canais = canais;
    imagem->pixels = dadosDaImagem;

    printf("Imagem carregada: %s, dimensoes: %d x %d, canais: %d\n", nomeDaImagem, largura, altura, canais);

    return imagem;
}

/** @brief Salva a imagem
 *
 * @param nomeDaImagem Nome que a imagem será salva
 * @param imagem A imagem que será salva
 */
void salvarImagem(const char *nomeDaImagem, const Imagem *imagem) {
    // Salva a imagem e retorna um boleano se foi salvo corretamente
    bool imagemSalva = stbi_write_png(nomeDaImagem, imagem->largura, imagem->altura, imagem->canais, imagem->pixels, imagem->largura * imagem->canais);

    // Imprime se a imagem foi salva
    if (imagemSalva) {
        printf("Imagem salva com sucesso: %s\n", nomeDaImagem);
    } else {
        printf("Erro ao salvar a imagem: %s\n", nomeDaImagem);
    }
}

/** @brief Libera a memoria alocada para a imagem
 *
 * @param imagem A imagem que será liberada
 */
void freeImagem(Imagem *imagem) {
    stbi_image_free(imagem->pixels);
    free(imagem);
}

/** @brief Ajusta um valor para um intervalo
 *
 * @param valor O valor que será ajustado
 * @param min O valor mínimo
 * @param max O valor máximo
 *
 * @return O valor ajustado
 */
int ajustar(int valor, int min, int max) {
    if (valor < min)
        return min;
    if (valor > max)
        return max;
    return valor;
}

/** @brief Inverte os valores dos pixels da imagem
 *
 * @param imagem A imagem que terá os pixels invertidos
 *
 * @return A imagem com os pixels invertidos
 */
Imagem *inverterPixels(const Imagem *imagem) {
    // Aloca memoria para a imagem invertida
    Imagem *imagemInvertida = (Imagem *)malloc(sizeof(Imagem));
    if (!imagemInvertida) {
        printf("Erro ao alocar memoria para a imagem invertida\n");
        return NULL;
    }

    // Atribui os valores a nova imagem e aloca memoria necessária pasa os pixels
    imagemInvertida->largura = imagem->largura;
    imagemInvertida->altura = imagem->altura;
    imagemInvertida->canais = imagem->canais;
    imagemInvertida->pixels = (unsigned char *)malloc(imagem->largura * imagem->altura * imagem->canais * sizeof(unsigned char));
    if (!imagemInvertida->pixels) {
        free(imagemInvertida);
        printf("Erro ao alocar memoria para os pixels da imagem invertida\n");
        return NULL;
    }

    // Percorre cada pixel da imagem e inverte os valores
    for (int i = 0; i < imagem->largura * imagem->altura * imagem->canais; ++i) {
        imagemInvertida->pixels[i] = 255 - imagem->pixels[i];
    }

    return imagemInvertida;
}

/** @brief Converte a imagem para preto e branco
 *
 * @param imagem A imagem que será convertida
 *
 * @return A imagem convertida para preto e branco
 */
Imagem *converterPeB(const Imagem *imagem) {
    // Aloca memoria para a imagem convertida
    Imagem *imagemPeB = (Imagem *)malloc(sizeof(Imagem));
    if (!imagemPeB) {
        printf("Erro ao alocar memoria para a imagem preto e branco\n");
        return NULL;
    }

    unsigned char *pixelsPeB = (unsigned char *)malloc(imagem->largura * imagem->altura * sizeof(unsigned char));
    if (!imagemPeB->pixels) {
        free(imagemPeB);
        printf("Erro ao alocar memoria para os pixels da imagem preto e branco\n");
        return NULL;
    }

    // Converte o pixel para preto e branco com base no número de canais
    for (int i = 0; i < imagem->largura * imagem->altura; ++i) {
        // Se a imagem tiver 3 ou 4 canais (RGB ou RGBA)
        if (imagem->canais == 3 || imagem->canais == 4) {
            unsigned char r = imagem->pixels[i * imagem->canais];
            unsigned char g = imagem->pixels[i * imagem->canais + 1];
            unsigned char b = imagem->pixels[i * imagem->canais + 2];
            pixelsPeB[i] = (unsigned char)round(0.299 * r + 0.587 * g + 0.114 * b);
        } else if (imagem->canais == 2) { // Se a imagem tiver 2 canais (RG ou RA)
            pixelsPeB[i] = imagem->pixels[i * imagem->canais];
        } else if (imagem->canais == 1) { // Se a imagem j[a for preto e branco
            pixelsPeB[i] = imagem->pixels[i];
        } else { // Se a imagem tiver outro número de canais
            printf("Tipo de imagem não suportado\n");
            free(pixelsPeB);
            free(imagemPeB);
            return NULL;
        }
    }

    // Atribui os valores a nova imagem e aloca memoria necessária para os pixels
    imagemPeB->largura = imagem->largura;
    imagemPeB->altura = imagem->altura;
    imagemPeB->canais = 1;
    imagemPeB->pixels = pixelsPeB;
    return imagemPeB;
}

/** @brief Aplica um kernel à imagem
 *
 * @param imagem A imagem que terá o kernel aplicado
 * @param kernel O kernel que será aplicado à imagem
 * @param tamanhoKernel O tamanho de um lado do kernel
 *
 * @return A imagem após o kernel ter sido aplicado
 */
Imagem *aplicarKernel(const Imagem *imagem, const float *kernel, const int tamanhoKernel) {
    Imagem *saida = (Imagem *)malloc(sizeof(Imagem));
    if (!saida) {
        printf("Erro ao alocar memoria na aplicacao do kernel\n");
        return NULL;
    }

    // Atribui os valores à nova imagem e aloca memoria necessária para os pixels
    saida->largura = imagem->largura;
    saida->altura = imagem->altura;
    saida->canais = imagem->canais;
    saida->pixels = (unsigned char *)malloc(imagem->largura * imagem->altura * imagem->canais * sizeof(unsigned char));
    if (!saida->pixels) {
        free(saida);
        printf("Erro ao alocar memoria para a imagem de saida\n");
        return NULL;
    }

    // Percorre cada pixel da imagem
    for (int imagemY = 0; imagemY < imagem->altura; imagemY++) {
        for (int imagemX = 0; imagemX < imagem->largura; imagemX++) {

            // Passa por cada canal (para várias cores)
            for (int channelIndex = 0; channelIndex < imagem->canais; channelIndex++) {
                float pixelValue = 0.0f;

                // Percorre cada posição no kernel
                for (int kernelY = -tamanhoKernel / 2; kernelY <= tamanhoKernel / 2; kernelY++) {
                    for (int kernelX = -tamanhoKernel / 2; kernelX <= tamanhoKernel / 2; kernelX++) {

                        // Calcula a posição do pixel
                        int pixelX = ajustar(imagemX + kernelX, 0, imagem->largura - 1);
                        int pixelY = ajustar(imagemY + kernelY, 0, imagem->altura - 1);

                        // Calcula a posição (índice) na imagem e no kernel
                        int pixelIndice = (pixelY * imagem->largura + pixelX) * imagem->canais + channelIndex;
                        int kernelIndice = (kernelY + tamanhoKernel / 2) * tamanhoKernel + (kernelX + tamanhoKernel / 2);

                        pixelValue += imagem->pixels[pixelIndice] * kernel[kernelIndice];
                    }
                }

                // Ajusta o valor do pixel para o intervalo 0-255
                saida->pixels[(imagemY * imagem->largura + imagemX) * imagem->canais + channelIndex] = (unsigned char)ajustar((int)pixelValue, 0, 255);
            }
        }
    }

    return saida;
}

/** @brief Aplica um efeito de borrado à imagem
 *
 * @param imagem A imagem que terá o efeito de borrado aplicado
 * @param nivel O nível de borrado que será aplicado (calcula o tamanho do kernel como 2 * nível + 1)
 *
 * @return A imagem após o efeito de borrado ter sido aplicado
 */
Imagem *aplicarBorrado(const Imagem *imagem, int nivel) {
    // Verifica se o nível é válido
    if (nivel < 1) {
        printf("Nivel de borramento deve ser no mínimo 1\n");
        return NULL;
    }

    // Calcula o tamanho do kernel
    int tamanhoKernel = nivel * 2 + 1;
    float *kernel = (float *)malloc(tamanhoKernel * tamanhoKernel * sizeof(float));
    if (!kernel) {
        printf("Erro ao alocar memoria para o kernel\n");
        return NULL;
    }

    // Cria o kernel e faz a divisao do peso
    for (int i = 0; i < tamanhoKernel * tamanhoKernel; i++) {
        kernel[i] = 1.0f / (tamanhoKernel * tamanhoKernel);
    }

    Imagem *imagemBorrada = aplicarKernel(imagem, kernel, tamanhoKernel);

    free(kernel);

    return imagemBorrada;
}

/** @brief Aplica um efeito de nitidez à imagem
 *
 * @param imagem A imagem que terá o efeito de nitidez aplicado
 * @param nivel O nível de nitidez que será aplicado, calcula o tamanho do kernel como 2 * nível + 1
 *
 * @return A imagem após o efeito de nitidez ter sido aplicado
 */
Imagem *aplicarNitidez(const Imagem *imagem, int nivel) {
    // Verifica se o nível é válido
    if (nivel < 1) {
        printf("Nível de nitidez inválido\n");
        return NULL;
    }

    // Aloca memoria para a imagem de saída
    Imagem *imagemNitida = (Imagem *)malloc(sizeof(Imagem));
    if (!imagemNitida) {
        printf("Erro ao alocar memoria para a imagem nitida\n");
        return NULL;
    }

    // Atribui os valores à nova imagem e aloca memoria necessária para os pixels
    imagemNitida->largura = imagem->largura;
    imagemNitida->altura = imagem->altura;
    imagemNitida->canais = imagem->canais;
    imagemNitida->pixels = (unsigned char *)malloc(imagem->largura * imagem->altura * imagem->canais * sizeof(unsigned char));
    if (!imagemNitida->pixels) {
        free(imagemNitida);
        printf("Erro ao alocar memoria para os pixels da imagem nitida\n");
        return NULL;
    }

    // Aplica o efeito de nitidez à imagem, primeiro aplicando o efeito de borrado e depois subtraindo de 2 vezes a imagem original
    Imagem *imagemBorrada = aplicarBorrado(imagem, nivel);

    // Percorre cada pixel da imagem
    for (int i = 0; i < imagem->largura * imagem->altura * imagem->canais; ++i) {
        int valorNitido = ajustar((int)(imagem->pixels[i] * 2 - imagemBorrada->pixels[i]), 0, 255);
        imagemNitida->pixels[i] = (unsigned char)valorNitido;
    }

    freeImagem(imagemBorrada);
    return imagemNitida;
}

/** @brief Aplica a detecção de borda à imagem
 *
 * @param imagem A imagem que terá a detecção de borda aplicada
 *
 * @return A imagem após a detecção de borda ter sido aplicada
 */
Imagem *aplicarDetecaoDeBorda(const Imagem *imagem) {
    // Aloca memoria para a imagem de saída
    Imagem *imagemBorda = (Imagem *)malloc(sizeof(Imagem));
    if (!imagemBorda) {
        printf("Erro ao alocar memoria para a imagem de borda\n");
        return NULL;
    }

    // Atribui os valores à nova imagem e aloca memoria necessária para os pixels
    imagemBorda->largura = imagem->largura;
    imagemBorda->altura = imagem->altura;
    imagemBorda->canais = imagem->canais;
    imagemBorda->pixels = (unsigned char *)malloc(imagem->largura * imagem->altura * imagem->canais * sizeof(unsigned char));
    if (!imagemBorda->pixels) {
        free(imagemBorda);
        printf("Erro ao alocar memoria para os pixels da imagem de borda\n");
        return NULL;
    }

    // Define os kernels para a detecção de borda
    float KX[] = {
        -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1};

    float KY[] = {
        -1, -2, -1,
        0, 0, 0,
        1, 2, 1};

    // Percorre cada pixel da imagem
    for (int imagemY = 0; imagemY < imagem->altura; imagemY++) {
        for (int imagemX = 0; imagemX < imagem->largura; imagemX++) {
            // Processa cada canal (por exemplo, R, G, B para RGB)
            for (int canal = 0; canal < imagem->canais; canal++) {
                float valorPixelX = 0.0f;
                float valorPixelY = 0.0f;

                // Percorre cada posição no kernel
                for (int kernelY = -1; kernelY <= 1; kernelY++) {
                    for (int kernelX = -1; kernelX <= 1; kernelX++) {
                        // Calcula a posição do pixel vizinho, limitado às bordas da imagem
                        int pixelX = ajustar(imagemX + kernelX, 0, imagem->largura - 1);
                        int pixelY = ajustar(imagemY + kernelY, 0, imagem->altura - 1);

                        // Calcula a posição (índice) na imagem e no kernel
                        int indicePixel = (pixelY * imagem->largura + pixelX) * imagem->canais + canal;
                        int indiceKernel = (kernelY + 1) * 3 + (kernelX + 1);

                        valorPixelX += imagem->pixels[indicePixel] * KX[indiceKernel];
                        valorPixelY += imagem->pixels[indicePixel] * KY[indiceKernel];
                    }
                }

                // Calcula o valor do pixel e ajusta para o intervalo 0-255
                int valorPixelCalculado = ajustar((int)sqrt(valorPixelX * valorPixelX + valorPixelY * valorPixelY), 0, 255);

                // Atribui o valor do pixel na imagem de saída
                int indiceSaida = (imagemY * imagem->largura + imagemX) * imagem->canais + canal;
                imagemBorda->pixels[indiceSaida] = (unsigned char)valorPixelCalculado;
            }
        }
    }

    return imagemBorda;
}

/** @brief Faz a comparação de 2 imagens e retorna se são iguais (com tolerância de erro de 1 pixel)
 *
 * @param imagem1 A primeira imagem a ser comparada
 * @param imagem2 A segunda imagem a ser comparada
 *
 * @return Retorna verdadeiro se as imagens forem iguais, falso caso contrário
 */
bool compararImagens(const Imagem *imagem1, const Imagem *imagem2) {
    // Verifica se as imagens têm as mesmas dimensões
    if (imagem1->largura != imagem2->largura || imagem1->altura != imagem2->altura || imagem1->canais != imagem2->canais) {
        return false;
    }

    // Compara cada pixel das imagens
    for (int i = 0; i < imagem1->largura * imagem1->altura * imagem1->canais; ++i) {
        if (abs(imagem1->pixels[i] - imagem2->pixels[i]) > 1) {
            return false;
        }
    }

    return true;
}

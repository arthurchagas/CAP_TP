#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "header.h"

/*
 * Procedimento escolherCards
 * Objetivo: escolher as cards de um novo jogo
 * Parâmetros formais:
 *      Dados de Entrada:
 *              tCardPar cardS : contém as reais dimensões da matriz
 *      Dados de Saída:
 *              tInstancia cardFF[LINHAS_MAX][COLUNAS_MAX] : matriz LINHAS_MAX x COLUNAS_MAX de tInstancia preenchida com o nome das cards selecionadas "aleatoriamente"
 */
void escolherCards(tInstancia cardFF[][COLUNAS_MAX], tCardPar cardS) {
    // Controladores de Laço
    int i, j;

    // Matriz Auxiliar
    int numerosAleatorios[LINHAS_MAX][COLUNAS_MAX];

    // Números Aleatórios
    int r;                  // Número "aleatório" entre 0 e RSC_QUANTIDADE - 1
    int s, t, u, v;         // Posições "aleatórias" entre 0 e RND_MAXIMO - RND_MINIMO - 1
    int def = 0;

    // Inicializar matriz
    preencherMatriz(numerosAleatorios, -1, cardS);

    // Determinar seed de randomização
    srand((unsigned int) time(NULL));

    while (def < cardS.linhas * cardS.colunas) {
        r = rand() % RSC_QUANTIDADE;
        s = rand() % cardS.linhas;
        t = rand() % cardS.colunas;
        u = rand() % cardS.linhas;
        v = rand() % cardS.colunas;


        if (!(s == u && t == v) &&                                                  // Posições s,t e u,v não são iguais
                numerosAleatorios[s][t] == -1 && numerosAleatorios[u][v] == -1 &&   // Posições s,t e u,v não receberam um número
                !existeNaMatriz(numerosAleatorios, r, cardS)) {                     // Número r não ocorreu na matriz
            numerosAleatorios[s][t] = r;
            numerosAleatorios[u][v] = r;

            def += 2;
        }
    }

    // Preencher cards com os nomes dos arquivos
    for (i = 0; i < cardS.linhas; ++i) {
        for (j = 0; j < cardS.colunas; ++j) {
            strcpy(cardFF[i][j].nome, rsc[numerosAleatorios[i][j]]);
            cardFF[i][j].status = true;
        }
    }
}

/*
 * Função existeNaMatriz
 * Objetivo: checar se um int n existe em uma matriz de int
 * Parâmetros formais:
 *      Dados de Entrada:
 *              int matriz[LINHAS_MAX][COLUNAS_MAX] : matriz LINHAS_MAX x COLUNAS_MAX de int a ser verificada
 *              int n : número a ser encontrado na matriz
 *              tCardPar cardS : contém as reais dimensões da matriz
 *      Valor de Retorno: boolean
 *              true, se o número n existir em qualquer posição da matriz
 *              fasle, se o número n não existir em nenhuma posição da matriz
 */
boolean existeNaMatriz(int matriz[][COLUNAS_MAX], int n, tCardPar cardS) {
    int i, j;
    for (i = 0; i < cardS.linhas; ++i)
        for (j = 0; j < cardS.colunas; ++j)
            if (matriz[i][j] == n)
                return true;

    return false;
}

/*
 * Procedimento preecherMatriz
 * Objetivo: preecher todas as posições de uma matriz de inteiros com um número n
 * Parâmetros formais:
 *      Dados de Entrada:
 *              int n : número com o qual a matriz será preenchida
 *              tCardPar cardS : contém as reais dimensões da matriz
 *      Dados de Saída:
 *              int matriz[LINHAS_MAX][COLUNAS_MAX] : matriz LINHAS_MAX x COLUNAS_MAX preenchida com n
 */
void preencherMatriz(int matriz[][COLUNAS_MAX], int n, tCardPar cardS) {
    int i, j;

    for (i = 0; i < cardS.linhas; ++i)
        for (j = 0; j < cardS.colunas; ++j)
            matriz[i][j] = n;
}
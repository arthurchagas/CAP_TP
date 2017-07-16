#include <string.h>
#include <stdlib.h>

#include "header.h"

void paginaLeaderboard(int max_posicoes) {
    tPlacar placar[max_posicoes];
    int posicoes, i;

    recuperarLeaderboard(placar, &posicoes);

    if (posicoes < max_posicoes) {
        for (i = posicoes; i < max_posicoes; ++i) {
            placar[i].pontuacao = 0;
            strcpy(placar[i].nome, "---");
        }
    }

    escreverLeaderboard(placar, max_posicoes);
}

void adicionarAoLeaderboard(int max_posicoes, int contadorDeCliques, tCardPar cardS, char * nome, char code[]) {
    tPlacar placar[max_posicoes + 1];
    int posicoes, i;
    char nomeArquivo[DATA_TAMANHO + strlen(DATA_EXTENSAO)];

    recuperarLeaderboard(placar, &posicoes);

    strcpy(placar[max_posicoes].nome, nome);
    placar[max_posicoes].pontuacao = (cardS.linhas*cardS.colunas*100)/contadorDeCliques;

    if (posicoes < max_posicoes) {
        for (i = posicoes; i < max_posicoes; ++i) {
            placar[i].pontuacao = 0;
            strcpy(placar[i].nome, "---");
        }
    }

    qsort(placar, (size_t) (max_posicoes + 1), sizeof(tPlacar), &ordenarLeaderboard);

    salvarLeaderboard(placar, posicoes + 1);

    strcpy(nomeArquivo, code);
    strcat(nomeArquivo, DATA_EXTENSAO);
    remove(nomeArquivo);
}

void processarLeaderboard(int max_posicoes, int contadorDeCliques, tCardPar cardS) {
    tPlacar placar[max_posicoes];
    int posicoes;

    recuperarLeaderboard(placar, &posicoes);

    if (placar[posicoes - 1].pontuacao < (cardS.linhas*cardS.colunas*100)/contadorDeCliques) {
        pedirNomeParaLeaderboard(contadorDeCliques, cardS);
    } else {
        fimDeJogo();
    }

}

int ordenarLeaderboard(const void *s1, const void *s2) {
    tPlacar *placar1 = (tPlacar *) s1;
    tPlacar *placar2 = (tPlacar *) s2;

    if (placar1->pontuacao != placar2->pontuacao)
        return (int) (placar2->pontuacao - placar1->pontuacao);
    return (int) (placar1->pontuacao - placar2->pontuacao);
}
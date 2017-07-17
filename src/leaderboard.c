#include <string.h>
#include <stdlib.h>

#include "header.h"

/*
 * Procedimento paginaLeaderboard
 * Objetivo: preparar placar para ser exibido
 * Parâmetros formais:
 *          Dados de entrada:
 *              int max_posicoes : número máximo de posições no leaderboard
 */
void paginaLeaderboard(int max_posicoes) {
    tPlacar placar[max_posicoes];
    int posicoes;

    // Recupera leaderboard do arquivo
    recuperarLeaderboard(placar, &posicoes);

    // Preenche posições nulas
    preencherLeaderboard(posicoes, max_posicoes, placar);

    // Mostra leaderboard
    escreverLeaderboard(placar, max_posicoes);
}

/* Procedimento adicionarAoLeaderboard
 * Objetivo: adicionar um usuario ao leaderboard
 * Parâmetros formais:
 *          Dados de entrada:
 *              int max_posicoes : número máximo de posições no leaderboard
 *              int contadorDeCliques : número de rounds corridos
 *              tCardPar cardS : contém as dimensões do jogo
 *              char * nome : nome do jogador
*/
void adicionarAoLeaderboard(int max_posicoes, int contadorDeCliques, tCardPar cardS, char * nome) {
    tPlacar placar[max_posicoes + 1];
    int posicoes;

    // Recupera leaderboard do arquivo
    recuperarLeaderboard(placar, &posicoes);

    // Adiciona o novo usuário ao fim do leaderboard (posição max_posiçoes)
    strcpy(placar[max_posicoes].nome, nome);
    placar[max_posicoes].pontuacao = (cardS.linhas*cardS.colunas*100)/contadorDeCliques;

    // Preenche posições nulas
    preencherLeaderboard(posicoes, max_posicoes, placar);

    // Ordena novo leaderboard
    qsort(placar, (size_t) (max_posicoes + 1), sizeof(tPlacar), &ordenarLeaderboard);

    // Salva leaderboard
    salvarLeaderboard(placar, posicoes + 1);
}

/*
 * Procedimento preencherLeaderboard
 * Objetivo: preencher posições restantes do placar
 * Parâmetros formais:
 *          Dados de entrada:
 *              int posicoes : número de posições já preenchidas
 *              int max_posicoes : número máximo de posições no leaderboard
 *          Dados de saída:
 *              tPlacar placar[] : placar preenchido
 *
 */
void preencherLeaderboard(int posicoes, int max_posicoes, tPlacar placar[]) {
    int i;

    if (posicoes < max_posicoes) {
        for (i = posicoes; i < max_posicoes; ++i) {
            placar[i].pontuacao = 0;
            strcpy(placar[i].nome, "---");
        }
    }
}

/*
 * Procedimento processarLeaderboard
 * Objetivo: determinar se o usuário pontuou o suficiente para entrar no leaderboard
 * Parâmetros formais:
 *          Dados de entrada:
 *              int max_posicoes : número máximo de posições no leaderboard
 *              int contadorDeCliques : número de rounds corridos
 *              tCardPar cardS : contém as dimensões do jogo
 */
void processarLeaderboard(int max_posicoes, int contadorDeCliques, tCardPar cardS) {
    tPlacar placar[max_posicoes];
    int posicoes;

    // Recupera leaderboard do arquivo
    recuperarLeaderboard(placar, &posicoes);

    // Determina se a pontuação obtida é suficiente para entrar no placar
    if (posicoes < max_posicoes || (placar[posicoes - 1].pontuacao < (cardS.linhas*cardS.colunas*100)/contadorDeCliques)) {
        // É suficiente, pedir o nome do usuário para o leaderboard
        pedirNomeParaLeaderboard(contadorDeCliques, cardS);
    } else {
        // Não é suficiente, exibir mensagem de fim de jogo
        fimDeJogo();
    }

}

/*
 * Procedimento ordenarLeaderboard
 * Objetivo: função comparadora de qsort
 * Parâmetros formais:
 *      const void *s1 : posição a
 *      const void *s2 : posição b
 */
int ordenarLeaderboard(const void *s1, const void *s2) {
    tPlacar *placar1 = (tPlacar *) s1;
    tPlacar *placar2 = (tPlacar *) s2;

    if (placar1->pontuacao != placar2->pontuacao)
        return (int) (placar2->pontuacao - placar1->pontuacao);
    return (int) (placar1->pontuacao - placar2->pontuacao);
}
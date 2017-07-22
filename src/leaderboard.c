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
void paginaLeaderboard(char data_local[], char leaderboard_nome[], char data_extensao[], int max_posicoes) {
    tPlacar placar[max_posicoes];
    int posicoes;

    // Preenche posições nulas
    preencherLeaderboard(max_posicoes, placar);

    // Recupera leaderboard do arquivo
    recuperarLeaderboard(data_local, leaderboard_nome, data_extensao, placar, &posicoes);

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
 *              char code[] : código da instancia do jogo
*/
void adicionarAoLeaderboard(char data_local[], char leaderboard_nome[], char data_extensao[], int max_posicoes, int contadorDeCliques, tCardPar cardS, char * nome, char code[]) {
    tPlacar placar[max_posicoes + 1];
    int posicoes;

    // Preenche posições nulas
    preencherLeaderboard(max_posicoes, placar);

    // Recupera leaderboard do arquivo
    recuperarLeaderboard(data_local, leaderboard_nome, data_extensao, placar, &posicoes);

    // Adiciona o novo usuário ao fim do leaderboard (posição max_posiçoes)
    strcpy(placar[max_posicoes].nome, nome);
    placar[max_posicoes].pontuacao = (cardS.linhas * cardS.colunas * 100) / contadorDeCliques;
    strcpy(placar[max_posicoes].code, code);

    // Ordena novo leaderboard
    qsort(placar, (size_t) (max_posicoes + 1), sizeof(tPlacar), &ordenarLeaderboard);

    // Salva leaderboard
    salvarLeaderboard(data_local, leaderboard_nome, data_extensao, placar, posicoes + 1);
}

/*
 * Procedimento preencherLeaderboard
 * Objetivo: preencher posições restantes do placar
 * Parâmetros formais:
 *          Dados de entrada:
 *              int max_posicoes : número máximo de posições no leaderboard
 *          Dados de saída:
 *              tPlacar placar[] : placar preenchido
 *
 */
void preencherLeaderboard(int max_posicoes, tPlacar placar[]) {
    int i;

    for (i = 0; i < max_posicoes; ++i) {
        placar[i].pontuacao = 0;
        strcpy(placar[i].nome, "---");
        strcpy(placar[i].code, "-1");
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
 *              char code[] : código da instancia do jogo
 */
void processarLeaderboard(char data_local[], char leaderboard_nome[], char data_extensao[], char cgi_caminho[], char css_path[], int max_posicoes, int contadorDeCliques, tCardPar cardS, char code[]) {
    tPlacar placar[max_posicoes];
    int posicoes;

    // Recupera leaderboard do arquivo
    recuperarLeaderboard(data_local, leaderboard_nome, data_extensao, placar, &posicoes);

    // Determina se a pontuação obtida é suficiente para entrar no placar
    if (posicoes < max_posicoes || (placar[posicoes - 1].pontuacao < (cardS.linhas*cardS.colunas*100)/contadorDeCliques)) {
        // É suficiente, pedir o nome do usuário para o leaderboard
        pedirNomeParaLeaderboard(code, cgi_caminho, css_path);
    } else {
        // Não é suficiente, exibir mensagem de fim de jogo
        fimDeJogo(cgi_caminho, css_path);
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
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "src/header.h"


int main() {
    char code[DATA_TAMANHO], nome[LEADERBOARD_NOME_TAMANHO], *queryString = NULL;
    int contadorDeCliques = 0;
    tInstancia cardFF[LINHAS_MAX][COLUNAS_MAX];
    tCardPar cardS;
    boolean recarregarPagina;
    short int leaderboard;

    // Inicializar cardS
    cardS.card1.X = -1;
    cardS.card1.Y = -1;
    cardS.card2.X = -1;
    cardS.card2.Y = -1;

    printf("Content-Type:text/html;charset=UTF-8\n\n");

    // Alocar memoria para a string do method "get"
    queryString = (char *) malloc(strlen(getenv("QUERY_STRING")) + 1);

    if (queryString) {
        // Extrai informações da query_string
        analisarEntrada(getenv("QUERY_STRING"), code, &cardS, &leaderboard, nome);

        if (!leaderboard) {
            // Codigo do jogo é -1, sinalizando novo jogo
            if (!strcmp(code, "-1")) {
                novoJogo(code, cardFF, cardS);
            } else {
                // Recuperar progresso do jogo
                recuperarProgresso(DATA_LOCAL, DATA_EXTENSAO, code, cardFF, &cardS, &recarregarPagina, &contadorDeCliques);

                // Página será recarregada e reseta o par selecionado
                if (recarregarPagina) {
                    cardS.card1.X = -1;
                    cardS.card1.Y = -1;
                    cardS.card2.X = -1;
                    cardS.card2.Y = -1;
                }

                // Determina o próximo passo do jogo
                proximoPasso(code, cardFF, cardS, contadorDeCliques + !recarregarPagina, recarregarPagina);
            }
        } else {
            if (leaderboard == 2) {
                recuperarProgresso(DATA_LOCAL, DATA_EXTENSAO, code, cardFF, &cardS, &recarregarPagina,
                                   &contadorDeCliques);

                // Adiciona um novo usuário ao placar
                adicionarAoLeaderboard(LEADERBOARD_POSICOES, contadorDeCliques, cardS, nome, code);
            }

            // Mostra página do placar
            paginaLeaderboard(LEADERBOARD_POSICOES);

        }
    } else {
        printf("Erro ao alocar memoria para leitura de dados<br>");
    }

    // Libera memória alocada para a query_string
    free(queryString);

    return 0;
}

/*
 * Procedimento proximoPasso
 * Objetivo: determinar o próximo passo a ser tomado
 * Parâmetros formais:
 *      Dados de entrada:
 *              char code[] : código da instância do jogo
 *              tInstancia cardFF[][COLUNAS_MAX] : matriz contendo a imagem de cada posição
 *              tCardPar cardS : contém as informações do último par de cartas selecionado anteriormente, além da dimensão do jogo
 *              contadorDeCliques : contém a contagem de rounds
 *              boolean recarregarPagina : flag de recarregamento da página
 */
void proximoPasso(char code[], tInstancia cardFF[][COLUNAS_MAX], tCardPar cardS, int contadorDeCliques, boolean recarregarPagina) {
    // Checar pelo fim do jogo
    if (!checarStatus(cardFF, cardS)) {
        // Jogo não terminou

        if (recarregarPagina) {
            // Página foi recarregada, gerar próxima página

            escreverCorpo(code, cardFF, cardS, false, -1, false);
            salvarProgresso(DATA_LOCAL, DATA_EXTENSAO, code, cardFF, cardS, false, contadorDeCliques);
        } else {
            if (cardS.card1.X != -1 && cardS.card2.X != -1) {
                // Par de cards não é nulo

                if (!strcmp(cardFF[cardS.card1.X][cardS.card1.Y].nome, cardFF[cardS.card2.X][cardS.card2.Y].nome)) {        // Cards tem o mesmo nome de arquivo
                    if (!((cardS.card1.X == cardS.card2.X) && (cardS.card1.Y == cardS.card2.Y))) {                          // Não é a mesma posição
                        // Marcar par como concluído

                        cardFF[cardS.card1.X][cardS.card1.Y].status = false;
                        cardFF[cardS.card2.X][cardS.card2.Y].status = false;

                    } else {                                                                                                // É a mesma posição
                        // Resetar cards selecionados

                        cardS.card1.X = -1;
                        cardS.card1.Y = -1;
                        cardS.card2.X = -1;
                        cardS.card2.Y = -1;
                    }

                    escreverCorpo(code, cardFF, cardS, true, 0, false);
                } else {
                    // Par escolhido, gerar página com tag de recarregamento

                    escreverCorpo(code, cardFF, cardS, true, 1, false);
                }

                salvarProgresso(DATA_LOCAL, DATA_EXTENSAO, code, cardFF, cardS, true, contadorDeCliques);
            } else {

                // Primeira card do par selecionada
                escreverCorpo(code, cardFF, cardS, false, -1, false);
                salvarProgresso(DATA_LOCAL, DATA_EXTENSAO, code, cardFF, cardS, false, contadorDeCliques);
            }
        }
    } else {
        // Fim de jogo
        processarLeaderboard(LEADERBOARD_POSICOES, contadorDeCliques, cardS, code);
    }
}

/*
 * procedimento novoJogo
 * Objetivo: iniciar uma nova instância do jogo
 * Parâmetros formais:
 *      Dados de saída:
 *              char code[] : código da instância do jogo
 *              tInstancia cardFF[][COLUNAS_MAX] : matriz contendo a imagem de cada posição
 *              tCardPar cardS : novo tCardPar preenchido com -1 (nenhuma tCard selecionada)
 */
void novoJogo(char code[], tInstancia cardFF[][COLUNAS_MAX], tCardPar cardS) {
    // Gera novo código
    sprintf(code, "%lld", time(NULL));

    // Escolhe as cards e suas posições que estarão no jogo
    escolherCards(cardFF, cardS);

    // Primeira página
    escreverCorpo(code, cardFF, cardS, true, 3, true);
    salvarProgresso(DATA_LOCAL, DATA_EXTENSAO, code, cardFF, cardS, false, 0);
}

/*
 * Função checarStatus
 * Objetivo: checar o parâmetro status de uma matriz do tipo tInstancia
 * Parâmetros formais:
 *      Dados de Entrada:
 *              tInstancia cardFF[LINHAS_MAX][COLUNAS_MAX] : matriz LINHAS_MAX x COLUNAS_MAX de tInstancia a ser verificada
 *              tCardPar cardS : contém as reais dimensões da matriz
 *      Valor de Retorno: boolean
 *              true, se o parâmetro status de todas as posições da matriz cardFF forem false
 *              false, se o parâmetro status de qualquer posição da matriz cardFF for true
 */
boolean checarStatus(tInstancia cardFF[][COLUNAS_MAX], tCardPar cardS) {
    int i, j;

    for (i = 0; i < cardS.linhas; ++i)
        for (j = 0; j < cardS.colunas; ++j)
            if (cardFF[i][j].status)
                return false;

    return true;
}
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "src/header.h"


int main() {
    char code[DATA_TAMANHO], *dados = NULL, nome[LEADERBOARD_NOME_TAMANHO];
    int contadorDeCliques = 0;
    long int tamanho;
    tInstancia cardFF[LINHAS_MAX][COLUNAS_MAX];
    tCardPar cardS;
    boolean refresh;
    short int leaderboard;

    printf("Content-Type:text/html;charset=UTF-8\n\n");
    if (!strcmp(getenv("REQUEST_METHOD"), "POST")) {
        tamanho = strtol(getenv("CONTENT_LENGTH"), NULL, 10);
        dados = (char *) malloc((size_t) (tamanho + 1));
        if (dados) {
            fread(dados, (size_t) (tamanho + 1), 1, stdin);
            analisarEntrada(code, dados, &cardS, &leaderboard, &contadorDeCliques, nome);
            if (!leaderboard) {
                if (!strcmp(code, "-1")) {
                    novoJogo(code, cardFF, cardS);
                } else {
                    recuperarProgresso(code, cardFF, &cardS, &refresh, &contadorDeCliques);
                    proximoPasso(code, cardFF, cardS, contadorDeCliques + 1, refresh);
                }
            } else {
                if (leaderboard == 2)
                    adicionarAoLeaderboard(LEADERBOARD_POSICOES, contadorDeCliques, cardS, nome);

                paginaLeaderboard(LEADERBOARD_POSICOES);
            }
        } else {
            printf("Erro ao alocar memoria para leitura de dados<br>");
        }
    } else {
        dados = (char *) malloc(strlen(getenv("QUERY_STRING")) + 1);
        if (dados) {
            if (!sscanf(getenv("QUERY_STRING"), "code=%s", code)) {
                printf("Erro ao analisar entrada<br>");
            } else {
                if (strcmp(code, "-1")) {
                    cardS.card1.X = -1;
                    cardS.card1.Y = -1;
                    cardS.card2.X = -1;
                    cardS.card2.Y = -1;

                    recuperarProgresso(code, cardFF, &cardS, &refresh, &contadorDeCliques);
                    proximoPasso(code, cardFF, cardS, contadorDeCliques, refresh);
                }
            }
        } else {
            printf("Erro ao alocar memoria para leitura de dados<br>");
        }
    }
    free(dados);

    return 0;
}

/*
 * Procedimento proximoPasso
 * Objetivo: determinar o próximo passo a ser tomado
 * Parâmetros formais:
 *      Dados de entrada:
 *              char code[DATA_TAMANHO] : código da instância do jogo
 *              tInstancia cardFF[LINHAS_MAX][COLUNAS_MAX] : matriz LINHAS_MAX x COLUNAS_MAX de tInstancia contendo a imagem de cada posição
 *              tCardPar cardS : contém as informações do último par de cartas selecionado anteriormente
 *              contadorDeCliques : contém a contagem de cards já viradas
 *              boolean refresh : flag de recarregamento da página
 */
void proximoPasso(char code[], tInstancia cardFF[][COLUNAS_MAX], tCardPar cardS, int contadorDeCliques, boolean refresh) {
    if (!checarStatus(cardFF, cardS)) {
        if (refresh) {
            escreverCorpo(code, cardFF, cardS, false);
            salvarProgresso(code, cardFF, cardS, false, contadorDeCliques);
        } else {
            if (cardS.card1.X != -1 && cardS.card2.X != -1) {
                if (!strcmp(cardFF[cardS.card1.X][cardS.card1.Y].nome, cardFF[cardS.card2.X][cardS.card2.Y].nome) &&
                    !((cardS.card1.X == cardS.card2.X) && (cardS.card1.Y == cardS.card2.Y))) {
                    cardFF[cardS.card1.X][cardS.card1.Y].status = false;
                    cardFF[cardS.card2.X][cardS.card2.Y].status = false;
                }
                escreverCorpo(code, cardFF, cardS, true);
                salvarProgresso(code, cardFF, cardS, true, contadorDeCliques);
            } else {
                escreverCorpo(code, cardFF, cardS, false);
                salvarProgresso(code, cardFF, cardS, false, contadorDeCliques);
            }
        }
    } else {
        processarLeaderboard(LEADERBOARD_POSICOES, contadorDeCliques, cardS);
    }
}

/*
 * procedimento novoJogo
 * Objetivo: iniciar uma nova instância do jogo
 * Parâmetros formais:
 *      Dados de saída:
 *              char code[DATA_TAMANHO] : novo código da instância do jogo
 *              tInstancia cardFF[LINHAS_MAX][COLUNAS_MAX] : nova matriz LINHAS_MAX x COLUNAS_MAX de tInstancia da instância do jogo
 *              tCardPar cardS : novo tCardPar preenchido com -1 (nenhuma tCard selecionada)
 */
void novoJogo(char code[], tInstancia cardFF[][COLUNAS_MAX], tCardPar cardS) {
    cardS.card2.X = -1;
    cardS.card2.Y = -1;

    sprintf(code, "%lld", time(NULL));
    escolherCards(cardFF, cardS);
    escreverCorpo(code, cardFF, cardS, false);
    salvarProgresso(code, cardFF, cardS, false, 0);
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
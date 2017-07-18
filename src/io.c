#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "header.h"

/*
 * Procedimento analisarEntrada
 * Objetivo analisar string de entrada dados
 * Parâmetros formais:
 *      Dados de entrada:
 *              char *dados : string crua de dados
 *      Dados de saída:
 *              char code[] : código da instância do jogo extraída de *dados
 *              tCardPar *cardS : informações sobre a última tCard selecionada anteriormente e as dimensões da instância do jogo extraídas de *dados
 *              short int *leaderboard : flag para exibição do leaderboard
 *              int *contadorDeCards : contagem de rounds do jogo
 *              char nome[] : nome do usuário para o leaderboard
 */
void analisarEntrada(char *dados, char code[], tCardPar *cardS, short int *leaderboard, char nome[]) {
    char *token;

    if (strstr(dados, "Leaderboard=Leaderboard")) {             // "Leaderboard=Leaderboard"
        *leaderboard = 1;
        if (strstr(dados, "Leaderboard=Leaderboard&")) {        // "Leaderboard=Leaderboard&d=%dx%d&jogadas=%d&nome=%s
            *leaderboard = 2;

            token = strtok(dados, "&");
            while (token != NULL) {
                if (strstr(token, "nome=") != NULL)
                    sscanf(token, "nome=%s", nome);
                else if (strstr(token, "code=") != NULL)
                    sscanf(token, "code=%s", code);

                token = strtok(NULL, "&");
            }
        }
    } else {
        *leaderboard = 0;
        token = strtok(dados, "&");
        while (token != NULL) {                                 // "coordX=%d&coordY=%d&d=%dx%d&code=%s
            if (strstr(token, "coordX=") != NULL)
                sscanf(token, "coordX=%d", &cardS->card1.X);
            else if (strstr(token, "coordY=") != NULL)
                sscanf(token, "coordY=%d", &cardS->card1.Y);
            else if (strstr(token, "d=") != NULL)
                sscanf(token, "d=%dx%d", &cardS->linhas, &cardS->colunas);
            else
                sscanf(token, "code=%s", code);

            token = strtok(NULL, "&");
        }
    }
}

/*
 * Procedimento salvarProgresso
 * Objetivo: salva o progresso de uma instância do jogo
 * Parâmetros formais:
 *      Dados de entrada:
 *              char local[] : posição relativa do arquivo de dados
 *              char extensao[] : extensão do arquivo de dados
 *              char code[] : código da instância do jogo / nome do arquivo de dados
 *              tInstancia cardFF[][COLUNAS_MAX] : matriz contendo a imagem de cada posição
 *              tCardPar cardS : contém as informações do último par de cartas selecionado anteriormente, além da dimensão do jogo
 *              boolean recarregarPagina : flag de recarregamento da página
 *              int contadorDeCliques : contagem de rounds do jogo
 */
void salvarProgresso(char local[], char extensao[], char code[], tInstancia cardFF[][COLUNAS_MAX], tCardPar cardS, boolean recarregarPagina, int contadorDeCliques) {
    int i, j;
    FILE *data = NULL;
    char nomeArquivo[strlen(local) + strlen(code) + strlen(extensao) + 1];

    // Deduz o nome do arquivo com posição relativa
    strcpy(nomeArquivo, local);
    strcat(nomeArquivo, code);
    strcat(nomeArquivo, extensao);

    // Abre o arquivo para escrita
    data = fopen(nomeArquivo, "w");
    if (data != NULL) {
        // Salva informações relevantes
        fprintf(data, "card1:%d,%d&card2:%d,%d&refresh=%d&cardFlip=%d&dim=%dx%d\n", cardS.card1.X,
                cardS.card1.Y, cardS.card2.X, cardS.card2.Y, recarregarPagina, contadorDeCliques,
                cardS.linhas, cardS.colunas);
        for (i = 0; i < cardS.linhas; ++i) {
            for (j = 0; j < cardS.colunas; ++j) {
                fprintf(data, "status:%d&file:%s\n", cardFF[i][j].status, cardFF[i][j].nome);
            }
        }
        fflush(data);
        fclose(data);
    }
}

/*
 * Procedimento recuperarProgresso
 * Objetivo: recuperar o progresso de uma instância do jogo
 * Parâmetros formais:
 *      Dados de entrada:
 *          char local[] : posição relativa do arquivo de dados
 *          char extensao[] : extensão do arquivo de dados
 *          char code[] : código da instância do jogo / nome do arquivo de dados
 *      Dados de saida:
 *          tInstancia cardFF[][COLUNAS_MAX] : triz contendo a imagem de cada posição
 *          tCardPar *cardS : contém as informações do último par de cartas selecionado anteriormente, além da dimensão do jogo
 *          boolean *recarregarPagina : flag de recarregamento de arquivo
 *          int *contadorDeCliques : contém a contagem de rounds
 */
void recuperarProgresso(char local[], char extensao[], char code[], tInstancia cardFF[][COLUNAS_MAX], tCardPar *cardS, boolean *recarregarPagina, int *contadorDeCliques) {
    char aux[RSC_NOME_TAMANHO];
    int i = 0, j = 0;
    char *nomeArquivo;
    char *buffer;
    FILE *data;
    long int tamanho;
    tCardPar cardSBuffer;

    nomeArquivo = (char *) malloc(strlen(local) + strlen(code) + strlen(extensao) + 1);

    if (nomeArquivo) {
        strcpy(nomeArquivo, local);
        strcat(nomeArquivo, code);
        strcat(nomeArquivo, extensao);

        data = fopen(nomeArquivo, "rb");
        if (data) {
            tamanho = tamanhoArquivo(data);

            buffer = (char *) malloc((size_t) (tamanho + 1));
            if (buffer) {
                if (fread(buffer, (size_t) tamanho, 1, data) == 1) {
                    sscanf(buffer, "card1:%d,%d&card2:%d,%d&refresh=%d&cardFlip=%d&dim=%dx%d", &cardSBuffer.card1.X,
                           &cardSBuffer.card1.Y, &cardSBuffer.card2.X, &cardSBuffer.card2.Y, recarregarPagina,
                           contadorDeCliques,
                           &cardS->linhas, &cardS->colunas);

                    if (cardSBuffer.card1.X != -1 && cardS->card1.X != -1) {
                        cardS->card2.X = cardSBuffer.card1.X;
                        cardS->card2.Y = cardSBuffer.card1.Y;
                    }

                    char *token;
                    token = strtok(buffer, "\n");
                    while (token != NULL) {
                        if (sscanf(token, "status:%d&file:%s", &cardFF[i][j].status, aux)) {
                            if (cardFF[i][j].status)
                                strcpy(cardFF[i][j].nome, aux);
                            else
                                strcpy(cardFF[i][j].nome, RSC_OK);


                            ++j;
                            if (j >= cardS->colunas)
                                ++i, j = 0;
                        }

                        token = strtok(NULL, "\n");
                    }
                } else {
                    printf("Erro na leitura do progresso feito<br>");
                }

                free(buffer);
            } else {
                printf("Erro ao alocar memoria para leitura do progresso feito<br>");
            }

            fclose(data);
        } else {
            printf("Erro ao abrir arquivo de progresso<br>");
        }

        free(nomeArquivo);
    } else {
        printf("Erro ao alocar memoria para leitura do progresso feito<br>");
    }
}

/*
 * Procedimento escreverCorpo
 * Objetivo: escreve o conteúdo da página
 * Parâmetros formais:
 *      Dados de entrada:
 *              char code[] : código da instância do jogo
 *              tInstancia cardFF[][COLUNAS_MAX] :  matriz contendo a imagem de cada posição
 *              tCardPar cardS : contém as informações do último par de cartas selecionado anteriormente, além da dimensão do jogo
 *              boolean recarregarPagina : flag de recarregamento da página
 *              int timeout : tempo de espera antes da página ser recarregada (segundos)
 */
void escreverCorpo(char code[], tInstancia cardFF[][COLUNAS_MAX], tCardPar cardS, boolean recarregarPagina, int timeout, boolean novoJogo) {
    int i, j;

    printf("<html>\n");
    escreverCabecalho(code, recarregarPagina, timeout);
    printf("   <body>\n");

    for (i = 0; i < cardS.linhas; ++i) {
        for (j = 0; j < cardS.colunas; ++j) {
            if (novoJogo) {
                printf("      <img src=%s/%s>\n", RSC_LOCAL, cardFF[i][j].nome);
            } else {
                if (cardFF[i][j].status) {
                    if (cardS.card1.X != -1 && cardS.card2.X != -1) {
                        if ((i == cardS.card1.X && j == cardS.card1.Y) || (i == cardS.card2.X && j == cardS.card2.Y)) {
                            printf("      <img src=%s/%s>\n", RSC_LOCAL, cardFF[i][j].nome);
                        } else {
                            printf("      <img src=%s/%s>\n", RSC_LOCAL, RSC_SELECT);
                        }
                    } else {
                        if ((i == cardS.card1.X && j == cardS.card1.Y) || (i == cardS.card2.X && j == cardS.card2.Y)) {
                            printf("      <img src=%s/%s>\n", RSC_LOCAL, cardFF[i][j].nome);
                        } else {
                            printf("      <form action=\"%s\" method=\"get\">\n"
                                           "         <input type=\"hidden\" name=\"coordX\" value=\"%d\"/>\n"
                                           "         <input type=\"hidden\" name=\"coordY\" value=\"%d\"/>\n"
                                           "         <input type=\"hidden\" name=\"code\" value=\"%s\"/>\n"
                                           "         <input type=\"submit\" value=\"\" class=\"botao_quadrado\" style=\"background-image: url(%s/%s);\"/>\n"
                                           "      </form>\n", CGI_CAMINHO, i, j, code, RSC_LOCAL, RSC_SELECT);
                        }
                    }
                } else {
                    printf("      <img src=%s/%s>\n", RSC_LOCAL, RSC_OK);
                }
            }
        }
        printf("      <br>\n");
    }
    printf("   </body>\n"
           "</html>\n");
}

/*
 * Procedimento escreverCabecalho
 * Objetivo: escrever conteúdo da tag <head> da página
 * Parâmetros formais:
 *      Dados de Entrada:
 *              char code[] : código da instância do jogo
 *              boolean recarregarPagina : flag que indica se a página deve ser recarregada
 *              int timeout : tempo de espera antes da página ser recarregada (segundos)
 */
void escreverCabecalho(char code[], boolean recarregarPagina, int timeout) {
    printf("   <head>\n"
           "      <meta http-equiv=\"Content-Type\" content=\"text/html;charset=UTF-8\" />\n");
    if (recarregarPagina)
        printf("      <meta http-equiv=\"refresh\" content=\"%d; url=%s?code=%s\" />\n", timeout, CGI_CAMINHO, code);
    printf("      <link rel=\"stylesheet\" type=\"text/css\" href=\"%s/style.css\"/>\n"
           "      <title>Jogo de CAP</title>\n"
           "   </head>\n", CSS_LOCAL);
}

/*
 * Procedimento recuperarLeaderboard
 * Objetivo: recuperar o placar do jogo
 * Parâmetros formais:
 *      Dados de saida:
 *          int posicoes : quantidade de posições no placar
 *          tPlacar placar[] : placar corrente
 */
void recuperarLeaderboard(tPlacar placar[], int *posicoes) {
    FILE * arquivoLeaderboard = NULL;
    char nomeArquivo[strlen(DATA_LOCAL) + strlen(LEADERBOARD_NOME) + strlen(DATA_EXTENSAO) + 1], * buffer, * token;
    long int tamanho;

    *posicoes = 0;

    strcpy(nomeArquivo, DATA_LOCAL);
    strcat(nomeArquivo, LEADERBOARD_NOME);
    strcat(nomeArquivo, DATA_EXTENSAO);

    arquivoLeaderboard = fopen(nomeArquivo, "rb");

    if (arquivoLeaderboard != NULL) {
        tamanho = tamanhoArquivo(arquivoLeaderboard);
        buffer = (char *) malloc((size_t) (tamanho + 1));
        fread(buffer, (size_t) (tamanho), 1, arquivoLeaderboard);

        token = strtok(buffer, "\n");
        while (token != NULL) {
            sscanf(token, "Nome:%s Code:%s Pontuacao:%lf", placar[*posicoes].nome, placar[*posicoes].code, &placar[*posicoes].pontuacao);

            ++(*posicoes);
            token = strtok(NULL, "\n");
        }

        fclose(arquivoLeaderboard);
        free(buffer);
    }
}

/*
 * Procedimento salvarProgresso
 * Objetivo: salva o leaderboard do jogo
 * Parâmetros formais:
 *      Dados de entrada:
 *          int posicoes : quantidade de posições no placar
 *          tPlacar placar[] : placar corrente
 */
void salvarLeaderboard(tPlacar placar[], int posicoes) {
    int i;
    FILE *data = NULL;
    char nomeArquivo[strlen(DATA_LOCAL) + strlen(LEADERBOARD_NOME) + strlen(DATA_EXTENSAO) + 1];

    strcpy(nomeArquivo, DATA_LOCAL);
    strcat(nomeArquivo, LEADERBOARD_NOME);
    strcat(nomeArquivo, DATA_EXTENSAO);

    data = fopen(nomeArquivo, "w");
    if (data != NULL) {
        for (i = 0; i < posicoes; ++i)
            fprintf(data, "Nome:%s Code:%s Pontuacao:%lf\n", placar[i].nome, placar[i].code, placar[i].pontuacao);
    }
    fflush(data);
    fclose(data);
}

/*
 * Procedimento escreverLeaderboard
 * Objetivo: escreve o conteúdo da página de leaderboard
 * Parâmetros formais:
 *      Dados de entrada:
 *          int posicoes : quantidade de posições no placar
 *          tPlacar placar[] : placar corrente
 */
void escreverLeaderboard(tPlacar placar[], int posicoes) {
    int i;

    printf("<html>\n"
           "   <head>\n"
           "      <meta http-equiv=\"Content-Type\" content=\"text/html;charset=UTF-8\" />\n"
           "      <link rel=\"stylesheet\" type=\"text/css\" href=\"%s/style.css\"/>\n"
           "      <title>Jogo de CAP</title>\n"
           "   </head>\n"
           "   <body>\n"
           "      <table>\n"
           "         <tr>\n"
           "            <th>#</th>\n"
           "            <th>NOME</th>\n"
           "            <th>PONTUACAO</th>\n"
           "         </tr>\n",
           CSS_LOCAL);
    for (i = 0; i < posicoes; ++i) {
        printf("         <tr>\n"
               "            <td>%d</td>\n"
               "            <td>%s</td>\n"
               "            <td>%.3lf</td>\n"
               "         </tr>\n",
               i + 1, placar[i].nome, placar[i].pontuacao);
    }
    printf("      </table>\n"
           "   </body>\n"
           "</html>\n");
}

/*
 * Função tamanhoArquivo
 * Objetivo: descobrir o tamanho de um arquivo
 * Parâmetros formais:
 *      Dados de Entrada:
 *          FILE * arquivo : arquivo-alvo
 * Valor de retorno:
 *      long int : tamanho do arquivo-alvo
 */
long int tamanhoArquivo(FILE * arquivo) {
    long int tamanho;

    fseek(arquivo, 0, SEEK_END);
    tamanho = ftell(arquivo);
    rewind(arquivo);

    return tamanho;
}

/*
 * Procedimento fimDeJogo
 * Objetivo: Escrever a página de fim de jogo
*/
void fimDeJogo() {
    printf("<html>\n");

    escreverCabecalho("-1", false, -1);

    printf("   <body>\n"
           "      <div class=\"centro_absoluto\">\n"
           "         Obrigado por jogar!<br>\n"
           "         <form action=\"%s\" method=\"get\">\n"
           "            <input type=\"submit\" name=\"Leaderboard\" value=\"Leaderboard\">\n"
           "         </form>"
           "      </div>\n"
           "   </body>\n"
           "</html>\n", CGI_CAMINHO);
}

/*
 * Procedimento pedirNomeParaLeaderboard
 * Objetivo : Escrever uma página pedindo o nome para o placar
 * Parâmetros formais :
 *      Dados de entrada:
 *          char code[] : código do jogo
 */
void pedirNomeParaLeaderboard (char code[]) {
    printf("<html>\n");

    escreverCabecalho("-1", false, -1);

    printf("   <body>\n"
           "      <div class=\"centro_absoluto\">\n"
           "          Obrigado por jogar!<br>\n"
           "          Insira seu nome para o placar!<br>\n"
           "          <form action=%s method=\"get\">\n"
           "              <input type=\"hidden\" name=\"Leaderboard\" value=\"Leaderboard\">\n"
           "              <input type=\"hidden\" name=\"code\" value=\"%s\"/>\n"
           "              <input type=\"text\" name=\"nome\" maxlength=%d>\n"
           "              <input type=\"submit\">\n"
           "          </form>\n"
           "      </div>\n"
           "   </body>\n"
           "</html>\n"
            , CGI_CAMINHO, code, LEADERBOARD_NOME_TAMANHO-1);
}
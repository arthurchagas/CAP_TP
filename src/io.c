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
 *              char code[DATA_TAMANHO] : código da instância do jogo extraída de *dados
 *              tCardPar *cardS : informações sobre a última tCard selecionada anteriormente e as dimensões da instância do jogo extraídas de *dados
 */
void analisarEntrada(char code[], char *dados, tCardPar *cardS, short int *leaderboard, int * contadorDeCards, char nome[]) {
    char *token;

    if (strstr(dados, "Leaderboard=Leaderboard")) {
        *leaderboard = 1;
        if (strstr(dados, "Leaderboard=Leaderboard&")) {
            *leaderboard = 2;
            token = strtok(dados, "&");
            while (token != NULL) {
                if (strstr(token, "d=") != NULL)
                    sscanf(token, "d=%dx%d", &cardS->linhas, &cardS->colunas);
                else if (strstr(token, "jogadas=") != NULL)
                    sscanf(token, "jogadas=%d", contadorDeCards);
                else if (strstr(token, "nome=") != NULL)
                    sscanf(token, "nome=%s", nome);

                token = strtok(NULL, "&");
            }
        }
    } else {
        *leaderboard = 0;
        token = strtok(dados, "&");
        while (token != NULL) {
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
 *              char code[DATA_TAMANHO] : código da instância do jogo
 *              tInstancia cardFF[LINHAS_MAX][COLUNAS_MAX] : matriz LINHAS_MAX x COLUNAS_MAX de tInstancia a ser escrita para o arquivo
 *              tCardPar cardS : tCardPar a ser escrito para o arquivo
 *              boolean refresh : flag de refresh a ser escrita para o arquivo
 *              int contadorDeCliques : contagem de cards viradas a ser escrita para o arquivo
 */
void salvarProgresso(char code[], tInstancia cardFF[][COLUNAS_MAX], tCardPar cardS,
                     boolean refresh, int contadorDeCliques) {
    int i, j;
    FILE *data = NULL;
    char *nomeArquivo = NULL;

    nomeArquivo = (char *) malloc(strlen(DATA_LOCAL) + strlen(code) + strlen(DATA_EXTENSAO) + 1);

    if (nomeArquivo != NULL) {
        strcpy(nomeArquivo, DATA_LOCAL);
        strcat(nomeArquivo, code);
        strcat(nomeArquivo, DATA_EXTENSAO);

        data = fopen(nomeArquivo, "w");
        if (data != NULL) {
            fprintf(data, "card1:%d,%d&card2:%d,%d&refresh=%d&cardFlip=%d&dim=%dx%d\n", cardS.card1.X, cardS.card1.Y,
                    cardS.card2.X, cardS.card2.Y, refresh, contadorDeCliques, cardS.linhas, cardS.colunas);
            for (i = 0; i < cardS.linhas; ++i) {
                for (j = 0; j < cardS.colunas; ++j) {
                    fprintf(data, "status:%d&file:%s\n", cardFF[i][j].status, cardFF[i][j].nome);
                }
            }
        }
        fflush(data);
        fclose(data);
    }
    free(nomeArquivo);
}

/*
 * Procedimento recuperarProgresso
 * Objetivo: recuperar o progresso de uma instância do jogo
 * Parâmetros formais:
 *      Dados de entrada:
 *          char code[DATA_TAMANHO] : código da instância do jogo
 *      Dados de saida:
 *          tInstancia cardFF[LINHAS_MAX][COLUNAS_MAX] : matriz LINHAS_MAX x COLUNAS_MAX de tInstancia recuperada do arquivo de progresso
 *          tCardPar *cardS : tCardPar recuperado do arquivo de progresso
 *          boolean *refresh : flag de refresh recuperado do arquivo de progresso
 *          int *contadorDeCliques : contagem de cards viradas recuperada do arquivo de progresso
 */
void recuperarProgresso(char code[], tInstancia cardFF[][COLUNAS_MAX], tCardPar *cardS,
                        boolean *refresh, int *contadorDeCliques) {
    char aux[RSC_NOME_TAMANHO + RSC_EXTENSAO_TAMANHO];
    int i = 0, j = 0;
    char *nomeArquivo;
    char *buffer;
    FILE *data;
    long int tamanho;
    tCardPar cardSBuffer;

    nomeArquivo = (char *) malloc(strlen(DATA_LOCAL) + strlen(code) + strlen(DATA_EXTENSAO) + 1);

    strcpy(nomeArquivo, DATA_LOCAL);
    strcat(nomeArquivo, code);
    strcat(nomeArquivo, DATA_EXTENSAO);

    data = fopen(nomeArquivo, "rb");
    if (data) {
        tamanho = tamanhoArquivo(data);

        buffer = (char *) malloc((size_t) (tamanho + 1));
        if (buffer) {
            if (fread(buffer, (size_t) tamanho, 1, data) == 1) {
                sscanf(buffer, "card1:%d,%d&card2:%d,%d&refresh=%d&cardFlip=%d&dim=%dx%d", &(cardSBuffer.card1.X),
                       &cardSBuffer.card1.Y,
                       &cardSBuffer.card2.X, &cardSBuffer.card2.Y, refresh, contadorDeCliques, &cardS->linhas, &cardS->colunas);

                if (cardSBuffer.card1.X == -1 && (*cardS).card1.X != -1) {
                    cardS->card2.X = -1;
                    cardS->card2.Y = -1;
                }

                if (cardSBuffer.card1.X != -1 && (*cardS).card1.X != -1) {
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
        } else {
            printf("Erro ao alocar memoria para leitura do progresso feito<br>");
        }
        free(buffer);
    } else {
        printf("Erro ao abrir arquivo de progresso<br>");
    }
    fclose(data);
    free(nomeArquivo);
}

/*
 * Procedimento escreverCorpo
 * Objetivo: escreve o conteúdo da página
 * Parâmetros formais:
 *      Dados de entrada:
 *              char code[DATA_TAMANHO] : código da instância do jogo
 *              tInstancia cardFF[LINHAS_MAX][COLUNAS_MAX] :  matriz LINHAS_MAX x COLUNAS_MAX de tInstancia contendo os dados de cada tCard
 *              tCardPar cardS : contém informações sobre o tamanho real da matriz e o último par de cards selecionado anteriormente
 *              boolean refresh : parâmetro a ser repassado para escreverCabeçalho
 */
void escreverCorpo(char code[], tInstancia cardFF[][COLUNAS_MAX], tCardPar cardS, boolean refresh) {
    int i, j;

    printf("<html>");
    escreverCabecalho(code, refresh);
    printf("<body>");

    for (i = 0; i < cardS.linhas; ++i) {
        for (j = 0; j < cardS.colunas; ++j) {
            if (cardFF[i][j].status) {
                if (cardS.card1.X != -1 && cardS.card2.X != -1) {
                    if ((i == cardS.card1.X && j == cardS.card1.Y) || (i == cardS.card2.X && j == cardS.card2.Y)) {
                        printf("<img src=%s/%s>", RSC_LOCAL, cardFF[i][j].nome);
                    } else {
                        printf("<img src=%s/%s>", RSC_LOCAL, RSC_SELECT);
                    }
                } else {
                    printf("<form action=\"%s\" method=\"post\">", CGI_CAMINHO);
                    printf("<input type=\"hidden\" name=\"coordX\" value=\"%d\"/>", i);
                    printf("<input type=\"hidden\" name=\"coordY\" value=\"%d\"/>", j);
                    printf("<input type=\"hidden\" name=\"d\" value=\"%dx%d\"/>", cardS.linhas, cardS.colunas);
                    printf("<input type=\"hidden\" name=\"code\" value=\"%s\"/>", code);
                    if ((i == cardS.card1.X && j == cardS.card1.Y) || (i == cardS.card2.X && j == cardS.card2.Y)) {
                        printf("<img src=%s/%s>", RSC_LOCAL, cardFF[i][j].nome);
                    } else {
                        printf("<input type=\"submit\" value=\"\" class=\"botao_quadrado\" style=\"background-image: url(%s/%s);\"/>",
                               RSC_LOCAL, RSC_SELECT);
                    }
                    printf("</form>");
                }
            } else {
                printf("<img src=%s/%s>", RSC_LOCAL, RSC_OK);
            }
        }
        printf("<br>");
    }
    printf("</body>");
    printf("</html>");
}

/*
 * Procedimento escreverCabecalho
 * Objetivo: escrever conteúdo da tag <head> da página
 * Parâmetros formais:
 *      Dados de Entrada:
 *              char code[DATA_TAMANHO] : código da instância do jogo
 *              boolean refresh : flag que indica se a página deve ser recarregada após 1 segundo
 */
void escreverCabecalho(char code[], boolean refresh) {
    printf("<head>");
    printf("<meta http-equiv=\"Content-Type\" content=\"text/html;charset=UTF-8\" />");
    if (refresh)
        printf("<meta http-equiv=\"refresh\" content=\"1; url=%s?code=%s\" />", CGI_CAMINHO, code);
    printf("<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/style.css\"/>", CSS_LOCAL);
    printf("<title>Jogo de CAP</title>");
    printf("</head>");
}

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
            sscanf(token, "Nome:%s Pontuacao:%lf", placar[*posicoes].nome, &placar[*posicoes].pontuacao);

            ++(*posicoes);
            token = strtok(NULL, "\n");
        }

        fclose(arquivoLeaderboard);
    }
}

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
            fprintf(data, "Nome:%s Pontuacao:%lf\n", placar[i].nome, placar[i].pontuacao);
    }
    fflush(data);
    fclose(data);
}

void escreverLeaderboard(tPlacar placar[], int posicoes) {
    int i;

    printf("<html>");
    printf("<head>");
    printf("<meta http-equiv=\"Content-Type\" content=\"text/html;charset=UTF-8\" />");
    printf("<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/style.css\"/>", CSS_LOCAL);
    printf("<title>Jogo de CAP</title>");
    printf("</head>");
    printf("<body>");
    printf("<table>");
    printf("<tr>");
    printf("<th>#</th>");
    printf("<th>NOME</th>");
    printf("<th>PONTUACAO</th>");
    printf("</tr>");
    for (i = 0; i < posicoes; ++i) {
        printf("<tr><td>%d</td><td>%s</td><td>%.3lf</td></tr>", i + 1, placar[i].nome, placar[i].pontuacao);
    }
    printf("</table>");
    printf("</body>");
    printf("</html>");
}

long int tamanhoArquivo(FILE * arquivo) {
    long int tamanho;

    fseek(arquivo, 0, SEEK_END);
    tamanho = ftell(arquivo);
    fseek(arquivo, 0, SEEK_SET);

    return tamanho;
}

void fimDeJogo() {
    printf("<html>");
    printf("<head>");
    printf("<meta http-equiv=\"Content-Type\" content=\"text/html;charset=UTF-8\" />");
    printf("<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/style.css\"/>", CSS_LOCAL);
    printf("<title>Jogo de CAP</title>");
    printf("</head>");
    printf("<body>");
    printf("<div class=\"centro_absoluto\">");
    printf("<a>Obrigado por jogar!</a>");
    printf("</div>");
    printf("</body>");
    printf("</html>");
}

void pedirNomeParaLeaderboard (int contadorDeCliques, tCardPar cardS) {
    printf("<html>");
    printf("<head>");
    printf("<meta http-equiv=\"Content-Type\" content=\"text/html;charset=UTF-8\" />");
    printf("<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/style.css\"/>", CSS_LOCAL);
    printf("<title>Jogo de CAP</title>");
    printf("</head>");
    printf("<body>");
    printf("<div class=\"centro_absoluto\">");
    printf("Obrigado por jogar!<br>Insira seu nome para o placar!<br>");
    printf("<form action=%s method=\"post\">", CGI_CAMINHO);
    printf("<input type=\"hidden\" name=\"Leaderboard\" value=\"Leaderboard\">");
    printf("<input type=\"hidden\" name=\"d\" value=\"%dx%d\">", cardS.linhas, cardS.colunas);
    printf("<input type=\"hidden\" name=\"jogadas\" value=\"%d\">", contadorDeCliques);
    printf("<input type=\"text\" name=\"nome\" maxlength=%d>", LEADERBOARD_NOME_TAMANHO-1);
    printf("<input type=\"submit\">");
    printf("</form>");
    printf("</div>");
    printf("</body>");
    printf("</html>");
}
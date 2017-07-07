#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define LINHAS_MAX 7
#define COLUNAS_MAX 10
#define RSC_EXTENSAO ".png"
#define RSC_EXTENSAO_TAMANHO 4
#define RSC_LOCAL "../memoria/resources"
#define RSC_QUANTIDADE 24
#define RSC_NOME_TAMANHO 5
#define RSC_OK "done.png"
#define RSC_SELECT "mmry.png"
#define CSS_LOCAL "../memoria/css"
#define SRV_CAMINHO "http://cap.dc.ufscar.br/~744344/cgi-bin/memoria.cgi"
#define DATA_TAMANHO 20
#define DATA_LOCAL "../memoria/dados/"
#define DATA_EXTENSAO ".dat"

static const char rsc[RSC_QUANTIDADE][RSC_NOME_TAMANHO] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11",
                                                           "12", "13", "14", "15", "16", "17", "18", "19", "20", "21",
                                                           "22", "23", "24"};

typedef int boolean;
enum {
    false, true
};

typedef struct {
    char nome[RSC_NOME_TAMANHO + RSC_EXTENSAO_TAMANHO];
    boolean status;
} cardFromFile;

typedef struct {
    int X;
    int Y;
} card;

typedef struct {
    card card1;
    card card2;
} cardSelecionado;

boolean existeNaMatriz(int [LINHAS_MAX][COLUNAS_MAX], int, int[2]);
boolean terminou(cardFromFile [LINHAS_MAX][COLUNAS_MAX], int[2]);
void escolherCards(cardFromFile [LINHAS_MAX][COLUNAS_MAX], int[2]);
void preencherMatriz(int [LINHAS_MAX][COLUNAS_MAX], int, int[2]);
void novoJogo(char [DATA_TAMANHO], cardSelecionado, cardFromFile [LINHAS_MAX][COLUNAS_MAX], int [2]);
void escreverCorpo(char [DATA_TAMANHO], cardFromFile [LINHAS_MAX][COLUNAS_MAX], cardSelecionado, boolean, int[2]);
void salvarProgresso(char [DATA_TAMANHO], cardFromFile [LINHAS_MAX][COLUNAS_MAX], cardSelecionado cardS, boolean, int, int [2]);
void recuperarProgresso(char [DATA_TAMANHO], cardFromFile [LINHAS_MAX][COLUNAS_MAX], cardSelecionado *, boolean *, int *, int [2]);
void proximoPasso(char [DATA_TAMANHO], cardFromFile [LINHAS_MAX][COLUNAS_MAX], cardSelecionado, boolean, int, int [2]);
void escreverCabecalho(char [DATA_TAMANHO], boolean);
void parseInput(char *, cardSelecionado *, int [2], char [DATA_TAMANHO]);

int main() {
    char code[DATA_TAMANHO];
    char *dados = NULL;
    long int tamanho;
    cardFromFile cardFF[LINHAS_MAX][COLUNAS_MAX];
    cardSelecionado cardS;
    boolean refresh;
    int cardFlip;
    int dimensoes[2];

    printf("Content-Type:text/html;charset=UTF-8\n\n");
    if (!strcmp(getenv("REQUEST_METHOD"), "POST")) {
        tamanho = strtol(getenv("CONTENT_LENGTH"), NULL, 10);
        dados = (char *) malloc((size_t) (tamanho + 1));
        if (dados) {
            fread(dados, (size_t) (tamanho + 1), 1, stdin);
            parseInput(dados, &cardS, dimensoes, code);
            if (!strcmp(code, "-1")) {
                novoJogo(code, cardS, cardFF, dimensoes);
            } else {
                recuperarProgresso(code, cardFF, &cardS, &refresh, &cardFlip, dimensoes);
                proximoPasso(code, cardFF, cardS, refresh, cardFlip + 1, dimensoes);
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

                    recuperarProgresso(code, cardFF, &cardS, &refresh, &cardFlip, dimensoes);
                    proximoPasso(code, cardFF, cardS, refresh, cardFlip, dimensoes);
                }
            }
        } else {
            printf("Erro ao alocar memoria para leitura de dados<br>");
        }
    }
    free(dados);

    return 0;
}

void parseInput(char * dados, cardSelecionado * cardS, int dimensoes[2], char code[DATA_TAMANHO]) {
    char * token;

    token = strtok(dados, "&");
    while (token != NULL) {
        if (strstr(token, "coordX=") != NULL)
            sscanf(token, "coordX=%d", &cardS->card1.X);
        else if (strstr(token, "coordY=") != NULL)
            sscanf(token, "coordY=%d", &cardS->card1.Y);
        else if (strstr(token, "d=") != NULL)
            sscanf(token, "d=%dx%d", &dimensoes[0], &dimensoes[1]);
        else
            sscanf(token, "code=%s", code);

        token = strtok(NULL, "&");
    }
}

void proximoPasso(char code[DATA_TAMANHO], cardFromFile cardFF[LINHAS_MAX][COLUNAS_MAX], cardSelecionado cardS, boolean refresh, int cardFlip, int dimensoes[2]) {
    if (!terminou(cardFF, dimensoes)) {
        if (refresh) {
            escreverCorpo(code, cardFF, cardS, false, dimensoes);
            salvarProgresso(code, cardFF, cardS, false, cardFlip, dimensoes);
        } else {
            if (cardS.card1.X != -1 && cardS.card2.X != -1) {
                if (!strcmp(cardFF[cardS.card1.X][cardS.card1.Y].nome, cardFF[cardS.card2.X][cardS.card2.Y].nome) &&
                    !((cardS.card1.X == cardS.card2.X) && (cardS.card1.Y == cardS.card2.Y))) {
                    cardFF[cardS.card1.X][cardS.card1.Y].status = false;
                    cardFF[cardS.card2.X][cardS.card2.Y].status = false;
                }
                escreverCorpo(code, cardFF, cardS, true, dimensoes);
                salvarProgresso(code, cardFF, cardS, true, cardFlip, dimensoes);
            } else {
                escreverCorpo(code, cardFF, cardS, false, dimensoes);
                salvarProgresso(code, cardFF, cardS, false, cardFlip, dimensoes);
            }
        }
    } else {
        printf("Fim!<br>");
    }
}

void novoJogo(char code[DATA_TAMANHO], cardSelecionado cardS, cardFromFile cardFF[LINHAS_MAX][COLUNAS_MAX], int dimensoes[2]) {
    cardS.card2.X = -1;
    cardS.card2.Y = -1;

    sprintf(code, "%lld", time(NULL));
    escolherCards(cardFF, dimensoes);
    escreverCorpo(code, cardFF, cardS, false, dimensoes);
    salvarProgresso(code, cardFF, cardS, false, 0, dimensoes);
}

void salvarProgresso(char code[DATA_TAMANHO], cardFromFile cardFF[LINHAS_MAX][COLUNAS_MAX], cardSelecionado cardS, boolean refresh, int cardFlip, int dimensoes[2]) {
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
            fprintf(data, "card1:%d,%d&card2:%d,%d&refresh=%d&cardFlip=%d&dim=%dx%d\n", cardS.card1.X, cardS.card1.Y, cardS.card2.X, cardS.card2.Y, refresh, cardFlip, dimensoes[0], dimensoes[1]);
            for (i = 0; i < dimensoes[0]; ++i) {
                for (j = 0; j < dimensoes[1]; ++j) {
                    fprintf(data, "status:%d&file:%s\n", cardFF[i][j].status, cardFF[i][j].nome);
                }
            }
        }
        fflush(data);
        fclose(data);
    }
    free(nomeArquivo);
}

void recuperarProgresso(char code[DATA_TAMANHO], cardFromFile cardFF[LINHAS_MAX][COLUNAS_MAX], cardSelecionado *cardS, boolean *refresh, int * cardFlip, int dimensoes[2]) {
    char aux[RSC_NOME_TAMANHO + RSC_EXTENSAO_TAMANHO];
    int i = 0, j = 0;
    char *nomeArquivo;
    char *buffer;
    FILE *data;
    long int tamanhoArquivo;
    cardSelecionado cardSBuffer;

    nomeArquivo = (char *) malloc(strlen(DATA_LOCAL) + strlen(code) + strlen(DATA_EXTENSAO) + 1);

    strcpy(nomeArquivo, DATA_LOCAL);
    strcat(nomeArquivo, code);
    strcat(nomeArquivo, DATA_EXTENSAO);

    data = fopen(nomeArquivo, "rb");
    if (data) {
        fseek(data, 0, SEEK_END);
        tamanhoArquivo = ftell(data);
        fseek(data, 0, SEEK_SET);

        buffer = (char *) malloc((size_t) (tamanhoArquivo + 1));
        if (buffer) {
            if (fread(buffer, (size_t) tamanhoArquivo, 1, data) == 1) {
                sscanf(buffer, "card1:%d,%d&card2:%d,%d&refresh=%d&cardFlip=%d&dim=%dx%d", &(cardSBuffer.card1.X), &(cardSBuffer.card1.Y),
                       &(cardSBuffer.card2.X), &(cardSBuffer.card2.Y), refresh, cardFlip, &dimensoes[0], &dimensoes[1]);

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
                        if (j >= dimensoes[1])
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

void escreverCorpo(char code[DATA_TAMANHO], cardFromFile cardFF[LINHAS_MAX][COLUNAS_MAX], cardSelecionado cardS, boolean refresh, int dimensoes[2]) {
    int i, j;

    printf("<html>");
    escreverCabecalho(code, refresh);
    printf("<body>");
    printf("<div id=centralizar>");
    printf("<a>");

    for (i = 0; i < dimensoes[0]; ++i) {
        for (j = 0; j < dimensoes[1]; ++j) {
            if (cardFF[i][j].status) {
                if (cardS.card1.X != -1 && cardS.card2.X != -1) {
                    if ((i == cardS.card1.X && j == cardS.card1.Y) || (i == cardS.card2.X && j == cardS.card2.Y)) {
                        printf("<img src=%s/%s>", RSC_LOCAL, cardFF[i][j].nome);
                    } else {
                        printf("<img src=%s/%s>", RSC_LOCAL, RSC_SELECT);
                    }
                } else {
                    printf("<form action=\"%s\" method=\"post\">", SRV_CAMINHO);
                    printf("<input type=\"hidden\" name=\"coordX\" value=\"%d\"/>", i);
                    printf("<input type=\"hidden\" name=\"coordY\" value=\"%d\"/>", j);
                    printf("<input type=\"hidden\" name=\"d\" value=\"%dx%d\"/>", dimensoes[0], dimensoes[1]);
                    printf("<input type=\"hidden\" name=\"code\" value=\"%s\"/>", code);
                    if ((i == cardS.card1.X && j == cardS.card1.Y) || (i == cardS.card2.X && j == cardS.card2.Y)) {
                        printf("<img src=%s/%s>", RSC_LOCAL, cardFF[i][j].nome);
                    } else {
                        printf("<input type=\"submit\" value=\"\" style=\"background-image: url(%s/%s);\"/>",
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
    printf("</a>");
    printf("</div>");
    printf("</body>");
    printf("</html>");
}

void escreverCabecalho(char code[DATA_TAMANHO], boolean refresh) {
    printf("<head>");
    printf("<meta http-equiv=\"Content-Type\" content=\"text/html;charset=UTF-8\" />");
    if (refresh)
        printf("<meta http-equiv=\"refresh\" content=\"1; url=%s?code=%s\" />", SRV_CAMINHO, code);
    printf("<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/style.css\"/>", CSS_LOCAL);
    printf("<title>Jogo de CAP</title>");
    printf("</head>");
}

void escolherCards(cardFromFile cardFF[LINHAS_MAX][COLUNAS_MAX], int dimensoes[2]) {
    // Controladores de Laço
    int i, j;
    boolean continuar = true;

    // Matriz Auxiliar
    int numerosAleatorios[LINHAS_MAX][COLUNAS_MAX];

    // Números Aleatórios
    int r;                  // Número "aleatório" entre 0 e RSC_QUANTIDADE - 1
    int s, t, u, v;         // Posições "aleatórias" entre 0 e RND_MAXIMO - RND_MINIMO - 1

    // Inicializar matriz
    preencherMatriz(numerosAleatorios, -1, dimensoes);

    // Determinar seed de randomização
    srand((unsigned int) time(NULL));

    while (continuar) {
        r = rand() % RSC_QUANTIDADE;
        s = rand() % dimensoes[0];
        t = rand() % dimensoes[1];
        u = rand() % dimensoes[0];
        v = rand() % dimensoes[1];


        // Posições s,t e u,v não são iguais
        if (!(s == u && t == v)) {
            // Posições s,t e u,v não receberam um número
            if (numerosAleatorios[s][t] == -1 && numerosAleatorios[u][v] == -1) {
                // Número r não ocorreu na matriz
                if (!existeNaMatriz(numerosAleatorios, r, dimensoes)) {
                    numerosAleatorios[s][t] = r;
                    numerosAleatorios[u][v] = r;
                }
            } else {
                // Verificar se ainda existe alguma posição que não recebeu um número
                continuar = existeNaMatriz(numerosAleatorios, -1, dimensoes);
            }
        }
    }

    // Preencher cards com os nomes dos arquivos
    for (i = 0; i < dimensoes[0]; ++i) {
        for (j = 0; j < dimensoes[1]; ++j) {
            strcpy(cardFF[i][j].nome, rsc[numerosAleatorios[i][j]]);
            strcat(cardFF[i][j].nome, RSC_EXTENSAO);
            cardFF[i][j].status = true;
        }
    }
}

boolean existeNaMatriz(int matriz[LINHAS_MAX][COLUNAS_MAX], int n, int dimensoes[2]) {
    int i, j;
    for (i = 0; i < dimensoes[0]; ++i)
        for (j = 0; j < dimensoes[1]; ++j)
            if (matriz[i][j] == n)
                return true;

    return false;
}

boolean terminou(cardFromFile cardFF[LINHAS_MAX][COLUNAS_MAX], int dimensoes[2]) {
    int i, j;

    for (i = 0; i < dimensoes[0]; ++i)
        for (j = 0; j < dimensoes[1]; ++j)
            if (cardFF[i][j].status)
                return false;

    return true;
}

void preencherMatriz(int matriz[LINHAS_MAX][COLUNAS_MAX], int n, int dimensoes[2]) {
    int i, j;

    for (i = 0; i < dimensoes[0]; ++i)
        for (j = 0; j < dimensoes[1]; ++j)
            matriz[i][j] = n;
}
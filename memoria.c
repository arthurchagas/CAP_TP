#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define LINHAS 4
#define COLUNAS 4
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

boolean existeNaMatriz(int [LINHAS][COLUNAS], int);
void escolherCards(cardFromFile [LINHAS][COLUNAS]);
void preencherMatriz(int [LINHAS][COLUNAS], int);
void novoJogo(char [DATA_TAMANHO], cardFromFile [LINHAS][COLUNAS]);
void escreverCorpo(char [DATA_TAMANHO], cardFromFile [LINHAS][COLUNAS], cardSelecionado);
void salvarProgresso(char [DATA_TAMANHO], cardFromFile [LINHAS][COLUNAS], cardSelecionado);
void recuperarProgresso(char [DATA_TAMANHO], cardFromFile [LINHAS][COLUNAS], cardSelecionado *);
void proximoPasso(char [DATA_TAMANHO], cardFromFile [LINHAS][COLUNAS], cardSelecionado);
void escreverCabecalho();

int main() {
    char code[DATA_TAMANHO];
    char *dados = NULL;
    long int tamanho = strtol(getenv("CONTENT_LENGTH"), NULL, 10);
    cardFromFile cardFF[LINHAS][COLUNAS];
    cardSelecionado cardS;

    dados = (char *) malloc((size_t) (tamanho + 1));
    if (dados) {
        fgets(dados, tamanho + 1, stdin);
        printf("Content-Type:text/html;charset=UTF-8\n\n");
        if (!sscanf(dados, "coordX=%d&coordY=%d&code=%s", &cardS.card1.X, &cardS.card1.Y, code)) {
            printf("Erro ao analisar entrada<br>");
        } else {
            if (!strcmp(code, "-1")) {
                novoJogo(code, cardFF);
            } else {
                recuperarProgresso(code, cardFF, &cardS);
                proximoPasso(code, cardFF, cardS);
            }
        }
    } else {
        printf("Erro ao alocar memoria para leitura de dados<br>");
    }
    free(dados);

    return 0;
}

void proximoPasso(char code[DATA_TAMANHO], cardFromFile cardFF[LINHAS][COLUNAS], cardSelecionado cardS) {
    if (cardS.card1.X != -1 && cardS.card2.X != -1) {
        if (!strcmp(cardFF[cardS.card1.X][cardS.card1.Y].nome, cardFF[cardS.card2.X][cardS.card2.Y].nome)) {
            cardFF[cardS.card1.X][cardS.card1.Y].status = false;
            cardFF[cardS.card2.X][cardS.card2.Y].status = false;
        }

        cardS.card1.X = -1;
        cardS.card1.Y = -1;
        cardS.card2.X = -1;
        cardS.card2.Y = -1;
    }

    escreverCorpo(code, cardFF, cardS);
    salvarProgresso(code, cardFF, cardS);
}

void novoJogo(char code[DATA_TAMANHO], cardFromFile cardFF[LINHAS][COLUNAS]) {
    cardSelecionado cardS;

    cardS.card1.X = -1;
    cardS.card1.Y = -1;
    cardS.card2.X = -1;
    cardS.card2.Y = -1;

    sprintf(code, "%lld", time(NULL));
    escolherCards(cardFF);
    escreverCorpo(code, cardFF, cardS);
    salvarProgresso(code, cardFF, cardS);
}

void salvarProgresso(char code[DATA_TAMANHO], cardFromFile cardFF[LINHAS][COLUNAS], cardSelecionado cardS) {
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
            fprintf(data, "card1:%d,%d&card2:%d,%d\n", cardS.card1.X, cardS.card1.Y, cardS.card2.X, cardS.card2.Y);
            for (i = 0; i < LINHAS; ++i) {
                for (j = 0; j < COLUNAS; ++j) {
                    fprintf(data, "status:%d&file:%s\n", cardFF[i][j].status, cardFF[i][j].nome);
                }
            }
        }
        fflush(data);
        fclose(data);
    }
    free(nomeArquivo);
}

void recuperarProgresso(char code[DATA_TAMANHO], cardFromFile cardFF[LINHAS][COLUNAS], cardSelecionado *cardS) {
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
                sscanf(buffer, "card1:%d,%d&card2:%d,%d", &(cardSBuffer.card1.X), &(cardSBuffer.card1.Y),
                       &(cardSBuffer.card2.X), &(cardSBuffer.card2.Y));

                if (cardSBuffer.card1.X == -1 && (*cardS).card1.X != -1) {
                    (*cardS).card2.X = -1;
                    (*cardS).card2.Y = -1;
                }

                if (cardSBuffer.card1.X != -1 && (*cardS).card1.X != -1) {
                    (*cardS).card2.X = cardSBuffer.card1.X;
                    (*cardS).card2.Y = cardSBuffer.card1.Y;
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
                        if (j >= COLUNAS)
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

void escreverCorpo(char code[DATA_TAMANHO], cardFromFile cardFF[LINHAS][COLUNAS], cardSelecionado cardS) {
    int i, j;

    printf("<html>");
    escreverCabecalho();
    printf("<body>");
    printf("<a>");

    for (i = 0; i < LINHAS; ++i) {
        for (j = 0; j < COLUNAS; ++j) {
            if (cardFF[i][j].status) {
                if (!(i == cardS.card1.X && j == cardS.card1.Y) || (i == cardS.card2.X && j == cardS.card2.Y)) {
                    printf("<form action=\"%s\" method=\"post\">", SRV_CAMINHO);
                    printf("<input type=\"hidden\" name=\"coordX\" value=\"%d\">", i);
                    printf("<input type=\"hidden\" name=\"coordY\" value=\"%d\">", j);
                    printf("<input type=\"hidden\" name=\"code\" value=\"%s\">", code);
                    printf("<input type=\"submit\" value=\"\" style=\"background-image: url(%s/%s); border: solid 0px #000000; width: 160px; height: 160px;\"/>",
                           RSC_LOCAL, RSC_SELECT);
                    printf("</form>");
                } else {
                    printf("<img src=%s/%s style=\"border: solid 0px #000000; width: 160px; height: 160px;\"/>",
                           RSC_LOCAL, cardFF[i][j].nome);
                }
            } else {
                printf("<img src=%s/%s style=\"border: solid 0px #000000; width: 160px; height: 160px;\"/>",
                           RSC_LOCAL, RSC_OK);
            }
        }
    }
    printf("</a>");
    printf("</body>");
    printf("</html>");
}

void escreverCabecalho() {
    printf("<head>"),
            printf("<meta http-equiv=\"Content-Type\" content=\"text/html;charset=UTF-8\">"),
            printf("<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/style.css\">", CSS_LOCAL),
            printf("<title>Jogo de CAP</title>"),
            printf("</head>");
}

void escolherCards(cardFromFile cardFF[LINHAS][COLUNAS]) {
    // Controladores de Laço
    int i, j;
    boolean continuar = true;

    // Matriz Auxiliar
    int numerosAleatorios[LINHAS][COLUNAS];

    // Números Aleatórios
    int r;                  // Número "aleatório" entre 0 e RSC_QUANTIDADE - 1
    int s, t, u, v;         // Posições "aleatórias" entre 0 e RND_MAXIMO - RND_MINIMO - 1

    // Inicializar matriz
    preencherMatriz(numerosAleatorios, -1);

    // Determinar seed de randomização
    srand((unsigned int) time(NULL));

    while (continuar) {
        r = rand() % RSC_QUANTIDADE;
        s = rand() % LINHAS;
        t = rand() % COLUNAS;
        u = rand() % LINHAS;
        v = rand() % COLUNAS;


        // Posições s,t e u,v não são iguais
        if (!(s == u && t == v)) {
            // Posições s,t e u,v não receberam um número
            if (numerosAleatorios[s][t] == -1 && numerosAleatorios[u][v] == -1) {
                // Número r não ocorreu na matriz
                if (!existeNaMatriz(numerosAleatorios, r)) {
                    numerosAleatorios[s][t] = r;
                    numerosAleatorios[u][v] = r;
                }
            } else {
                // Verificar se ainda existe alguma posição que não recebeu um número
                continuar = existeNaMatriz(numerosAleatorios, -1);
            }
        }
    }

    // Preencher cards com os nomes dos arquivos
    for (i = 0; i < LINHAS; ++i) {
        for (j = 0; j < COLUNAS; ++j) {
            strcpy(cardFF[i][j].nome, rsc[numerosAleatorios[i][j]]);
            strcat(cardFF[i][j].nome, RSC_EXTENSAO);
            cardFF[i][j].status = true;
        }
    }
}

boolean existeNaMatriz(int matriz[LINHAS][COLUNAS], int n) {
    int i, j;
    for (i = 0; i < LINHAS; ++i)
        for (j = 0; j < COLUNAS; ++j)
            if (matriz[i][j] == n)
                return true;

    return false;
}

void preencherMatriz(int matriz[LINHAS][COLUNAS], int n) {
    int i, j;

    for (i = 0; i < LINHAS; ++i)
        for (j = 0; j < COLUNAS; ++j)
            matriz[i][j] = n;
}
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
#define CGI_CAMINHO "http://cap.dc.ufscar.br/~744344/cgi-bin/memoria.cgi"
#define DATA_TAMANHO 20
#define DATA_LOCAL "../memoria/dados/"
#define DATA_EXTENSAO ".dat"

static const char rsc[RSC_QUANTIDADE][RSC_NOME_TAMANHO] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11",
                                                           "12", "13", "14", "15", "16", "17", "18", "19", "20", "21",
                                                           "22", "23", "24"};

typedef enum { false, true } boolean;

typedef struct {
    char nome[RSC_NOME_TAMANHO + RSC_EXTENSAO_TAMANHO];
    boolean status;
} progressoNoJogo;

typedef struct {
    int X;
    int Y;
} card;

typedef struct {
    card card1;
    card card2;
    int linhas;
    int colunas;
} cardSelecionado;

boolean existeNaMatriz(int [LINHAS_MAX][COLUNAS_MAX], int, cardSelecionado);
boolean checarStatus(progressoNoJogo [LINHAS_MAX][COLUNAS_MAX], cardSelecionado);
void escolherCards(progressoNoJogo [LINHAS_MAX][COLUNAS_MAX], cardSelecionado);
void preencherMatriz(int [LINHAS_MAX][COLUNAS_MAX], int, cardSelecionado);
void novoJogo(char[DATA_TAMANHO], progressoNoJogo[LINHAS_MAX][COLUNAS_MAX], cardSelecionado);
void escreverCorpo(char [DATA_TAMANHO], progressoNoJogo [LINHAS_MAX][COLUNAS_MAX], cardSelecionado, boolean);
void salvarProgresso(char [DATA_TAMANHO], progressoNoJogo [LINHAS_MAX][COLUNAS_MAX], cardSelecionado cardS, boolean, int);
void recuperarProgresso(char [DATA_TAMANHO], progressoNoJogo [LINHAS_MAX][COLUNAS_MAX], cardSelecionado *, boolean *, int *);
void proximoPasso(char [DATA_TAMANHO], progressoNoJogo [LINHAS_MAX][COLUNAS_MAX], cardSelecionado, int, boolean);
void escreverCabecalho(char [DATA_TAMANHO], boolean);
void analisarEntrada(char [DATA_TAMANHO], char *, cardSelecionado *);

int main() {
    char code[DATA_TAMANHO], *dados = NULL;
    int contadorDeCliques;
    long int tamanho;
    progressoNoJogo cardFF[LINHAS_MAX][COLUNAS_MAX];
    cardSelecionado cardS;
    boolean refresh;

    printf("Content-Type:text/html;charset=UTF-8\n\n");
    if (!strcmp(getenv("REQUEST_METHOD"), "POST")) {
        tamanho = strtol(getenv("CONTENT_LENGTH"), NULL, 10);
        dados = (char *) malloc((size_t) (tamanho + 1));
        if (dados) {
            fread(dados, (size_t) (tamanho + 1), 1, stdin);
            analisarEntrada(code, dados, &cardS);
            if (!strcmp(code, "-1")) {
                novoJogo(code, cardFF, cardS);
            } else {
                recuperarProgresso(code, cardFF, &cardS, &refresh, &contadorDeCliques);
                proximoPasso(code, cardFF, cardS, contadorDeCliques + 1, refresh);
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
 * Procedimento analisarEntrada
 * Objetivo analisar string de entrada dados
 * Parâmetros formais:
 *      Dados de entrada:
 *              char *dados : string crua de dados
 *      Dados de saída:
 *              char code[DATA_TAMANHO] : código da instância do jogo extraída de *dados
 *              cardSelecionado *cardS : informações sobre a última card selecionada anteriormente e as dimensões da instância do jogo extraídas de *dados
 */
void analisarEntrada(char code[DATA_TAMANHO], char *dados, cardSelecionado *cardS) {
    char *token;

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

/*
 * Procedimento proximoPasso
 * Objetivo: determinar o próximo passo a ser tomado
 * Parâmetros formais:
 *      Dados de entrada:
 *              char code[DATA_TAMANHO] : código da instância do jogo
 *              progressoNoJogo cardFF[LINHAS_MAX][COLUNAS_MAX] : matriz LINHAS_MAX x COLUNAS_MAX de progressoNoJogo contendo a imagem de cada posição
 *              cardSelecionado cardS : contém as informações do último par de cartas selecionado anteriormente
 *              contadorDeCliques : contém a contagem de cards já viradas
 *              boolean refresh : flag de recarregamento da página
 */
void proximoPasso(char code[DATA_TAMANHO], progressoNoJogo cardFF[LINHAS_MAX][COLUNAS_MAX], cardSelecionado cardS, int contadorDeCliques, boolean refresh) {
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
        printf("Fim!<br>");
    }
}

/*
 * procedimento novoJogo
 * Objetivo: iniciar uma nova instância do jogo
 * Parâmetros formais:
 *      Dados de saída:
 *              char code[DATA_TAMANHO] : novo código da instância do jogo
 *              progressoNoJogo cardFF[LINHAS_MAX][COLUNAS_MAX] : nova matriz LINHAS_MAX x COLUNAS_MAX de progressoNoJogo da instância do jogo
 *              cardSelecionado cardS : novo cardSelecionado preenchido com -1 (nenhuma card selecionada)
 */
void novoJogo(char code[DATA_TAMANHO], progressoNoJogo cardFF[LINHAS_MAX][COLUNAS_MAX], cardSelecionado cardS) {
    cardS.card2.X = -1;
    cardS.card2.Y = -1;

    sprintf(code, "%lld", time(NULL));
    escolherCards(cardFF, cardS);
    escreverCorpo(code, cardFF, cardS, false);
    salvarProgresso(code, cardFF, cardS, false, 0);
}

/*
 * Procedimento salvarProgresso
 * Objetivo: salva o progresso de uma instância do jogo
 * Parâmetros formais:
 *      Dados de entrada:
 *              char code[DATA_TAMANHO] : código da instância do jogo
 *              progressoNoJogo cardFF[LINHAS_MAX][COLUNAS_MAX] : matriz LINHAS_MAX x COLUNAS_MAX de progressoNoJogo a ser escrita para o arquivo
 *              cardSelecionado cardS : cardSelecionado a ser escrito para o arquivo
 *              boolean refresh : flag de refresh a ser escrita para o arquivo
 *              int contadorDeCliques : contagem de cards viradas a ser escrita para o arquivo
 */
void salvarProgresso(char code[DATA_TAMANHO], progressoNoJogo cardFF[LINHAS_MAX][COLUNAS_MAX], cardSelecionado cardS,
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
 *          progressoNoJogo cardFF[LINHAS_MAX][COLUNAS_MAX] : matriz LINHAS_MAX x COLUNAS_MAX de progressoNoJogo recuperada do arquivo de progresso
 *          cardSelecionado *cardS : cardSelecionado recuperado do arquivo de progresso
 *          boolean *refresh : flag de refresh recuperado do arquivo de progresso
 *          int *contadorDeCliques : contagem de cards viradas recuperada do arquivo de progresso
 */
void recuperarProgresso(char code[DATA_TAMANHO], progressoNoJogo cardFF[LINHAS_MAX][COLUNAS_MAX], cardSelecionado *cardS,
                        boolean *refresh, int *contadorDeCliques) {
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
 *              progressoNoJogo cardFF[LINHAS_MAX][COLUNAS_MAX] :  matriz LINHAS_MAX x COLUNAS_MAX de progressoNoJogo contendo os dados de cada card
 *              cardSelecionado cardS : contém informações sobre o tamanho real da matriz e o último par de cards selecionado anteriormente
 *              boolean refresh : parâmetro a ser repassado para escreverCabeçalho
 */
void escreverCorpo(char code[DATA_TAMANHO], progressoNoJogo cardFF[LINHAS_MAX][COLUNAS_MAX], cardSelecionado cardS, boolean refresh) {
    int i, j;

    printf("<html>");
    escreverCabecalho(code, refresh);
    printf("<body>");
    printf("<div id=centralizar>");
    printf("<a>");

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

/*
 * Procedimento escreverCabecalho
 * Objetivo: escrever conteúdo da tag <head> da página
 * Parâmetros formais:
 *      Dados de Entrada:
 *              char code[DATA_TAMANHO] : código da instância do jogo
 *              boolean refresh : flag que indica se a página deve ser recarregada após 1 segundo
 */
void escreverCabecalho(char code[DATA_TAMANHO], boolean refresh) {
    printf("<head>");
    printf("<meta http-equiv=\"Content-Type\" content=\"text/html;charset=UTF-8\" />");
    if (refresh)
        printf("<meta http-equiv=\"refresh\" content=\"1; url=%s?code=%s\" />", CGI_CAMINHO, code);
    printf("<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/style.css\"/>", CSS_LOCAL);
    printf("<title>Jogo de CAP</title>");
    printf("</head>");
}

/*
 * Procedimento escolherCards
 * Objetivo: escolher as cards de um novo jogo
 * Parâmetros formais:
 *      Dados de Entrada:
 *              cardSelecionado cardS : contém as reais dimensões da matriz
 *      Dados de Saída:
 *              progressoNoJogo cardFF[LINHAS_MAX][COLUNAS_MAX] : matriz LINHAS_MAX x COLUNAS_MAX de progressoNoJogo preenchida com o nome das cards selecionadas "aleatoriamente"
 */
void escolherCards(progressoNoJogo cardFF[LINHAS_MAX][COLUNAS_MAX], cardSelecionado cardS) {
    // Controladores de Laço
    int i, j;
    boolean continuar = true;

    // Matriz Auxiliar
    int numerosAleatorios[LINHAS_MAX][COLUNAS_MAX];

    // Números Aleatórios
    int r;                  // Número "aleatório" entre 0 e RSC_QUANTIDADE - 1
    int s, t, u, v;         // Posições "aleatórias" entre 0 e RND_MAXIMO - RND_MINIMO - 1

    // Inicializar matriz
    preencherMatriz(numerosAleatorios, -1, cardS);

    // Determinar seed de randomização
    srand((unsigned int) time(NULL));

    while (continuar) {
        r = rand() % RSC_QUANTIDADE;
        s = rand() % cardS.linhas;
        t = rand() % cardS.colunas;
        u = rand() % cardS.linhas;
        v = rand() % cardS.colunas;


        // Posições s,t e u,v não são iguais
        if (!(s == u && t == v)) {
            // Posições s,t e u,v não receberam um número
            if (numerosAleatorios[s][t] == -1 && numerosAleatorios[u][v] == -1) {
                // Número r não ocorreu na matriz
                if (!existeNaMatriz(numerosAleatorios, r, cardS)) {
                    numerosAleatorios[s][t] = r;
                    numerosAleatorios[u][v] = r;
                }
            } else {
                // Verificar se ainda existe alguma posição que não recebeu um número
                continuar = existeNaMatriz(numerosAleatorios, -1, cardS);
            }
        }
    }

    // Preencher cards com os nomes dos arquivos
    for (i = 0; i < cardS.linhas; ++i) {
        for (j = 0; j < cardS.colunas; ++j) {
            strcpy(cardFF[i][j].nome, rsc[numerosAleatorios[i][j]]);
            strcat(cardFF[i][j].nome, RSC_EXTENSAO);
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
 *              cardSelecionado cardS : contém as reais dimensões da matriz
 *      Valor de Retorno: boolean
 *              true, se o número n existir em qualquer posição da matriz
 *              fasle, se o número n não existir em nenhuma posição da matriz
 */
boolean existeNaMatriz(int matriz[LINHAS_MAX][COLUNAS_MAX], int n, cardSelecionado cardS) {
    int i, j;
    for (i = 0; i < cardS.linhas; ++i)
        for (j = 0; j < cardS.colunas; ++j)
            if (matriz[i][j] == n)
                return true;

    return false;
}

/*
 * Função checarStatus
 * Objetivo: checar o parâmetro status de uma matriz do tipo progressoNoJogo
 * Parâmetros formais:
 *      Dados de Entrada:
 *              progressoNoJogo cardFF[LINHAS_MAX][COLUNAS_MAX] : matriz LINHAS_MAX x COLUNAS_MAX de progressoNoJogo a ser verificada
 *              cardSelecionado cardS : contém as reais dimensões da matriz
 *      Valor de Retorno: boolean
 *              true, se o parâmetro status de todas as posições da matriz cardFF forem false
 *              false, se o parâmetro status de qualquer posição da matriz cardFF for true
 */
boolean checarStatus(progressoNoJogo cardFF[LINHAS_MAX][COLUNAS_MAX], cardSelecionado cardS) {
    int i, j;

    for (i = 0; i < cardS.linhas; ++i)
        for (j = 0; j < cardS.colunas; ++j)
            if (cardFF[i][j].status)
                return false;

    return true;
}


/*
 * Procedimento preecherMatriz
 * Objetivo: preecher todas as posições de uma matriz de inteiros com um número n
 * Parâmetros formais:
 *      Dados de Entrada:
 *              int n : número com o qual a matriz será preenchida
 *              cardSelecionado cardS : contém as reais dimensões da matriz
 *      Dados de Saída:
 *              int matriz[LINHAS_MAX][COLUNAS_MAX] : matriz LINHAS_MAX x COLUNAS_MAX preenchida com n
 */
void preencherMatriz(int matriz[LINHAS_MAX][COLUNAS_MAX], int n, cardSelecionado cardS) {
    int i, j;

    for (i = 0; i < cardS.linhas; ++i)
        for (j = 0; j < cardS.colunas; ++j)
            matriz[i][j] = n;
}
#include <stdio.h>

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
#define LEADERBOARD_NOME "leaderboard"
#define LEADERBOARD_POSICOES 10
#define LEADERBOARD_NOME_TAMANHO 32

typedef enum { false, true } boolean;
typedef struct {
    char nome[RSC_NOME_TAMANHO + RSC_EXTENSAO_TAMANHO];
    boolean status;
} tInstancia;

typedef struct {
    int X;
    int Y;
} tCard;

typedef struct {
    tCard card1;
    tCard card2;
    int linhas;
    int colunas;
} tCardPar;

typedef struct {
    char nome[LEADERBOARD_NOME_TAMANHO];
    double pontuacao;
} tPlacar;

static const char rsc[RSC_QUANTIDADE][RSC_NOME_TAMANHO] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11",
                                                           "12", "13", "14", "15", "16", "17", "18", "19", "20", "21",
                                                           "22", "23", "24"};

boolean existeNaMatriz(int [][COLUNAS_MAX], int, tCardPar);
boolean checarStatus(tInstancia [][COLUNAS_MAX], tCardPar);
long int tamanhoArquivo(FILE * arquivo);
void recuperarLeaderboard(tPlacar placar[], int *);
void escolherCards(tInstancia [][COLUNAS_MAX], tCardPar);
void preencherMatriz(int [][COLUNAS_MAX], int, tCardPar);
void novoJogo(char [], tInstancia[][COLUNAS_MAX], tCardPar);
void escreverCorpo(char [], tInstancia [][COLUNAS_MAX], tCardPar, boolean);
void salvarProgresso(char [], tInstancia [][COLUNAS_MAX], tCardPar cardS, boolean, int);
void recuperarProgresso(char [], tInstancia [][COLUNAS_MAX], tCardPar *, boolean *, int *);
void proximoPasso(char [], tInstancia [][COLUNAS_MAX], tCardPar, int, boolean);
void escreverCabecalho(char [], boolean);
void analisarEntrada(char [], char *, tCardPar *, short int *, int *, char *);
void escreverLeaderboard(tPlacar [], int);
void paginaLeaderboard(int);
void salvarLeaderboard(tPlacar [], int);
int ordenarLeaderboard(const void *s1, const void *s2);
void pedirNomeParaLeaderboard (int, tCardPar);
void processarLeaderboard(int, int, tCardPar);
void fimDeJogo();
void adicionarAoLeaderboard(int, int, tCardPar, char *, char []);
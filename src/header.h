#include <stdio.h>

// Constantes

// Dimensões máximas
#define LINHAS_MAX 7
#define COLUNAS_MAX 10

// Local
#define RSC_LOCAL "../memoria/resources"        // Imagens
#define CSS_LOCAL "../memoria/css/"             // CSS
#define DATA_LOCAL "../memoria/dados/"          // Dados de jogo
#define CGI_CAMINHO "http://cap.dc.ufscar.br/~744344/cgi-bin/memoria.cgi"       // Programa CGI

// Imagens
#define RSC_QUANTIDADE 24       // Quantidade
#define RSC_NOME_TAMANHO 32     // Tamanho máximo do nome
#define RSC_OK "done.png"       // Imagem do par
#define RSC_SELECT "mmry.png"   // Carta não virada

// Dados
#define DATA_TAMANHO 20         // Tamanho máximo do código de jogo
#define DATA_EXTENSAO ".dat"    // Extensão dos arquivos de dados

// Placar
#define LEADERBOARD_NOME "leaderboard"  // Nome do arquivo de leaderboard
#define LEADERBOARD_POSICOES 10         // Posições no leaderboard
#define LEADERBOARD_NOME_TAMANHO 32     // Tamanho máximo do nome do leaderboard


// Registros

typedef enum {
    false=0,
    true
} boolean;

typedef struct {
    char nome[RSC_NOME_TAMANHO];
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
    char code[DATA_TAMANHO];
} tPlacar;

// Constantes
static const char rsc[RSC_QUANTIDADE][RSC_NOME_TAMANHO] = {"1.png", "2.png", "3.png", "4.png", "5.png", "6.png", "7.png", "8.png", "9.png", "10.png", "11.png",
                                                           "12.png", "13.png", "14.png", "15.png", "16.png", "17.png", "18.png", "19.png", "20.png", "21.png",
                                                           "22.png", "23.png", "24.png"};

// Protótipos
boolean existeNaMatriz(int [][COLUNAS_MAX], int, tCardPar);
boolean checarStatus(tInstancia [][COLUNAS_MAX], tCardPar);

long int tamanhoArquivo(FILE * arquivo);
int ordenarLeaderboard(const void *s1, const void *s2);

void recuperarLeaderboard(tPlacar placar[], int *);
void preencherLeaderboard(int, int, tPlacar []);
void salvarLeaderboard(tPlacar [], int);

void escolherCards(tInstancia [][COLUNAS_MAX], tCardPar);
void preencherMatriz(int [][COLUNAS_MAX], int, tCardPar);
void novoJogo(char [], tInstancia[][COLUNAS_MAX], tCardPar);
void escreverCorpo(char [], tInstancia [][COLUNAS_MAX], tCardPar, boolean, int, boolean);
void salvarProgresso(char [], char [], char [], tInstancia [][COLUNAS_MAX], tCardPar cardS, boolean, int);
void recuperarProgresso(char [], char [], char [], tInstancia [][COLUNAS_MAX], tCardPar *, boolean *, int *);
void proximoPasso(char [], tInstancia [][COLUNAS_MAX], tCardPar, int, boolean);
void escreverCabecalho(char [], boolean, int);
void analisarEntrada(char [], char *, tCardPar *, short int *, char *);
void escreverLeaderboard(tPlacar [], int);
void paginaLeaderboard(int);

void pedirNomeParaLeaderboard (char []);
void processarLeaderboard(int, int, tCardPar, char[]);
void fimDeJogo();
void adicionarAoLeaderboard(int, int, tCardPar, char *, char []);

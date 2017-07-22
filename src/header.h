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
#define RSC_SELECT "mmry.jpg"   // Carta não virada

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
static const char rsc[RSC_QUANTIDADE][RSC_NOME_TAMANHO] = {"0.jpg", "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "10.jpg", "11.jpg", "12.jpg", "13.jpg", "14.jpg", "15.jpg", "16.jpg", "17.jpg", "18.jpg", "20.jpg", "21.jpg", "22.jpg", "23.jpg", "25.jpg"};

// Protótipos
boolean existeNaMatriz(int [][COLUNAS_MAX], int, tCardPar);
boolean checarStatus(tInstancia [][COLUNAS_MAX], tCardPar);

long int tamanhoArquivo(FILE * arquivo);
int ordenarLeaderboard(const void *s1, const void *s2);

void recuperarLeaderboard(char [], char [], char [], tPlacar placar[], int *);
void preencherLeaderboard(int, tPlacar []);
void salvarLeaderboard(char [], char [], char [], tPlacar [], int);
void escreverLeaderboard(tPlacar [], int);
void paginaLeaderboard(char [], char [], char [], int);
void pedirNomeParaLeaderboard (char [], char [], char []);
void processarLeaderboard(char [], char[], char [], char [], char [], int, int, tCardPar, char[]);
void adicionarAoLeaderboard(char [], char [], char [], int, int, tCardPar, char *, char []);

void escolherCards(tInstancia [][COLUNAS_MAX], tCardPar);
void preencherMatriz(int [][COLUNAS_MAX], int, tCardPar);

void novoJogo(char [], char [], char [], char [], char [], char [], char [], char [], tInstancia[][COLUNAS_MAX], tCardPar);
void escreverCorpo(char [], char [], char [], char [], char [], char [], tInstancia [][COLUNAS_MAX], tCardPar, boolean, int, boolean);
void salvarProgresso(char [], char [], char [], tInstancia [][COLUNAS_MAX], tCardPar cardS, boolean, int);
void recuperarProgresso(char [], char [], char [], char [], tInstancia [][COLUNAS_MAX], tCardPar *, boolean *, int *, int);
void proximoPasso(char [], char [], char [], char [], char [], char [], char [], char [], tInstancia [][COLUNAS_MAX], tCardPar, int, boolean, int, char []);
void analisarEntrada(char [], char *, tCardPar *, short int *, char *);

void fimDeJogo(char [], char []);
void escreverCabecalho(char [], char [], char [], boolean, int);

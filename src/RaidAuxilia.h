#ifndef RAIDAUXILIA_H
#define RAIDAUXILIA_H

#include "MapGen.h"

/* Tabela de cores usadas: (C para claro, E para escuro)

    Verde C com fundo Verde C:  170
    Verde E com fundo Verde C:  162
    Azul C com fundo Azul C:    153
    Azul E com fundo Azul C:    145
    Cinza E com fundo Cinza E:  136
    Preto com fundo Preto:      0
    Preto com fundo Branco:     15
    Amarelo C com fundo Azul C: 158
    Branco com fundo Azul C:    159

   Tabela de esquema de cores:

    Preto e Branco
    Floresta
    Tundra
    Deserto

   Tabela de caracteres especiais usados:

      Texto
    á (%a): 160
    ã (%A): 198
    é (%e): 130
    ê (%E): 136
    í (%i): 161
    ó (%o): 162
    õ (%O): 228
    ô : 147
    ú (%u): 163
    ç (%c): 135

      Graph default
    seta direita cheia: 16
    seta esquerda cheia: 17
    "explosão": 15

      Graph custom
    exclamação dupla (%1): 19
    carinha feliz (%2): 2
    coração (%3): 3
    seta normal acima (%4): 24
    copas (baralho) (%5): 5
    y cortado (%6): 190
    "bomba acesa" (%7): 208
    "foice dupla" (%8): 21
    "AE" (%9): 147
    duas semicolcheiras (%0): 14                       */


#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <windows.h>
#include "conio2.h"
#include <unistd.h> //Função access()

//Limite de objetos
#define EXPLO_MAX 80
#define NAVIO_MAX 50
#define HELI_MAX 100

//Tempo para cada objeto se mover
#define TEMPO_TIRO 20
#define TEMPO_EXPLO 50
#define TEMPO_PLAYER 30
#define TEMPO_TIRO_DELAY TEMPO_TIRO + 50
#define TEMPO_DEATH_DELAY 1000

//Caractéres dos inimigos
#define HELI_CHAR 'X'
#define NAVIO_ESQ 223
#define NAVIO_CEN 219
#define NAVIO_DIR 223

//Posições das estatísticas
#define X_GAS LARG/2 - 6

typedef struct cores {
    int terra;
    int ilha;
    int veg;
    char vegc;
    int vegtaxa;
    int vegi;
    char vegic;
    int vegitaxa;
    int agua;
    char aguac;
    int ponte;
    int road;
    char roadc;
    int fuel1;
    int fuel2;
    int navio;
    int heli;
    int defroad;
    int defagua;
    int vida;
    char vidac;
} TCores;

typedef struct graphs {
    TCores mapa; // Esquema usado para o mapa: | 0 = preto e branco | 1 = default (floresta) | 2 = tundra | 3 = deserto |
    char player;
    int playercor;
    char tiro;
    int tirocor;
    char explo;
    int explocor;
} TGraphs;

typedef struct tempos {
	int fase;
	int inimigos;
	//int tiro
	//int explo
} TTempos;

typedef struct faseinfo {
	TTempos tempo;
	int tiro_lim;
	int linhas;
	short int dificuldade;
	int vidas;
	int xie, xid, xfe, xfd; //Posições x inicial e final dos lados esquerdo e direito
	int ponteY;
} TFaseInfo;

typedef struct contador {
	int navios;
	int heli;
	int explo;
	int tiros;
} TContador;

typedef struct player {
    int x, y;
    int tempo;
    int tempo_tiro;
    int upcor;
    char upchar;
    float gas;
} TPlayer;

typedef struct tiro {
    int x, y;
    int tempo;
} TTiro;

typedef struct explosao {
    int x, y;
    //char c;
    //int cor;
    int nivel; //Define o nível da explosão, criando ao redor bombas com um nível menor, a não ser que o nível seja 0
    int tempo;
} TExplo;

typedef struct navio {
	int x, y;
	unsigned char dir, cen, esq; //Caractere direito, central e esquerdo
	//int ang; //Direção do movimento
	int mov; //Sentido do movimento
	int vivo; //Verificador se o inimigo ainda está vivo
	int tempo;
} TNavio;

typedef struct helicoptero {
	int x, y;
	char c;
	int mov;
	int vivo;
	int tempo;
} THeli;

TTiro addTiro(int x, int y);
TExplo addExplo(int x, int y, int nivel);
TNavio addNavio(int x, int y, /*int ang,*/ unsigned char dir, unsigned char cen, unsigned char esq);
THeli addHeli(int x, int y, char c);
int intNDigits(int n);
void printGas(float gasf);
int testaColTiro(TTiro tiro[], int i, TExplo explo[], TContador *n, TGraphs g, int fase_pos);
int testaColPlayer(TPlayer *p, TExplo explo[], TContador *n, TGraphs g, int *recarregado, int fase_pos, int d_x, int d_y);
void testaColExplo(TExplo explo[], TPlayer *pl, int i, TContador *n, TGraphs g, int *ponte_viva, int fase_pos, int d_x, int d_y);
void hideCursor();
void explodePonte(TGraphs g);
int lerMatriz(char nomedoarquivo[], char matriz[][LARG], int custom_true);
int printFromFile(int x, int y, TCores cores, char nomedoarquivo[]);
void printNaAgua(unsigned char texto[], TCores cores);
int confirmaReset();
int confirmaSaida();
void pause();

#endif // RAIDAUXILIA_H

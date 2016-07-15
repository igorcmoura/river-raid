#ifndef SCORES_H
#define SCORES_H

#include "RaidAuxilia.h"

#define NOME_MAX 10

typedef struct pontuacao {
	char nome_jogador[NOME_MAX];
	char nome_fase[ALT];
	short int modo_jogo;
	int score;
} TPont;

int testScore(int score, short int vivo, short int modo_jogo, char nome_fase[]);
int highScores();

#endif // SCORES_H

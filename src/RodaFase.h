#ifndef RODAFASE_H
#define RODAFASE_H

#include "RaidAuxilia.h"

int novoJogo(TGraphs g);
int faseCustom(TGraphs g);
int modoSurvival(TGraphs g);
int rodaFase(char fase[][LARG],
             unsigned short int cor[][LARG],
             char extensao[][LARG],
             unsigned short int extcor[][LARG],
             char conexao[][LARG],
             unsigned short int concor[][LARG],
             TGraphs g,
             TPlayer *p,
             int *pontos,
             TFaseInfo info,
             int redraw);
int initFase(char fase[][LARG],
             unsigned short int cor[][LARG],
             char extensao[][LARG],
             unsigned short int extcor[][LARG],
             char conexao[][LARG],
             unsigned short int concor[][LARG],
             TCores cores,
             TFaseInfo *info,
             int redraw);

#endif // RODAFASE_H

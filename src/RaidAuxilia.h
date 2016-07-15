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
    � (%a): 160
    � (%A): 198
    � (%e): 130
    � (%E): 136
    � (%i): 161
    � (%o): 162
    � (%O): 228
    � : 147
    � (%u): 163
    � (%c): 135

      Graph default
    seta direita cheia: 16
    seta esquerda cheia: 17
    "explos�o": 15

      Graph custom
    exclama��o dupla (%1): 19
    carinha feliz (%2): 2
    cora��o (%3): 3
    seta normal acima (%4): 24
    copas (baralho) (%5): 5
    y cortado (%6): 190
    "bomba acesa" (%7): 208
    "foice dupla" (%8): 21
    "AE" (%9): 147
    duas semicolcheiras (%0): 14                       */


#include <stdio.h>
#include <conio2.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <windows.h>
#include <unistd.h> //Fun��o access()
#include "MapGen.h"

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

//Caract�res dos inimigos
#define HELI_CHAR 'X'
#define NAVIO_ESQ 223
#define NAVIO_CEN 219
#define NAVIO_DIR 223

//Posi��es das estat�sticas
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
	int xie, xid, xfe, xfd; //Posi��es x inicial e final dos lados esquerdo e direito
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

TTiro addTiro(int x, int y) {
	TTiro tiro;
	tiro.x = x;
	tiro.y = y;
	tiro.tempo = -1;
	return tiro;
}

typedef struct explosao {
    int x, y;
    //char c;
    //int cor;
    int nivel; //Define o n�vel da explos�o, criando ao redor bombas com um n�vel menor, a n�o ser que o n�vel seja 0
    int tempo;
} TExplo;

TExplo addExplo(int x, int y, int nivel) {
	TExplo expl;
	expl.x = x;
	expl.y = y;
	expl.nivel = nivel;
	expl.tempo = -1;
	return expl;
}

typedef struct navio {
	int x, y;
	unsigned char dir, cen, esq; //Caractere direito, central e esquerdo
	//int ang; //Dire��o do movimento
	int mov; //Sentido do movimento
	int vivo; //Verificador se o inimigo ainda est� vivo
	int tempo;
} TNavio;

TNavio addNavio(int x, int y, /*int ang,*/ unsigned char dir, unsigned char cen, unsigned char esq) {
	TNavio navio;
	navio.x = x;
	navio.y = y;
	navio.dir = dir;
	navio.cen = cen;
	navio.esq = esq;
	//navio.ang = ang;
	navio.mov = 0;
	navio.vivo = 1;
	navio.tempo = -1;
	return navio;
}

typedef struct helicoptero {
	int x, y;
	char c;
	int mov;
	int vivo;
	int tempo;
} THeli;

THeli addHeli(int x, int y, char c) {
	THeli heli;
	heli.x = x;
	heli.y = y;
	heli.c = c;
	heli.mov = 0;
	heli.vivo = 1;
	heli.tempo = -1;
	return heli;
}

//Faz a contagem de digitos de um inteiro
int intNDigits(int n) {
	int digits = 0;
	if(n == 0)
		return 1;
	while(n) {
		digits++;
		n /= 10;
	}
	return digits;
}

void printGas(float gasf) {
	int i, gas = gasf;
	if(gas % 10)
		gas += 10;
	gas /= 10;
	putchar('|');
	for(i = 0; i < gas && i < 10; i++)
		putchar(219);
	for(; i < 10; i++)
		putchar(' ');
	putchar('|');
}

//Testes de colis�o
//Retorna 1 se o tiro colidiu e o index do tiro � igual a nova quantidade de tiros, caso contr�rio, 0
int testaColTiro(TTiro tiro[], int i, TExplo explo[], TContador *n, TGraphs g, int fase_pos) {
	struct char_info readinfo;

	gettext(tiro[i].x, tiro[i].y, tiro[i].x, tiro[i].y, &readinfo);
	if(readinfo.letter != g.mapa.aguac && readinfo.letter != g.explo && readinfo.letter != g.tiro) {
	    if(readinfo.letter == 'T' ||
		   readinfo.letter == 'I' ||
		   readinfo.letter == g.mapa.vegc ||
		   readinfo.letter == g.mapa.roadc) {

			//Se a colis�o ocorreu com um objeto s�lido a explos�o aparece na posi��o abaixo do objeto
			if(n->explo < EXPLO_MAX) {
				explo[n->explo] = addExplo(tiro[i].x, tiro[i].y+fase_pos+1, 1);
	            n->explo++;
	        }
	    } else {
	       	//Sen�o aparece em cima do objeto
	        if(n->explo < EXPLO_MAX) {
		    	explo[n->explo] = addExplo(tiro[i].x, tiro[i].y+fase_pos, 1);
		        n->explo++;
		    }
			textcolor(g.tirocor);
			gotoxy(tiro[i].x, tiro[i].y);
			putchar(g.tiro);
	    }
	    n->tiros--;
	    if(i != n->tiros) {
	    	tiro[i] = tiro[n->tiros];
	        return 1;
	    }
	} else {
		textcolor(g.tirocor);
		gotoxy(tiro[i].x, tiro[i].y);
		putchar(g.tiro);
	}
	return 0;
}

//Retorna 1 se o jogador ainda est� vivo, caso contr�rio, 0
int testaColPlayer(TPlayer *p, TExplo explo[], TContador *n, TGraphs g, int *recarregado, int fase_pos, int d_x, int d_y) {
	struct char_info readinfo;

	gettext(p->x, p->y, p->x, p->y, &readinfo);
	//Faz o teste se a nova posi��o � �gua
	if(readinfo.letter != g.mapa.aguac && readinfo.letter != g.explo  && readinfo.letter != g.player) {
    	//Se n�o for testa se � posto de reabastecimento
        if(readinfo.letter == 'F' || readinfo.letter == 'U' ||
		   readinfo.letter == 'E' || readinfo.letter == 'L') {

			//Recarrega apenas e reativa o estado de recarregado para aquela linha
			p->gas += 30;
			textcolor(15);
			gotoxy(X_GAS, ALT);
			printGas(p->gas);
			*recarregado = 1;

			if(readinfo.letter == 'F' || readinfo.letter == 'E') p->upcor = g.mapa.fuel1;
			if(readinfo.letter == 'U' || readinfo.letter == 'L') p->upcor = g.mapa.fuel2;
			p->upchar = readinfo.letter;
			textcolor(g.playercor);
			gotoxy(p->x, p->y);
			putchar(g.player);
            return 1;
		}
		if(readinfo.letter == 'T' ||
		   readinfo.letter == 'I' ||
		   readinfo.letter == g.mapa.vegc ||
		   readinfo.letter == g.mapa.roadc) {

        	//Se a colis�o ocorreu com um objeto s�lido a explos�o aparece na posi��o deslocada do objeto
            if(n->explo < EXPLO_MAX) {
            	explo[n->explo] = addExplo(p->x+d_x, p->y+fase_pos+d_y, 1);
                n->explo++;
            }
            return 0;
        }
        //Sen�o aparece em cima do objeto
        if(n->explo < EXPLO_MAX) {
        	explo[n->explo] = addExplo(p->x, p->y+fase_pos, 1);
            n->explo++;
        }
        textcolor(g.playercor);
		gotoxy(p->x, p->y);
		putchar(g.player);
        return 0;
    }
    p->upcor = g.mapa.agua;
    p->upchar = g.mapa.aguac;
    textcolor(g.playercor);
	gotoxy(p->x, p->y);
	putchar(g.player);
    return 1;
}

void testaColExplo(TExplo explo[], TPlayer *pl, int i, TContador *n, TGraphs g, int *ponte_viva, int fase_pos, int d_x, int d_y) {
	struct char_info readinfo;
	gettext(explo[i].x+d_x, explo[i].y-fase_pos+d_y, explo[i].x+d_x, explo[i].y-fase_pos+d_y, &readinfo);
	if(readinfo.letter != 'T' &&
	   readinfo.letter != 'I' &&
	   readinfo.letter != g.mapa.roadc &&
	   readinfo.letter != g.mapa.vegc &&
	   readinfo.letter != g.explo) {

		//Se na dire��o da explos�o s� tiver �gua o o player s� adiciona se a explos�o for de um n�vel maior que 1
		if(readinfo.letter != g.mapa.aguac && readinfo.letter != g.player && readinfo.letter != g.tiro) {
			//Testa se a explos�o atingiu a ponte
			if(readinfo.letter == 'P')
				*ponte_viva = 0;
			if(n->explo < EXPLO_MAX && explo[i].y+d_y < fase_pos+ALT && explo[i].x+d_x > 0 && explo[i].x+d_x <= LARG) {
			    explo[n->explo] = addExplo(explo[i].x+d_x, explo[i].y+d_y, 1);
			    n->explo++;
			}
		} else if(explo[i].nivel > 0) {
			if(readinfo.letter == g.player) {
				pl->upcor = g.mapa.agua;
				pl->upchar = g.mapa.aguac;
			}
			if(n->explo < EXPLO_MAX && explo[i].y+d_y < fase_pos+ALT && explo[i].x+d_x > 0 && explo[i].x+d_x <= LARG) {
				explo[n->explo] = addExplo(explo[i].x+d_x, explo[i].y+d_y, explo[i].nivel-1);
			    n->explo++;
			}
		}
	}
}

/* Esconde o cursor, usando magia negra ainda n�o estudada */
void hideCursor() {
	HANDLE standardOutputHandle;
	CONSOLE_CURSOR_INFO consoleCursorInfo;
	standardOutputHandle = GetStdHandle (STD_OUTPUT_HANDLE);
	consoleCursorInfo.dwSize = 10;
	consoleCursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo (standardOutputHandle, &consoleCursorInfo);
}

/* Explode a ponte dos menus est�ticos. S� funciona neles e para eles */
void explodePonte(TGraphs g) {
	TExplo expl[EXPLO_MAX];
	int n_expl = 0;

	struct char_info info;

    int i, delay = 0;

    expl[n_expl].x = 39;
    expl[n_expl].y = 3;
    expl[n_expl].nivel = 1;
    expl[n_expl].tempo = 0;
    n_expl++;
	//Processa as explos�es
	while(delay < TEMPO_EXPLO * 20) {
		Sleep(1);
		i = 0;
	    while(i < n_expl) {
			textcolor(g.explocor);
			putchxy(expl[i].x, expl[i].y, g.explo);
	       	if(expl[i].tempo == TEMPO_EXPLO/4) {
	       		//Cria novas explos�es ao redor
	       		//A esquerda
	       		gettext(expl[i].x-1, expl[i].y, expl[i].x-1, expl[i].y, &info);
	       		if(info.letter != 'T' && info.letter != g.mapa.roadc && info.letter != g.mapa.vegc && info.letter != g.explo) {
	       			if(info.letter != g.mapa.aguac) {
						expl[n_expl] = addExplo(expl[i].x-1, expl[i].y, 1);
		       			if(n_expl < EXPLO_MAX)
		       				n_expl++;
	       			} else if(expl[i].nivel > 0) {
	       				expl[n_expl] = addExplo(expl[i].x-1, expl[i].y, 0);
		       			if(n_expl < EXPLO_MAX)
		       				n_expl++;
	       			}
	       		}

	       		//A direita
	       		gettext(expl[i].x+1, expl[i].y, expl[i].x+1, expl[i].y, &info);
	       		if(info.letter != 'T' && info.letter != g.mapa.roadc && info.letter != g.mapa.vegc && info.letter != g.explo) {
	       			if(info.letter != g.mapa.aguac) {
	       				expl[n_expl] = addExplo(expl[i].x+1, expl[i].y, 1);
		       			if(n_expl < EXPLO_MAX)
		       				n_expl++;
	       			} else if(expl[i].nivel > 0) {
	       				expl[n_expl] = addExplo(expl[i].x+1, expl[i].y, 0);
		       			if(n_expl < EXPLO_MAX)
		       				n_expl++;
	       			}
	       		}

	       		//Acima
	       		gettext(expl[i].x, expl[i].y-1, expl[i].x, expl[i].y-1, &info);
	       		if(info.letter != 'T' && info.letter != g.mapa.roadc && info.letter != g.mapa.vegc && info.letter != g.explo && expl[i].nivel > 0) {
	       			expl[n_expl] = addExplo(expl[i].x, expl[i].y-1, 0);
		       		if(n_expl < EXPLO_MAX)
		       			n_expl++;
	       		}

	       		//Abaixo
	       		gettext(expl[i].x, expl[i].y+1, expl[i].x, expl[i].y+1, &info);
	       		if(info.letter != 'T' && info.letter != g.mapa.roadc && info.letter != g.mapa.vegc && info.letter != g.explo && expl[i].nivel > 0) {
	       			expl[n_expl] = addExplo(expl[i].x, expl[i].y+1, 0);
		       		if(n_expl < EXPLO_MAX)
		       			n_expl++;
	       		}
		  	}
	       	expl[i].tempo++;
	       	if(expl[i].tempo >= TEMPO_EXPLO) {
	       		textcolor(g.mapa.agua);
	       		putchxy(expl[i].x, expl[i].y, g.mapa.aguac);
	       		n_expl--;
	       		expl[i] = expl[n_expl];
	       		i--;
	       	}
	       	i++;
	    }
	    delay++;
	}
}

/* Recebe um nome de arquivo e uma matriz e bota aquele nessa. Retorna o �ndice da �ltima linha da matriz, ou -1 se o arquivo n�o foi encontrado.
   Cada linha encerra apenas quando acha um '\n' ou um EOF, e vai s� at� 80 chars. Pode descartar navios, helic�pteros e postos de gasolina ou n�o. */
int lerMatriz(char nomedoarquivo[], char matriz[][LARG], int custom_true) {
    int i = -1, j;
    char c = 'A';
    FILE *f = fopen(nomedoarquivo, "r");
    if(f == NULL) {
        return -1;
    }
    while(c != EOF) {
        // Come�a a linha
        i++;
        j = 0;
        // L� at� um '\n', um EOF ou at� o limite da linha
        while((c = fgetc(f)) != '\n' && c != EOF && j < LARG) {
            if(!custom_true || (c != 'N' && c != 'X' && c != 'G'))
                matriz[i][j] = c;
            else
                matriz[i][j] = ' ';
            j++;
        }
        // Se acabou antes do limite da linha, preenche o resto com '\0'
        while(j < LARG) {
            matriz[i][j] = 0;
            j++;
        }
        //  Se ainda n�o achou um '\n' ou um EOF, segue lendo e descartando
        while(c != '\n' && c != EOF) {
            c = fgetc(f);
        }
    }
    fclose(f);
    return i;
}

/* Recebe o nome de um arquivo .txt (que deve estar na pasta do .exe) e o printa na tela, a partir dos x e y dados, e conforme o esquema de cores.
   Retorna 0 se n�o achou o arquivo, ou 1 se teve sucesso                                                                                          */
int printFromFile(int x, int y, TCores cores, char nomedoarquivo[]) {
    int special = 0, road = 0, gas = 0;
    char c;
    FILE *f = fopen(nomedoarquivo, "r");
    if(f == NULL) {
    	textcolor(15);
    	gotoxy(1,1);
    	printf("Erro 404: arquivo \"%s\" n%co encontrado.\nConfira se o jogo est%c instalado corretamente.", nomedoarquivo, 198, 160);
    	getch();
        return 0;
    } else {
        gotoxy(x,y);
        while((c = fgetc(f)) != EOF) {
            switch(c) {
                case 'T':   if(randomTrue(cores.vegtaxa)) {
                                c = cores.vegc;
                                textcolor(cores.veg);
                            }
                            else textcolor(cores.terra);
                            break;
                case ' ':   c = cores.aguac;
                            textcolor(cores.agua);
                            break;
                case 'P':   textcolor(cores.ponte);
                            break;
                case 'W':   c = cores.roadc;
                            textcolor(cores.road);
                            road = 15;
                            break;
                case 'G':   switch(gas) {
                                case 0: textcolor(cores.fuel1);
                                        c = 'F';
                                        gas++;
                                        break;
                                case 1: textcolor(cores.fuel2);
                                        c = 'U';
                                        gas++;
                                        break;
                                case 2: textcolor(cores.fuel1);
                                        c = 'E';
                                        gas++;
                                        break;
                                case 3: textcolor(cores.fuel2);
                                        c = 'L';
                                        gas = 0;
                            }
                            break;
                case 'X':   textcolor(cores.heli);
                            break;
                case 'N':   textcolor(cores.navio);
                            fgetc(f);
                            fgetc(f);
                            fgetc(f);
                            putchar(223);
                            putchar(219);
                            putchar(219);
                            c = 223;
                            break;
                case '%':   special = 1;
                default:    if(road > 0) {
                                textcolor(cores.defroad);
                            }
                            else
                                textcolor(cores.defagua);
            }
            if(special)
                switch(c = fgetc(f)) {
                    case 'a': c = 160; break;
                    case 'A': c = 198; break;
                    case 'e': c = 130; break;
                    case 'E': c = 136; break;
                    case 'i': c = 161; break;
                    case 'o': c = 162; break;
                    case 'O': c = 228; break;
                    case 'u': c = 163; break;
                    case 'c': c = 135; break;
                    case '1': c = 19; break;
                    case '2': c = 2; break;
                    case '3': c = 3; break;
                    case '4': c = 24; break;
                    case '5': c = 5; break;
                    case '6': c = 190; break;
                    case '7': c = 208; break;
                    case '8': c = 21; break;
                    case '9': c = 146; break;
                    case '0': c = 14;
                }
            if(c != '\n')
                putchar(c);
            else {
                y++;
                gotoxy(x,y);
            }
            special = 0;
            if(road > 0)
                road--;
        }
        gotoxy(1,1);
        fclose(f);
        return 1;
    }
}

/* Recebe uma string e uma struct de cores, printa ela na tela de acordo com as defini��es dessa struct*/
void printNaAgua(unsigned char texto[], TCores cores) {
	int i = 0;
	while(i < strlen(texto)) {
		textcolor(cores.defagua);
		switch(texto[i]) {
			case ' ':
				textcolor(cores.agua);
				putchar(cores.aguac);
				break;
			case '%':
				i++;
				switch(texto[i]) {
					case 'a': putchar(160); break;
                    case 'A': putchar(198); break;
                    case 'e': putchar(130); break;
                    case 'E': putchar(136); break;
                    case 'i': putchar(161); break;
                    case 'o': putchar(162); break;
                    case 'O': putchar(228); break;
                    case 'u': putchar(163); break;
                    case 'c': putchar(135); break;
                    case '1': putchar(19); break;
                    case '2': putchar(2); break;
                    case '3': putchar(3); break;
                    case '4': putchar(24); break;
                    case '5': putchar(5); break;
                    case '6': putchar(190); break;
                    case '7': putchar(208); break;
                    case '8': putchar(21); break;
                    case '9': putchar(146); break;
                    case '0': putchar(14); break;
                    default: putchar(texto[i]);
				}
				break;
			default:
				putchar(texto[i]);
		}
		i++;
	}
}

/* Pausa a fase e pede se o usu�rio realmente quer reset�-la */
int confirmaReset() {
    TCores cores = {15, 15, 15, 'T', 0, 15, 'T', 0, 15, ' ', 15, 15, 'W', 15, 15, 15, 15, 15, 15, 15, 'T'};
    struct char_info holdscreen[481];
    char escolha;

    // Salva o peda�o da tela a ser utilizado
    gettext(10, 9, 70, 16, holdscreen);
    //Impress�o da caixa de mensagem; se n�o achar o .txt, simplesmente encerra a fun��o dizendo que o usu�rio n�o quis resetar
    if(!printFromFile(10, 9, cores, "resources/reallyreset.txt")) {
        return 0;
    }
    // Pega a resposta do usu�rio
    do {
        escolha = tolower(getch());
    } while(escolha != 'n' && escolha != 's' && escolha != 27);
    if(escolha == 'n' || escolha == 27) {
        // Se ele escolheu n�o resetar, restaura a tela
        puttext(10, 9, 70, 16, holdscreen);
        return 0;
    }
    // Se confirmou o reset, retorna 1
    return 1;
}

/* Pausa a fun��o que chamou, salva um peda�o da tela, e printa nele a tela de confima��o. Caso o usu�rio confirme a saida, retorna 1; sen�o, retorna 0.
   Se for retornar 0, restaura o peda�o retirado. Se printFromFile n�o achou o arquivo "tela de confirma��o", limpa a tela, printa erro e confirma saida*/
int confirmaSaida() {
    TCores cores = {15, 15, 15, 'T', 0, 15, 'T', 0, 15, ' ', 15, 15, 'W', 15, 15, 15, 15, 15, 15, 15, 'T'};
    struct char_info holdscreen[239];
    char escolha;

    // Salva o peda�o da tela a ser utilizado
    gettext(23, 10, 56, 16, holdscreen);
    //Impress�o da caixa de mensagem
    if(!printFromFile(23, 10, cores, "resources/reallyquit.txt")) {
        // Se n�o achou o arquivo, limpa a tela, printa um erro e confirma sa�da
        textcolor(15);
        clrscr();
        printf("Erro 404: arquivo \"resources/reallyquit.txt\" n%co encontrado.\nConfira se o jogo est%c instalado corretamente.", 198, 160);
        getch();
        return -1;
    }
    // Pega a resposta do usu�rio
    do {
        escolha = tolower(getch());
    } while(escolha != 'n' && escolha != 's' && escolha != 27);
    if(escolha == 'n') {
        // Se ele escolheu n�o sair, restaura a tela
        puttext(23, 10, 56, 16, holdscreen);
        return 0;
    }
    // Se confirmou a sa�da, limpa a tela e retorna 1
    textcolor(15);
    clrscr();
    return 1;
}

void pause() {
    TCores cores = {15, 15, 15, 'T', 0, 15, 'T', 0, 15, ' ', 15, 15, 'T', 15, 15, 15, 15, 15, 15, 15, 'T'};
    struct char_info holdscreen[211];
    // Salva o peda�o da tela a ser utilizado
    gettext(22, 10, 56, 15, holdscreen);
    //Impress�o da caixa de mensagem
    printFromFile(22, 10, cores, "resources/pause.txt");
    // Espera despausar
    getch();
    // Restaura a tela
    puttext(22, 10, 56, 15, holdscreen);
}


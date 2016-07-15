#include "RaidAuxilia.h"

TTiro addTiro(int x, int y) {
	TTiro tiro;
	tiro.x = x;
	tiro.y = y;
	tiro.tempo = -1;
	return tiro;
}


TExplo addExplo(int x, int y, int nivel) {
	TExplo expl;
	expl.x = x;
	expl.y = y;
	expl.nivel = nivel;
	expl.tempo = -1;
	return expl;
}


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

//Testes de colisão
//Retorna 1 se o tiro colidiu e o index do tiro é igual a nova quantidade de tiros, caso contrário, 0
int testaColTiro(TTiro tiro[], int i, TExplo explo[], TContador *n, TGraphs g, int fase_pos) {
	struct char_info readinfo;

	gettext(tiro[i].x, tiro[i].y, tiro[i].x, tiro[i].y, &readinfo);
	if(readinfo.letter != g.mapa.aguac && readinfo.letter != g.explo && readinfo.letter != g.tiro) {
	    if(readinfo.letter == 'T' ||
		   readinfo.letter == 'I' ||
		   readinfo.letter == g.mapa.vegc ||
		   readinfo.letter == g.mapa.roadc) {

			//Se a colisão ocorreu com um objeto sólido a explosão aparece na posição abaixo do objeto
			if(n->explo < EXPLO_MAX) {
				explo[n->explo] = addExplo(tiro[i].x, tiro[i].y+fase_pos+1, 1);
	            n->explo++;
	        }
	    } else {
	       	//Senão aparece em cima do objeto
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

//Retorna 1 se o jogador ainda está vivo, caso contrário, 0
int testaColPlayer(TPlayer *p, TExplo explo[], TContador *n, TGraphs g, int *recarregado, int fase_pos, int d_x, int d_y) {
	struct char_info readinfo;

	gettext(p->x, p->y, p->x, p->y, &readinfo);
	//Faz o teste se a nova posição é água
	if(readinfo.letter != g.mapa.aguac && readinfo.letter != g.explo  && readinfo.letter != g.player) {
    	//Se não for testa se é posto de reabastecimento
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

        	//Se a colisão ocorreu com um objeto sólido a explosão aparece na posição deslocada do objeto
            if(n->explo < EXPLO_MAX) {
            	explo[n->explo] = addExplo(p->x+d_x, p->y+fase_pos+d_y, 1);
                n->explo++;
            }
            return 0;
        }
        //Senão aparece em cima do objeto
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

		//Se na direção da explosão só tiver água o o player só adiciona se a explosão for de um nível maior que 1
		if(readinfo.letter != g.mapa.aguac && readinfo.letter != g.player && readinfo.letter != g.tiro) {
			//Testa se a explosão atingiu a ponte
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

/* Esconde o cursor, usando magia negra ainda não estudada */
void hideCursor() {
	HANDLE standardOutputHandle;
	CONSOLE_CURSOR_INFO consoleCursorInfo;
	standardOutputHandle = GetStdHandle (STD_OUTPUT_HANDLE);
	consoleCursorInfo.dwSize = 10;
	consoleCursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo (standardOutputHandle, &consoleCursorInfo);
}

/* Explode a ponte dos menus estáticos. Só funciona neles e para eles */
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
	//Processa as explosões
	while(delay < TEMPO_EXPLO * 20) {
		Sleep(1);
		i = 0;
	    while(i < n_expl) {
			textcolor(g.explocor);
			putchxy(expl[i].x, expl[i].y, g.explo);
	       	if(expl[i].tempo == TEMPO_EXPLO/4) {
	       		//Cria novas explosões ao redor
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

/* Recebe um nome de arquivo e uma matriz e bota aquele nessa. Retorna o índice da última linha da matriz, ou -1 se o arquivo não foi encontrado.
   Cada linha encerra apenas quando acha um '\n' ou um EOF, e vai só até 80 chars. Pode descartar navios, helicópteros e postos de gasolina ou não. */
int lerMatriz(char nomedoarquivo[], char matriz[][LARG], int custom_true) {
    int i = -1, j;
    char c = 'A';
    FILE *f = fopen(nomedoarquivo, "r");
    if(f == NULL) {
        return -1;
    }
    while(c != EOF) {
        // Começa a linha
        i++;
        j = 0;
        // Lê até um '\n', um EOF ou até o limite da linha
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
        //  Se ainda não achou um '\n' ou um EOF, segue lendo e descartando
        while(c != '\n' && c != EOF) {
            c = fgetc(f);
        }
    }
    fclose(f);
    return i;
}

/* Recebe o nome de um arquivo .txt (que deve estar na pasta do .exe) e o printa na tela, a partir dos x e y dados, e conforme o esquema de cores.
   Retorna 0 se não achou o arquivo, ou 1 se teve sucesso                                                                                          */
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

/* Recebe uma string e uma struct de cores, printa ela na tela de acordo com as definições dessa struct*/
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

/* Pausa a fase e pede se o usuário realmente quer resetá-la */
int confirmaReset() {
    TCores cores = {15, 15, 15, 'T', 0, 15, 'T', 0, 15, ' ', 15, 15, 'W', 15, 15, 15, 15, 15, 15, 15, 'T'};
    struct char_info holdscreen[481];
    char escolha;

    // Salva o pedaço da tela a ser utilizado
    gettext(10, 9, 70, 16, holdscreen);
    //Impressão da caixa de mensagem; se não achar o .txt, simplesmente encerra a função dizendo que o usuário não quis resetar
    if(!printFromFile(10, 9, cores, "resources/reallyreset.txt")) {
        return 0;
    }
    // Pega a resposta do usuário
    do {
        escolha = tolower(getch());
    } while(escolha != 'n' && escolha != 's' && escolha != 27);
    if(escolha == 'n' || escolha == 27) {
        // Se ele escolheu não resetar, restaura a tela
        puttext(10, 9, 70, 16, holdscreen);
        return 0;
    }
    // Se confirmou o reset, retorna 1
    return 1;
}

/* Pausa a função que chamou, salva um pedaço da tela, e printa nele a tela de confimação. Caso o usuário confirme a saida, retorna 1; senão, retorna 0.
   Se for retornar 0, restaura o pedaço retirado. Se printFromFile não achou o arquivo "tela de confirmação", limpa a tela, printa erro e confirma saida*/
int confirmaSaida() {
    TCores cores = {15, 15, 15, 'T', 0, 15, 'T', 0, 15, ' ', 15, 15, 'W', 15, 15, 15, 15, 15, 15, 15, 'T'};
    struct char_info holdscreen[239];
    char escolha;

    // Salva o pedaço da tela a ser utilizado
    gettext(23, 10, 56, 16, holdscreen);
    //Impressão da caixa de mensagem
    if(!printFromFile(23, 10, cores, "resources/reallyquit.txt")) {
        // Se não achou o arquivo, limpa a tela, printa um erro e confirma saída
        textcolor(15);
        clrscr();
        printf("Erro 404: arquivo \"resources/reallyquit.txt\" n%co encontrado.\nConfira se o jogo est%c instalado corretamente.", 198, 160);
        getch();
        return -1;
    }
    // Pega a resposta do usuário
    do {
        escolha = tolower(getch());
    } while(escolha != 'n' && escolha != 's' && escolha != 27);
    if(escolha == 'n') {
        // Se ele escolheu não sair, restaura a tela
        puttext(23, 10, 56, 16, holdscreen);
        return 0;
    }
    // Se confirmou a saída, limpa a tela e retorna 1
    textcolor(15);
    clrscr();
    return 1;
}

void pause() {
    TCores cores = {15, 15, 15, 'T', 0, 15, 'T', 0, 15, ' ', 15, 15, 'T', 15, 15, 15, 15, 15, 15, 15, 'T'};
    struct char_info holdscreen[211];
    // Salva o pedaço da tela a ser utilizado
    gettext(22, 10, 56, 15, holdscreen);
    //Impressão da caixa de mensagem
    printFromFile(22, 10, cores, "resources/pause.txt");
    // Espera despausar
    getch();
    // Restaura a tela
    puttext(22, 10, 56, 15, holdscreen);
}

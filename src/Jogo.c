#include "RodaFase.h"

int menuEscolha(TGraphs g);
int instrucoes(TGraphs *g);
int opcoes(TGraphs *g);

int main() {
    int escolha;
    TGraphs graphs = {{170, 170, 162, 'Y', 70, 162, 'Y', 70, 145, 'w', 136, 0, 'W', 207, 252, 158, 157, 15, 159, 12, 3}, 'A', 158, '!', 156, 15, 156};
    //system("mode con:cols=80 lines=25"); //Altera o tamanho da janela para caber o jogo perfeitamente
    hideCursor();
    srand(time(0));

    do {
        escolha = menuEscolha(graphs);
        switch (escolha) {
            case 1: escolha = novoJogo(graphs);
                    break;
            case 2: escolha = instrucoes(&graphs);
                    break;
            case 3: escolha = faseCustom(graphs);
                    break;
            case 4: escolha = modoSurvival(graphs);
                    break;
            case 6: escolha = creds();
        }
    } while (escolha != 0 && escolha != -1);
    // Se alguma função que não a confirmaSaida retornar um erro, é necessário imprimir a mensagem de erro
    if (escolha) {
        textcolor(15);
        clrscr();
        printf("Erro 404: arquivo n%co encontrado.\nConfira se o jogo est%c instalado corretamente.", 198, 160);
    	getch();
    }
    return 42;
}


int menuEscolha(TGraphs g) {
    TPlayer p = {15, 22, 0, 0};
    TTiro t = {0, 0};

    TExplo expl[EXPLO_MAX];
    int n_expl = 0;

    struct char_info info;
    int i, escolha = 0, escolhatemp = -1, delay = 0;
    char mov;
    if (!printFromFile(1, 1, g.mapa, "resources/menu.txt")) {
        return -1;
    }
    textcolor(g.playercor);
	gotoxy(p.x, p.y);
	putchar(g.player);
    do {
    	Sleep(1);
        // Movimento do avião
    	p.tempo++;
    	p.tempo_tiro++;
    	// Move o avião para a direita, se ainda não está no limite
	    if(GetAsyncKeyState(0x41) && p.tempo >= TEMPO_PLAYER) { //'a' pressionado
	    	if (p.x > 10) {
	            textcolor(g.mapa.agua);
	            putchxy(p.x, p.y, g.mapa.aguac);
	            p.x--;
	            textcolor(g.playercor);
	            gotoxy(p.x, p.y);
	            putchar(g.player);
	            p.tempo = 0;
	        }
	    }
	    // Move o avião para a esquerda, se ainda não está no limite
	    if(GetAsyncKeyState(0x44) && p.tempo >= TEMPO_PLAYER) { //'d' pressionado
	    	if (p.x < 70) {
                textcolor(g.mapa.agua);
                putchxy(p.x, p.y, g.mapa.aguac);
                p.x++;
                textcolor(g.playercor);
	            gotoxy(p.x, p.y);
	            putchar(g.player);
                p.tempo = 0;
            }
	    }
	    // Verifica se existe um tiro na tela, se não tiver coloca 1
	    if(GetAsyncKeyState(0x4B) && p.tempo_tiro >= TEMPO_TIRO_DELAY && escolhatemp == -1) { //'k' pressionado
	    	if (!t.x) {
                t.x = p.x;
                t.y = p.y - 1;
                t.tempo = 0;
                textcolor(g.tirocor);
	            gotoxy(t.x, t.y);
	            putchar(g.tiro);
	            p.tempo_tiro = 0;
            }
	    }

        if (kbhit()) {
        	switch (tolower(getch())) {
                // Chama a confirmaSaida() e sai (ou não) do jogo
                case 27:    if(confirmaSaida())
                                return 0;
            }
        }

        // Movimento do tiro
        if (t.x) {
        	t.tempo++;
            // Move o tiro apenas após o tempo ter sido incrementado o bastante
            if (t.tempo >= TEMPO_TIRO) {
                t.tempo = 0;
                textcolor(g.mapa.agua);
                putchxy(t.x, t.y, g.mapa.aguac);
                t.y--;
                if(t.y < 1) {
                	t.x = 0;
                	t.y = 0;
                } else {
	                gettext(t.x, t.y, t.x, t.y, &info);
	                // Se a próxima posição é água, move o tiro acima
	                if (info.letter == g.mapa.aguac) {
	                    textcolor(g.tirocor);
			            gotoxy(t.x, t.y);
			            putchar(g.tiro);
	                }
	                // Se não, libera a explosão
	                else {
	                    // Define o que o usuário escolheu, a partir do x do tiro
	                    switch (t.x) {
	                        case 12: case 13: case 15: case 16: case 17: case 18: case 19: case 20: escolhatemp = 1; break;
	                        case 21: case 22: case 24: case 25: case 26: case 27: case 28: case 29: case 32: escolhatemp = 2; break;
	                        case 30: case 31: case 33: case 34: case 35: case 36: case 38: case 41: escolhatemp = 3; break;
	                        case 37: escolhatemp = 4; break;
	                        case 39: case 40: case 42: case 43: case 44: case 45: case 46: case 47: case 50: escolhatemp = 5; break;
	                        case 48: case 49: case 51: case 52: case 53: case 54: case 55: case 56: case 59: escolhatemp = 6; break;
	                        case 57: case 58: case 60: case 61: case 62: case 63: escolhatemp = 0; break;
	                        default: t.y++;
	                    }
	                    expl[n_expl] = addExplo(t.x, t.y, 1);
						if(n_expl < EXPLO_MAX)
							n_expl++;
	                    t.x = t.y = 0;
	                }
            	}
            }
        }
        //Processa as explosões
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

        //Incrementa o delay
        if(escolhatemp > -1) {
        	delay++;
        	if(delay >= TEMPO_EXPLO * 10)
        		if(escolhatemp == 0) {
        			if (confirmaSaida())
						return 0;
					escolhatemp = -1;
					delay = 0;
					gotoxy(57, 20);
					printNaAgua("-> Sair", g.mapa);
				} else if(escolhatemp == 5) {
					highScores();
					escolhatemp = -1;
					delay = 0;
					gotoxy(39, 16);
					printNaAgua("-> Quadro de pontua%c%Oes", g.mapa);
				} else
        			escolha = escolhatemp;
        }
    } while (escolha == 0);
    return escolha;
}

/* Função que mostra os créditos. Retorna 1 se teve sucesso, 0 se o usuário confirmou saída, e -1 se não achou o arquivo */
int creds() {
    TCores cores = {236, 236, 236, 'T', 0, 236, 'I', 0, 236, ' ', 236, 236, 'W', 236, 236, 236, 236, 236, 236, 236, 'V'};
    char escolha;
    if (!printFromFile(1, 1, cores, "resources/creds.txt")) {
        return -1;
    }
    do {
        if ((escolha = tolower(getch())) == 27)
            if (confirmaSaida())
                return 0;
    } while (escolha == 27);
    return 1;
}

/* Função que mostra as instruções e dá acesso às opções. Retorna 1 se teve sucesso, 0 se o usuário confirmou saída, e -1 se não achou o arquivo */
int instrucoes(TGraphs *g) {
    char escolha;
    if (!printFromFile(1, 1, g->mapa, "resources/instrucoes.txt")) {
        return -1;
    }
    do {
        switch (escolha = tolower(getch())) {
            case 27:    if (confirmaSaida())
                            return 0;
                        break;
            case 'p':   explodePonte(*g);
                        switch (escolha = opcoes(g)) {
                            case 0:  return 0;
                            case -1: return -1;
                            case 1:  if (!printFromFile(1, 1, g->mapa, "resources/instrucoes.txt")) {
                                         return -1;
                                     }
                                     escolha = 27;
                        }
        }
    } while (escolha == 27);
    return 1;
}

/* Função para alterar skins e cores. Retorna 1 se teve sucesso, 0 se o usuário confirmou saída, e -1 se não achou o arquivo */
int opcoes(TGraphs *g) {
    int i;
    while (-1) {
        if (!printFromFile(1, 1, g->mapa, "resources/opcoes.txt")) {
            return -1;
        }
        textcolor(g->playercor);
        gotoxy(42, 14);
        putchar(g->player);
        textcolor(g->tirocor);
        gotoxy(46, 14);
        putchar(g->tiro);
        switch (tolower(getch())) {
            case 'p':   explodePonte(*g);
                        textcolor(15);
                        clrscr();
                        gotoxy(33, 12);
                        printf("N%co dessa vez %c", 198, 2);
                        Sleep(1500);
                        for (i = 0; i < 2000; i++) {
                            gotoxy(rand()%78+1, rand()%25+1);
                            textcolor(rand()%255);
                            puts("HUE");
                            gotoxy(1,1);
                            Sleep(2);
                        }
                        break;
            case 'b':   g->mapa.terra = 15;
                        g->mapa.ilha = 15;
                        g->mapa.veg = 15;
                        g->mapa.vegc = 'T';
                        g->mapa.vegtaxa = 0;
                        g->mapa.vegi = 15;
                        g->mapa.vegic = 'I';
                        g->mapa.vegitaxa = 0;
                        g->mapa.agua = 15;
                        g->mapa.aguac = ' ';
                        g->mapa.ponte = 15;
                        g->mapa.road = 0;
                        g->mapa.roadc = 'W';
                        g->mapa.fuel1 = 15;
                        g->mapa.fuel2 = 15;
                        g->mapa.navio = 15;
                        g->mapa.heli = 15;
                        g->mapa.defroad = 15;
                        g->mapa.defagua = 15;
                        g->mapa.vida = 15;
                        g->mapa.vidac = 3;
                        g->playercor = 15;
                        g->tirocor = 15;
                        g->explocor = 15;
                        break;
            case 'f':   g->mapa.terra = 170;
                        g->mapa.ilha = 170;
                        g->mapa.veg = 162;
                        g->mapa.vegc = 'Y';
                        g->mapa.vegtaxa = 70;
                        g->mapa.vegi = 162;
                        g->mapa.vegic = 'Y';
                        g->mapa.vegitaxa = 70;
                        g->mapa.agua = 145;
                        g->mapa.aguac = 'w';
                        g->mapa.ponte = 136;
                        g->mapa.road = 0;
                        g->mapa.roadc = 'W';
                        g->mapa.fuel1 = 207;
                        g->mapa.fuel2 = 252;
                        g->mapa.navio = 158;
                        g->mapa.heli = 157;
                        g->mapa.defroad = 15;
                        g->mapa.defagua = 159;
                        g->mapa.vida = 12;
                        g->mapa.vidac = 3;
                        g->playercor = 158;
                        g->tirocor = 156;
                        g->explocor = 156;
                        break;
            case 't':   g->mapa.terra = 255;
                        g->mapa.ilha = 187;
                        g->mapa.veg = 242;
                        g->mapa.vegc = '*';
                        g->mapa.vegtaxa = 40;
                        g->mapa.vegi = 187;
                        g->mapa.vegic = 'I';
                        g->mapa.vegitaxa = 0;
                        g->mapa.agua = 145;
                        g->mapa.aguac = 'w';
                        g->mapa.ponte = 136;
                        g->mapa.road = 0;
                        g->mapa.roadc = 'W';
                        g->mapa.fuel1 = 207;
                        g->mapa.fuel2 = 252;
                        g->mapa.navio = 158;
                        g->mapa.heli = 157;
                        g->mapa.defroad = 15;
                        g->mapa.defagua = 159;
                        g->mapa.vida = 12;
                        g->mapa.vidac = 3;
                        g->playercor = 158;
                        g->tirocor = 156;
                        g->explocor = 156;
                        break;
            case 'd':   g->mapa.terra = 238;
                        g->mapa.ilha = 238;
                        g->mapa.veg = 226;
                        g->mapa.vegc = 'l';
                        g->mapa.vegtaxa = 10;
                        g->mapa.vegi = 226;
                        g->mapa.vegic = 'l';
                        g->mapa.vegitaxa = 10;
                        g->mapa.agua = 145;
                        g->mapa.aguac = 'w';
                        g->mapa.ponte = 0;
                        g->mapa.road = 102;
                        g->mapa.roadc = 'W';
                        g->mapa.fuel1 = 207;
                        g->mapa.fuel2 = 252;
                        g->mapa.navio = 158;
                        g->mapa.heli = 157;
                        g->mapa.defroad = 96;
                        g->mapa.defagua = 159;
                        g->mapa.vida = 12;
                        g->mapa.vidac = 3;
                        g->playercor = 158;
                        g->tirocor = 156;
                        g->explocor = 156;
                        break;
            case 'a':   switch (tolower(getch())) {
                            case '1': g->player = 19; break;
                            case '2': g->player = 2; break;
                            case '3': g->player = 3; break;
                            case '4': g->player = 24; break;
                            case '5': g->player = 5; break;
                            case '6': g->player = 190; break;
                            case '7': g->player = 208; break;
                            case '8': g->player = 21; break;
                            case '9': g->player = 146; break;
                            case '0': g->player = 14; break;
                            case '-': g->player = 'A'; break;
                            case '=': g->player = '!'; break;
                        }
                        break;
            case 'k':   switch (tolower(getch())) {
                            case '1': g->tiro = 19; break;
                            case '2': g->tiro = 2; break;
                            case '3': g->tiro = 3; break;
                            case '4': g->tiro = 24; break;
                            case '5': g->tiro = 5; break;
                            case '6': g->tiro = 190; break;
                            case '7': g->tiro = 208; break;
                            case '8': g->tiro = 21; break;
                            case '9': g->tiro = 146; break;
                            case '0': g->tiro = 14; break;
                            case '-': g->tiro = 'A'; break;
                            case '=': g->tiro = '!'; break;
                        }
                        break;
            case 27:    if (confirmaSaida())
                            return 0;
                        break;
            default:    return 1;
        }
    }
}


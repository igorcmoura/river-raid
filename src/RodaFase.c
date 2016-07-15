#include "RodaFase.h"

/* Roda o modo campanha. Se tiver rodado normalmente ou der apenas um erro interno retorna 1;
   Se o jogador quer sair retorna 0; E caso ocorra um erro na abertura de um arquivo retorna -1*/
int novoJogo(TGraphs g) {
	TFaseInfo info;
	char nome_arquivo[] = {"resources/default/fasexx.txt"};
	char str_aux[LARG];
	int fase_num;

	char extensao[ALT][LARG];
	unsigned short int extcor[ALT][LARG];
	char conexao[CONALT][LARG];
	unsigned short int concor[ALT][LARG];
	char fase[ALTMAX][LARG];
	unsigned short int cor[ALTMAX][LARG];

	TPlayer pl;

	int score = 0;
	short int vivo = -1; //-1 para entrar no reset

	info.vidas = 3;

	if(!printFromFile(1, 1, g.mapa, "resources/pickdiff.txt"))
		return -1;

	do {
		info.dificuldade = tolower(getch());
		if(info.dificuldade == 27)
			return 1;
	} while(info.dificuldade == 27 || (info.dificuldade != '1' && info.dificuldade != '2' && info.dificuldade != '3' && info.dificuldade != 'p'));

	//Caso da sele��o da dificuldade lun�tico
	if(info.dificuldade == 'p') {
		info.dificuldade = 4;
		textcolor(g.mapa.defagua);
		gotoxy(21, 23);
		sprintf(str_aux, "[p] modo \"Lun%ctico das Pontes\" ativado!", 160);
		printNaAgua(str_aux, g.mapa);
		explodePonte(g);
		getch();
	} else
		info.dificuldade -= 48;

	do {
		do {
			if(vivo == -1) {
				score = 0;
				fase_num = 0;
				vivo = 0;
			}
			fase_num++;
			sprintf(nome_arquivo, "resources/default/fase%d.txt", fase_num);
			if((info.linhas = lerMatriz(nome_arquivo, fase, 1)) == -1)
				return -1;
			if(!enemyGen(fase, info.dificuldade, info.linhas+1))
				return 1;
			if(!initFase(fase, cor, extensao, extcor, conexao, concor, g.mapa, &info, vivo ? 0 : 1))
				return 1;
			vivo = rodaFase(fase, cor, extensao, extcor, conexao, concor, g, &pl, &score, info, vivo ? 0 : 1);
			if(vivo == 2)
				return 1;
		} while(vivo && fase_num < 10);
		vivo = testScore(score, vivo, 1, "");
		if(vivo == -1)
			return -1;
		if(vivo)
			vivo = -1;
	} while(vivo == -1);
	return 1;
}

/* Roda as fases customizadas. Se tiver rodado normalmente ou der apenas um erro interno retorna 1;
   Se o jogador quer sair retorna 0; E caso ocorra um erro na abertura de um arquivo retorna -1*/
int faseCustom(TGraphs g) {
	char nome_arquivo[ALT+11];
	char nome_fase[ALT];
	char str_aux[LARG]; //String de aux�lio

	short int escolha, autoInim = 0;

	char extensao[ALT][LARG];
	unsigned short int extcor[ALT][LARG];
	char fase[ALTMAX][LARG];
	unsigned short int cor[ALTMAX][LARG];
	TFaseInfo info;
	TPlayer p;

	int score = 0;
	short int vivo = -1;

	info.linhas = 0;
	info.dificuldade = 1;
	info.vidas = 1;
	//Faz a leitura do nome da fase
	do {
		//No caso em que j� foi lido o nome, mas o arquivo n�o foi encontrado, pede se deseja tentar outro arquivo
		if(info.linhas == -1) {
			if(!printFromFile(1, 11, g.mapa, "resources/picklevelerro.txt"))
					return -1;

			sprintf(str_aux, "Arquivo \"resources/%s.txt\" n%co encontrado.", nome_arquivo, 198);
			gotoxy(27 - (strlen(nome_arquivo)/2), 11);
			printNaAgua(str_aux, g.mapa);
			do {
				escolha = tolower(getch());
				if(escolha == 27)
					return 1;
			} while(escolha == 27 || (escolha != 's' && escolha != 'n'));
			if(escolha == 'n')
				return 1;
		}
		if(!printFromFile(1, 1, g.mapa, "resources/picklevel1.txt"))
			return -1;

		textcolor(g.mapa.defagua);
		gotoxy(29, 9);
		fgets(nome_arquivo, ALT+2, stdin); //Faz uma leitura com um n�mero limitado de caract�res
		fflush(stdin);
		nome_arquivo[strlen(nome_arquivo)-1] = 0; //Remove o \n do nome do arquivo
		sprintf(str_aux, "resources/custom/%s.txt", nome_arquivo); //Passa para a string auxiliar o nome com a pasta
	} while((info.linhas = lerMatriz(str_aux, fase, 0)) == -1);
	strcpy(nome_fase, nome_arquivo);
	strcpy(nome_arquivo, str_aux); //Se o arquivo foi encontrado passa a string auxiliar para a vari�vel definitiva do nome do arquivo

	//Pede se quer inimigos aleat�rios
	if(!printFromFile(1, 11, g.mapa, "resources/picklevel2.txt"))
		return -1;

	do {
		autoInim = tolower(getch());
		if(autoInim == 27)
			return 1;
	} while(autoInim == 27 || (autoInim != 's' && autoInim != 'n'));
	if(autoInim == 's')
		autoInim = 1;
	else
		autoInim = 0;

	//Pede a info.dificuldade
	if(!printFromFile(1, 1, g.mapa, "resources/pickdiff.txt"))
		return -1;

	do {
		info.dificuldade = tolower(getch());
		if(info.dificuldade == 27)
			return 1;
	} while(info.dificuldade == 27 || (info.dificuldade != '1' && info.dificuldade != '2' && info.dificuldade != '3' && info.dificuldade != 'p'));

	//Caso da sele��o da dificuldade lun�tico
	if(info.dificuldade == 'p') {
		info.dificuldade = 4;
		textcolor(g.mapa.defagua);
		gotoxy(21, 23);
		sprintf(str_aux, "[p] modo \"Lun%ctico das Pontes\" ativado!", 160);
		printNaAgua(str_aux, g.mapa);
		explodePonte(g);
		getch();
	} else
		info.dificuldade -= 48;

	lerMatriz(nome_arquivo, fase, autoInim);
	if(autoInim)
		if(!enemyGen(fase, info.dificuldade, info.linhas+1))
			return 1;
	if(!initFase(fase, cor, extensao, extcor, NULL, NULL, g.mapa, &info, 1))
		return 1;

	do {
		do {
			if(vivo == -1) {
				score = 0;
				vivo = 0;
			}
			vivo = rodaFase(fase, cor, extensao, extcor, NULL, NULL, g, &p, &score, info, 1);
			if(vivo == 2)
				return 1;
		} while(vivo == -1);
		vivo = testScore(score, vivo, 2, nome_fase);
		if(vivo == -1)
			return -1;
		if(vivo)
			vivo = -1;
	} while(vivo == -1);

	return 1;
}

/* Roda o modo survival. Onde ocorre gera��o de fases aleat�rias.
   Se tiver rodado normalmente ou der apenas um erro interno retorna 1; Se o jogador quer sair retorna 0;
   E caso ocorra um erro na abertura de um arquivo retorna -1*/
int modoSurvival(TGraphs g) {
	char extensao[ALT][LARG];
	unsigned short int extcor[ALT][LARG];
	char conexao[CONALT][LARG];
	unsigned short int concor[ALT][LARG];
	char fase[ALTMAX][LARG];
	unsigned short int cor[ALTMAX][LARG];
	TFaseInfo info;
	short int vivo = -1; //-1 para entrar no reset

	TPlayer pl;
	int score;

	info.vidas = 3;

	do {
		do {
			if(vivo == -1) {
				score = 0;
				info.dificuldade = 2;
				vivo = 0;
			}
			if(score > 1000)
				info.dificuldade = 3;
			if(score > 10000)
				info.dificuldade = 4;

			if(!mapGen(fase))
				return -1;
			info.linhas = ALTMAX-1;
			if(!enemyGen(fase, info.dificuldade, info.linhas+1))
				return 1;
			if(!initFase(fase, cor, extensao, extcor, conexao, concor, g.mapa, &info, vivo ? 0 : 1))
				return 1;
			vivo = rodaFase(fase, cor, extensao, extcor, conexao, concor, g, &pl, &score, info, vivo ? 0 : 1);
			if(vivo == 2)
				return 1;
		} while(vivo); //Ainda n�o morreu
		vivo = testScore(score, vivo, 3, "");
		if(vivo == -1)
			return -1;
		if(vivo)
			vivo = -1;
	} while(vivo == -1); //Pediu reset

	return 1;
}

/* Fun��o principal do jogo em si. Recebe a matriz de chars, de cores da fase, struct dos gr�ficos, um ponteiro para o score - que ser� modificado apenas antes de sair da fun��o,
   com um multiplicador conforme a dificuldade -, informa��es da fase e op��o de printar a fase como inicial(redraw). Retorna 1 se o player continua vivo, 0 se ele morreu, -1 se deu reset e 2 se quer sair.

   Todas as vari�veis da struct info e a quantidade de pontos j� devem estar definidas */
int rodaFase(char fase[][LARG], unsigned short int cor[][LARG], char extensao[][LARG], unsigned short int extcor[][LARG], char conexao[][LARG], unsigned short int concor[][LARG], TGraphs g, TPlayer *p, int *pontos, TFaseInfo info, int redraw) {
    int score, reset, vivo; //Vari�vel de pontua��o, para verificar se � para resetar a fase e se o jogador se encontra vivo
    char c; //Caract�r auxiliar

    int ponte_viva = 1; //Vari�vel para verificar se a ponte foi destruida
    int fase_tempo; //Tempo de movimento da fase
    int fase_pos; //Posi��o da fase
    int i, j, k; //Vari�veis auxiliares
    int recarregado; //Vari�vel para teste se o combust�vel j� foi recarregado naquela linha

    int death_delay; //Tempo em que o jogo fica rodando ap�s a morte do jogador

    TTiro tiro[info.tiro_lim]; //Vetor de tiros
    TExplo explo[EXPLO_MAX]; //Vetor de explos�es
    TNavio navio[NAVIO_MAX]; //Vetor de navios
    THeli heli[HELI_MAX]; //Vetor de helic�pteros
    TContador n; //Struct para guardar a quantidade de cada objeto acima

    struct char_info readinfo;
    do { //while vidas > 0
        // Inicializa��es
        n.tiros = 0;
    	n.explo = 0;
    	n.navios = 0;
    	n.heli = 0;

    	p->tempo = 0; //Tempo do movimento lateral do jogador
    	p->tempo_tiro = 0; //Tempo entre cada tiro dado pelo jogador

    	fase_tempo = 0; //Tempo de movimento da fase
    	death_delay = 0; //Tempo ap�s morte ou fim da fase
    	recarregado = 0; //Teste se o combust�vel ja foi recarregado naquela linha

    	score = 0; //In�cio da pontuntua��o parcial

        if(redraw) {
        	//Caso seja recome�o da fase reseta a gasolina pra 100
    		p->gas = 100;
    		//E apaga a �ltima linha para dar espa�o �s estat�sticas de jogo
        	textcolor(15);
    		for(i = 0; i < LARG; i++) {
        		putchxy(i+1, ALT, ' ');
        	}
    	}
        //Printa as estat�sticas
	    textcolor(g.mapa.vida);
		gotoxy(1,ALT);
	    for(i = 0; i < info.vidas; i++) {
	       	putchar(g.mapa.vidac);
	    }
        textcolor(15);
		gotoxy(X_GAS, ALT);
        printGas(p->gas);
        gotoxy(LARG-(intNDigits(score)+intNDigits(*pontos)+8), ALT);
        printf("Pontos:%d/%d", score, *pontos);

        fase_pos = info.linhas;
        c = 'A';
        if(redraw) {
        	//Reseta as stat�sticas do jogador
        	p->x = 40;
        	p->y = 22;
    		p->upcor = g.mapa.agua;
    		p->upchar = g.mapa.aguac;
        	// Print do in�cio da fase
        	gotoxy(1,1);
	        for(i = info.linhas-ALT; i < info.linhas-1; i++) {
	        	j = 0;
	            while(j < LARG) {
	                //Adi��o de navios
	                if(fase[i][j] == 'N') {
	                	if(n.navios < NAVIO_MAX) {
	                		navio[n.navios] = addNavio(j+1, i+1, NAVIO_ESQ, NAVIO_CEN, NAVIO_DIR);
	                		//Printa o novo navio
            				textcolor(g.mapa.navio);
            				putchar(navio[n.navios].esq);
            				putchar(navio[n.navios].cen);
            				putchar(navio[n.navios].cen);
            				putchar(navio[n.navios].dir);
	                		n.navios++;
	                	} else {
	                		for(k = 0; k < 4; k++)
	                		textcolor(g.mapa.agua);
	                		putchar(g.mapa.aguac);
	                	}
	                	j += 4;
            			c = 'N';
	                }
	                //Adi��o de helic�pteros
	                if(fase[i][j] == 'X') {
	                	if(n.heli < HELI_MAX) {
	                		heli[n.heli] = addHeli(j+1, i+1, HELI_CHAR);
	                		//Printa o novo helic�ptero
	                		textcolor(g.mapa.heli);
	                		putchar(heli[n.heli].c);
	                		n.heli++;
	                	} else {
	                		textcolor(g.mapa.agua);
	                		putchar(g.mapa.aguac);
	                	}
	                	j++;
	                	c = 'X';
	                }
	                if(c != fase[i][j]) {
	                    c = fase[i][j];
	                    textcolor(cor[i][j]);
	                }
	                putchar(fase[i][j]);
	                j++;
	            }
	        }
	        textcolor(g.playercor);
	        gotoxy(p->x, p->y);
	        putchar(g.player);
	    	gotoxy(1,1);
	        //Define a posi��o da fase para a pr�xima linha
	        fase_pos = info.linhas-ALT;
	        redraw = 0;
	        getch();
	    } else {
	    	fase_pos += CONALT-1;
	    	//Printa a primeira linha conex�o das fases
	        gotoxy(1,1);
		    i = 0;
		    while(i < LARG) {
		    	//Foi somado anteriormente a fase_pos o n�mero de linhas da conex�o e decrementado 1, ent�o basta tirar o n�mero de linhas para termos o index da conex�o
				if(c != conexao[fase_pos-info.linhas][i]) {
			        c = conexao[fase_pos-info.linhas][i];
					textcolor(concor[fase_pos-info.linhas][i]);
			    }
			    putchar(c);
			    i++;
		    }
	    }

        reset = 0;
        vivo = 1;
        // Loop do jogo
        while (!reset) {
        	Sleep(1);
            // Movimento causado por Teclado
            p->tempo++;
            p->tempo_tiro++;
            if(GetAsyncKeyState(0x41) && p->tempo >= TEMPO_PLAYER - 3*info.dificuldade) { //'a' pressionado
            	if(vivo) {
					textcolor(p->upcor);
					gotoxy(p->x, p->y);
					putchar(p->upchar);
	                p->x--;
	                //Faz o teste de colis�o do jogador
	                vivo = testaColPlayer(p, explo, &n, g, &recarregado, fase_pos, 1, 0);
                }
                p->tempo = 0;
            }
            if(GetAsyncKeyState(0x44) && p->tempo >= TEMPO_PLAYER - 3*info.dificuldade) { //'d' pressionado
            	if(vivo){
                    textcolor(p->upcor);
					gotoxy(p->x, p->y);
					putchar(p->upchar);
	                p->x++;
	                //Faz o teste de colis�o do jogador
	                vivo = testaColPlayer(p, explo, &n, g, &recarregado, fase_pos, -1, 0);
                }
                p->tempo = 0;
        	}
        	if(GetAsyncKeyState(0x4B) && p->tempo_tiro >= TEMPO_TIRO_DELAY - 3*info.dificuldade) { //'k' pressionado
        		if(n.tiros < info.tiro_lim && vivo) {
                    tiro[n.tiros] = addTiro(p->x, p->y-1);
                    n.tiros++;
                }
                p->tempo_tiro = 0;
        	}
        	if (kbhit()) {
                switch (tolower(getch())) {
                	case 'r':
						if (confirmaReset())
                        	reset = 1;
                        break;
                    case 13:
						pause();
                        break;
                    case 27:
						if (confirmaSaida())
                        	return 2;
                }
            }

            //Movimento da fase
            fase_tempo++;
            if(fase_tempo >= (info.tempo.fase) && vivo) { //Movimenta a fase apenas se o jogador ainda estive vivo
            	//Apaga os tiros antes de mover o mapa
            	textcolor(g.mapa.agua);
            	for(i = 0; i < n.tiros; i++) {
            		putchxy(tiro[i].x, tiro[i].y, g.mapa.aguac);
            	}
            	//Faz o movimento da fase se esta ainda n�o acabou
            	movetext(1, 1, LARG, ALT-2, 1, 2);
	            fase_pos--;

	            //Remove os inimigos que j� sairam da fase
	            i = 0;
	            while(i < n.navios) {
	            	if(navio[i].y > fase_pos+ALT-1) {
	            		n.navios--;
	            		if(i != n.navios) {
	            			navio[i] = navio[n.navios];
	            			i--;
	            		}
	            	}
	            	i++;
	            }
	            i = 0;
	            while(i < n.heli) {
	            	if(heli[i].y > fase_pos+ALT-1) {
	            		n.heli--;
	            		if(i != n.heli) {
	            			heli[i] = heli[n.heli];
	            			i--;
	            		}
	            	}
	            	i++;
				}

	            //Testa colis�o dos tiros
	            i = 0;
	            while(i < n.tiros) {
	            	i -= testaColTiro(tiro, i, explo, &n, g, fase_pos);
	            	i++;
	            }
	            //Recoloca os tiros na tela
	            for(i = 0; i < n.tiros; i++) {
	            	textcolor(g.tirocor);
	            	gotoxy(tiro[i].x, tiro[i].y);
	            	putchar(g.tiro);
	            }

				//Faz o movimento do jogador
	            textcolor(p->upcor);
	            gotoxy(p->x, p->y+1);
	            putchar(p->upchar);
	            vivo = testaColPlayer(p, explo, &n, g, &recarregado, fase_pos, 0, 1);

            	//Printa a nova parte da fase
	            c = 'A';
	            if(fase_pos >= info.linhas) {
	            	//Printa a conex�o das fases
	            	gotoxy(1,1);
		            i = 0;
		            while(i < LARG) {
		            	//Foi somado anteriormente a fase_pos o n�mero de linhas da conex�o e decrementado 1, ent�o basta tirar o n�mero de linhas para termos o index da conex�o
			            if(c != conexao[fase_pos-info.linhas][i]) {
			            	c = conexao[fase_pos-info.linhas][i];
							textcolor(concor[fase_pos-info.linhas][i]);
			            }
			            putchar(c);
			            i++;
		            }
	            } else if(fase_pos >= 0) {
            		if((int)((p->gas - 0.4)/10) < (int)(p->gas/10)) {
			            textcolor(15);
			            gotoxy(X_GAS, ALT);
			            printGas(p->gas);
		            }
		            p->gas -= 0.4;//Reduz a gasolina por movimento
		            recarregado = 0;//Reseta o estado de recarregado para poder recarregar

	            	gotoxy(1,1);
	            	i = 0;
	            	while(i < LARG) {
		                //Adi��o de navios
		                if(fase[fase_pos][i] == 'N') {
		                	if(n.navios < NAVIO_MAX) {
		                		navio[n.navios] = addNavio(i+1, fase_pos+1, NAVIO_ESQ, NAVIO_CEN, NAVIO_DIR);
		                		//Printa o novo navio
	            				textcolor(g.mapa.navio);
	            				putchar(navio[n.navios].esq);
	            				putchar(navio[n.navios].cen);
	            				putchar(navio[n.navios].cen);
	            				putchar(navio[n.navios].dir);
		                		n.navios++;
		                	} else {
		                		for(j = 0; j < 4; j++) {
									textcolor(g.mapa.agua);
									putchar(g.mapa.aguac);
			                	}
		                	}
		                	i += 4;
		                	c = 'N';
		                }
		                //Adi��o de helic�pteros
		                if(fase[fase_pos][i] == 'X') {
		                	if(n.heli < HELI_MAX) {
		                		heli[n.heli] = addHeli(i+1, fase_pos+1, HELI_CHAR);
		                		//Printa o novo helic�ptero
		                		textcolor(g.mapa.heli);
		                		putchar(heli[n.heli].c);
		                		n.heli++;
		                	} else {
		                		textcolor(g.mapa.agua);
								putchar(g.mapa.aguac);
		                	}
		                	i++;
		                	c = 'X';
		                }
		                if(c != fase[fase_pos][i]) {
		                    c = fase[fase_pos][i];
		                    textcolor(cor[fase_pos][i]);
		                }
		                putchar(c);
		                i++;
	            	}
	            } else {
	            	if(-fase_pos < ALT) {
	            		//Printa a extens�o da fase
		            	gotoxy(1,1);
		            	i = 0;
		            	while(i < LARG) {
		            		//Como fase_pos � negativo a partir daqui a posi��o da matriz extens�o fica ALT(25)+fase_pos < 25
			                if(c != extensao[ALT+fase_pos][i]) {
			                    c = extensao[ALT+fase_pos][i];
			                    textcolor(extcor[ALT+fase_pos][i]);
			                }
			                putchar(c);
			                i++;
		            	}
	            	} else {
	            		ponte_viva = 0; //Se certifica de explodir a ponte(passar a fase) em caso de erro
	            		reset = 1; //Finaliza o loop dessa fase
	            	}
	            }

	            fase_tempo = 0;
            }

            //Processamento dos tiros
            i = 0;
            while(i < n.tiros) {
            	//Verifica se os tiros rec�m adicionado j� colidiram
            	if(tiro[i].tempo == -1) {
            		testaColTiro(tiro, i, explo, &n, g, fase_pos);
            		tiro[i].tempo++;
            		i--;
            	} else {
            		tiro[i].tempo++;
            		if(tiro[i].tempo >= TEMPO_TIRO) {
            			tiro[i].tempo = 0;
            			tiro[i].y--;
            			if(tiro[i].y < 1) {
            				n.tiros--;
            				textcolor(g.mapa.agua);
            				putchxy(tiro[i].x, 1, g.mapa.aguac);
            				if(i != n.tiros) {
            					tiro[i] = tiro[n.tiros];
            					i--;
            				}
            			} else {
            				j = i;
            				j -= testaColTiro(tiro, j, explo, &n, g, fase_pos);
            				textcolor(g.mapa.agua);
            				putchxy(tiro[i].x, tiro[i].y+1, g.mapa.aguac);
            				i = j;
            			}
            		}
            	}
            	i++;
            }

            //Processamento dos inimigos
            //Navios
            i = 0;
            while(i < n.navios) {
            	for(j = navio[i].x; j < navio[i].x+4; j++) {
            		//Teste se h� um tiro, uma explos�o ou avi�o no meio do navio
            		gettext(j, navio[i].y-fase_pos, j, navio[i].y-fase_pos, &readinfo);
            		if(readinfo.letter == g.player || readinfo.letter == g.tiro || readinfo.letter == g.explo) {
            			if(navio[i].vivo) {
            				score += 30;
							navio[i].vivo = 0;
            			}
            		}
            	}
            	if(navio[i].vivo) {
	            	if(navio[i].tempo == -1) {
	            		//Printa o novo navio
	            		textcolor(g.mapa.navio);
	            		gotoxy(navio[i].x, navio[i].y-fase_pos);
	            		putchar(navio[i].esq);
	            		putchar(navio[i].cen);
	            		putchar(navio[i].cen);
	            		putchar(navio[i].dir);
	            		navio[i].tempo++;
	            		i--;
	            	} else {
	            		navio[i].tempo++;
	            		if(navio[i].tempo >= info.tempo.inimigos) {
		            		//Verifica se j� est� em movimento
		            		if(navio[i].mov) {
		            			//Testa se tem algo na dire��o do movimento dele
		            			//(int)(navio[i].x+1.5+(mov*2.5)) -> Express�o para verificar a posi��o do movimento
		            			gettext((int)(navio[i].x+1.5+(navio[i].mov*2.5)), navio[i].y-fase_pos, (int)(navio[i].x+1.5+(navio[i].mov*2.5)), navio[i].y-fase_pos, &readinfo);
		            			if(readinfo.letter != g.mapa.aguac && readinfo.letter != g.player) {
		            				//Se tiver algo que n�o seja �gua ou o jogador, muda a dire��o do movimento
		            				navio[i].mov = -navio[i].mov;
		            			} else {
		            				//Avan�a
		            				textcolor(g.mapa.agua);
		            				putchxy((int)(navio[i].x+1.5-(navio[i].mov*1.5)), navio[i].y-fase_pos, g.mapa.aguac);
		            				textcolor(g.mapa.navio);
		            				gotoxy(navio[i].x+navio[i].mov, navio[i].y-fase_pos);
		            				putchar(navio[i].esq);
		            				putchar(navio[i].cen);
		            				putchar(navio[i].cen);
		            				putchar(navio[i].dir);
		            				if(readinfo.letter == g.player) {
		            					vivo = 0;
		            					if(n.explo < EXPLO_MAX) {
		            						explo[n.explo] = addExplo((int)(navio[i].x+1.5+(navio[i].mov*2.5)), navio[i].y, 1);
		            						n.explo++;
		            					}
		            				}
		            				navio[i].x += navio[i].mov;
		            			}
		            		} else {
		            			//Testa se deve iniciar o movimento
		            			if(randomTrue(30 + 10*info.dificuldade)) {
		            				if(randomTrue(50))
		            					navio[i].mov = 1;
		            				else
		            					navio[i].mov = -1;
		            			}
		            		}
		            		navio[i].tempo = 0;
	            		}
            		}
            	} else {
					n.navios--;
            		if(i != n.navios) {
						navio[i] = navio[n.navios];
						i--;
					}
            	}
            	i++;
            }

            //Helic�pteros
            i = 0;
            while(i < n.heli) {
            	//Teste se tem um tiro, uma explos�o ou avi�o no meio do navio
            	gettext(heli[i].x, heli[i].y-fase_pos, heli[i].x, heli[i].y-fase_pos, &readinfo);
            	if(readinfo.letter == g.player || readinfo.letter == g.tiro || readinfo.letter == g.explo) {
            		if(heli[i].vivo) {
            			score += 60;
						heli[i].vivo = 0;
            		}
            	}
            	if(heli[i].vivo) {
	            	if(heli[i].tempo == -1) {
	            		//Printa o novo helic�ptero
	            		textcolor(g.mapa.heli);
	            		gotoxy(heli[i].x, heli[i].y-fase_pos);
	            		putchar(heli[i].c);
	            		heli[i].tempo++;
	            		i--;
	            	} else {
	            		heli[i].tempo++;
	            		if(heli[i].tempo >= info.tempo.inimigos) {
		            		//Verifica se j� est� em movimento
		            		if(heli[i].mov) {
		            			//Testa se tem algo na dire��o do movimento dele
		            			gettext(heli[i].x+heli[i].mov, heli[i].y-fase_pos, heli[i].x+heli[i].mov, heli[i].y-fase_pos, &readinfo);
		            			if(readinfo.letter != g.mapa.aguac && readinfo.letter != g.player) {
		            				//Se tiver algo que n�o seja �gua ou o jogador, muda a dire��o do movimento
		            				heli[i].mov = -heli[i].mov;
		            			} else {
		            				//Avan�a
		            				textcolor(g.mapa.agua);
		            				putchxy(heli[i].x, heli[i].y-fase_pos, g.mapa.aguac);
		            				textcolor(g.mapa.heli);
									gotoxy(heli[i].x+heli[i].mov, heli[i].y-fase_pos);
									putchar(heli[i].c);
		            				if(readinfo.letter == g.player) {
		            					vivo = 0;
		            					if(n.explo < EXPLO_MAX) {
		            						explo[n.explo] = addExplo(heli[i].x+heli[i].mov, heli[i].y, 1);
		            						n.explo++;
		            					}
		            				}
		            				heli[i].x += heli[i].mov;
		            			}
		            		} else {
		            			//Testa se deve iniciar o movimento
		            			if(randomTrue(40))
		            				if(randomTrue(50))
		            					heli[i].mov = 1;
		            				else
		            					heli[i].mov = -1;
		            		}
		            		heli[i].tempo = 0;
	            		}
            		}
            	} else {
					n.heli--;
            		if(i != n.heli) {
						heli[i] = heli[n.heli];
						i--;
					}
            	}
            	i++;
            }

            //Processamento das explos�es
            i = 0;
            while(i < n.explo) {
            	if(explo[i].y < fase_pos+1 || explo[i].y > fase_pos+ALT-1) {
            		n.explo--;
		       		if(i != n.explo) {
		       			explo[i] = explo[n.explo];
		       			i--;
		       		}
            	} else {
            		textcolor(g.explocor);
					putchxy(explo[i].x, explo[i].y-fase_pos, g.explo);
			       	if(explo[i].tempo == TEMPO_EXPLO/4) {
			       		//Cria novas explos�es ao redor
			       		//A esquerda
			       		testaColExplo(explo, p, i, &n, g, &ponte_viva, fase_pos, -1, 0);

			       		//A direita
			       		testaColExplo(explo, p, i, &n, g, &ponte_viva, fase_pos, 1, 0);

			       		//Acima
			       		testaColExplo(explo, p, i, &n, g, &ponte_viva, fase_pos, 0, -1);

			       		//Abaixo
			       		testaColExplo(explo, p, i, &n, g, &ponte_viva, fase_pos, 0, 1);
				  	}
			       	explo[i].tempo++;
			       	if(explo[i].tempo >= TEMPO_EXPLO) {
			       		if(explo[i].x == p->x && explo[i].y-fase_pos == p->y && vivo) {
			       			textcolor(g.playercor);
			       			gotoxy(p->x, p->y);
			       			putchar(g.player);
			       		} else {
				       		textcolor(g.mapa.agua);
				       		gotoxy(explo[i].x, explo[i].y-fase_pos);
				       		putchar(g.mapa.aguac);
			       		}
			       		n.explo--;
			       		if(i != n.explo) {
			       			explo[i] = explo[n.explo];
			       			i--;
			       		}
			       	}
            	}
            	i++;
            }

            //Processa gasolina
            if(p->gas > 100)
            	p->gas = 100;
            if(p->gas <= 0 && vivo) {
            	p->gas = 0;
            	vivo = 0;
            	if(n.explo < EXPLO_MAX) {
            		explo[n.explo] = addExplo(p->x, p->y+fase_pos, 1);
            		n.explo++;
            	}

            }

            //Se morto faz a contagem de delay at� resetar	DONE
            if(!vivo) {
            	if(death_delay == 0) {
            		info.vidas--;
            		gotoxy(1, ALT);
            		textcolor(g.mapa.vida);
            		for(i = 0; i < info.vidas; i++) {
            			putchar(g.mapa.vidac);
	         		}
	         		putchar(' ');
	         		gotoxy(X_GAS, ALT);
	         		textcolor(15);
	         		printGas(p->gas);
            	}
            	death_delay++;
            }
            if(death_delay >= TEMPO_DEATH_DELAY) {
            	reset = 1;
            }
			//Printa estat�sticas do jogo
			textcolor(15);
	        gotoxy(LARG-(intNDigits(score)+intNDigits(*pontos)+8), ALT);
	        printf("Pontos:%d/%d", score, *pontos);
        } //while !reset
        redraw = 1;//Come�a a fase novamente com o avi�o parado no in�cio da fase
    } while (info.vidas > 0 && ponte_viva);
    //Limpa os tiros e explos�es da tela
    textcolor(g.mapa.agua);
    for(i = 0; i < n.tiros; i++) {
    	gotoxy(tiro[i].x, tiro[i].y);
    	putchar(g.mapa.aguac);
    }
    for(i = 0; i < n.explo; i++) {
    	gotoxy(explo[i].x, explo[i].y);
    	putchar(g.mapa.aguac);
    }
    *pontos += score * info.dificuldade;
    score = 0;
    textcolor(15);
	gotoxy(LARG-(intNDigits(score)+intNDigits(*pontos)+8), ALT);
	printf("Pontos:%d/%d", score, *pontos);
	if(ponte_viva)
    	return 0;
    if(vivo)
    	return 1;
    return -1;
}

/* Faz a inicializa��o da fase, ou seja, tranforma alguns peda�os da matriz fase, gera a matriz cor e define
   o tempo de movimento de cada objeto. Se ocorre algum erro retorna 0; Sen�o retorna 1*/
int initFase(char fase[][LARG], unsigned short int cor[][LARG], char extensao[][LARG], unsigned short int extcor[][LARG], char conexao[][LARG], unsigned short int concor[][LARG], TCores cores, TFaseInfo *info, int redraw) {
	int i, j, k;
	int processado;

	//Processa os tempos e o limite de tiros de acordo com a dificuldade
	info->tempo.fase = 180 - (info->dificuldade * 40);
	info->tempo.inimigos = 300 - (info->dificuldade * 50);
	info->tiro_lim = 9 - (info->dificuldade * 2);

	if(fase[info->linhas][0] == 0)
		info->linhas--;

	//Pega as posi��es x da esquerda e direita do come�o da fase
	info->xie = MINT;
	while(fase[info->linhas][info->xie] == 'T' && info->xie < LARG)
		info->xie++;

	info->xid = LARG-MINT;
	while(fase[info->linhas][info->xid] == 'T' && info->xid > 0)
		info->xid--;

	//Caso seja uma nova fase ligada a anterior, gera a conex�o
	if(!redraw) {
		if(info->xie > info->xid) {
	    	textcolor(15);
		    clrscr();
		    printf("Imposs%cvel criar conex%co com a fase. Verifique se a mesma possui abertura no seu in%ccio.", 161, 198, 161);
		    getch();
		    return 0;
	    }

		//Manda os pontos x finais da �ltima fase e x inicias da nova fase
		buildConnection(conexao, info->xfe, info->xfd, info->xie, info->xid);
		//Gera a matriz de cores da extens�o
		for(i = 0; i < CONALT; i++) {
			j = 0;
			while(j < LARG && conexao[i][j] != 0) {
				switch(conexao[i][j]) {
					case 'T':
						concor[i][j] = cores.terra;
						if(randomTrue(cores.vegtaxa)) {
							concor[i][j] = cores.veg;
							conexao[i][j] = cores.vegc;
						}
						break;
					case ' ':
						concor[i][j] = cores.agua;
						conexao[i][j] = cores.aguac;
						break;
				}
				j++;
			}
		}
	}

	//Pega as posi��es x da esquerda e direita do fim da nova fase
	info->xfe = MINT;
	while(fase[0][info->xfe] == 'T' && info->xfe < LARG)
		info->xfe++;

	info->xfd = LARG-MINT;
	while(fase[0][info->xfd] == 'T' && info->xfd > 0)
		info->xfd--;

	//Gera a extens�o da nova fase
	buildExtension(extensao, &info->xfe, &info->xfd, -1);
	//Gera a matriz de cores da extens�o
	for(i = 0; i < ALT; i++) {
		j = 0;
		while(j < LARG && extensao[i][j] != 0) {
			switch(extensao[i][j]) {
				case 'T':
					extcor[i][j] = cores.terra;
					if(randomTrue(cores.vegtaxa)) {
						extcor[i][j] = cores.veg;
						extensao[i][j] = cores.vegc;
					}
					break;
				case ' ':
					extcor[i][j] = cores.agua;
					extensao[i][j] = cores.aguac;
					break;
			}
			j++;
		}
	}

	info->linhas++;

	//Faz a procura da ponte
	processado = 0;
	i = 0;
	while(!processado) {
		j = 0;
		while(j < LARG) {
			if(fase[i][j] == 'P') {
				info->ponteY = i;
				processado = 1;
				for(j = 0; j < LARG; j++)
					if(fase[i][j] != 'P')
						fase[i][j] = 'W';
			}
			j++;
		}
		i++;
		if(processado) {
			j = 0;
			while(j < LARG) {
				if(fase[i][j] == 'P') {
					for(j = 0; j < LARG; j++) {
						if(fase[i][j] != 'P')
							fase[i][j] = 'W';
					}
					i--;
					for(j = 2; j < LARG; j += 3) {
						if(fase[i][j] != 'P')
							fase[i][j] = '_';
					}
				}
				j++;
			}
		}
		if(i >= info->linhas) {
			textcolor(15);
			clrscr();
			printf("Erro, nenhuma ponte encontrada.");
			getch();
			return 0;
		}
	}

	//Gera as ilhas
	for(i = 0; i < info->linhas; i++) {
		j = 0;
		while(j < LARG) {
			//Procura onde come�a a �gua
			if(fase[i][j] != 'T') {
				while(j < LARG) {
					//Procura onde come�a a nova terra
					if(fase[i][j] == 'T') {
						k = j;
						while(j < LARG) {
							//V� se existe �gua ap�s essa nova terra
							if(fase[i][j] != 'T') {
								//Se existe preenche com I
								for(; k < j; k++)
									fase[i][k] = 'I';

								//E procura a nova terra novamente
								while(fase[i][j] != 'T' && j < LARG)
									j++;
								k = j;
							}
							j++;
						}
					}
					j++;
				}
			}
			j++;
		}
	}

	//Gera a matriz de cores
	for(i = 0; i < info->linhas; i++) {
		j = 0;
		while(j < LARG && fase[i][j] != 0) {
			switch(fase[i][j]) {
				case 'T':
					cor[i][j] = cores.terra;
					if(randomTrue(cores.vegtaxa)) {
						cor[i][j] = cores.veg;
						fase[i][j] = cores.vegc;
					}
					break;
				case 'I':
					cor[i][j] = cores.ilha;
					if(randomTrue(cores.vegitaxa)) {
						cor[i][j] = cores.vegi;
						fase[i][j] = cores.vegic;
					}
					break;
				case ' ':
					cor[i][j] = cores.agua;
					fase[i][j] = cores.aguac;
					break;
				case 'P':
					cor[i][j] = cores.ponte;
					break;
				case 'W':
					cor[i][j] = cores.road;
					fase[i][j] = cores.roadc;
					break;
				case '_':
					cor[i][j] = cores.defroad;
					break;
				case 'G':
					cor[i][j] = cores.fuel1;
					cor[i+1][j] = cores.fuel2;
					cor[i+2][j] = cores.fuel1;
					cor[i+3][j] = cores.fuel2;
					fase[i][j] = 'F';
					fase[i+1][j] = 'U';
					fase[i+2][j] = 'E';
					fase[i+3][j] = 'L';
					break;
				case 'X':
					cor[i][j] = cores.heli;
					break;
				case 'N':
					cor[i][j] = cores.navio;
					break;
			}
			j++;
		}
	}

	return 1;
}

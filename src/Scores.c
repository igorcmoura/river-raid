#include "Scores.h"

/* Faz o teste se o jogador entrou nos highscores, e depois pede se deseja reiniciar.
   Retorna 1 caso queira, 0 caso não queira e -1 em caso de erro */
int testScore(int score, short int vivo, short int modo_jogo, char nome_fase[]) {
	TPont pont, leitor[10];
	FILE *f;

	TCores cores = {15, 15, 15, 'T', 0, 15, 'T', 0, 15, ' ', 15, 15, 'W', 15, 15, 15, 15, 15, 15, 15, 'T'};

	int i, pos, cont, escolha, aceito = 0; //aceito - testa se o jogador foi aceito nos highscores

	//Testa se o arquivo ainda não existe
	if(access("highscores.bin", F_OK) == -1) {
		//Caso o arquivo não exista cria um novo
		f = fopen("highscores.bin", "wb");
		if(f == NULL) {
			textcolor(15);
			clrscr();
			gotoxy(1,1);
			printf("Erro! N%co foi poss%cvel criar \"highscores.bin\"", 198, 161);
			getch();
			return -1;
		}
		//Cria o espaço para todos os highscores
		strcpy(pont.nome_jogador, "");
		strcpy(pont.nome_fase, "");
		pont.modo_jogo = 0;
		pont.score = 0;
		for(i = 0; i < 30; i ++)
			fwrite(&pont, sizeof(TPont), 1, f);
		fclose(f);
	}

	//Abre o arquivo
	f = fopen("highscores.bin", "r+b");
	if(f == NULL) {
		textcolor(15);
		clrscr();
		gotoxy(1,1);
		printf("Erro! N%co foi poss%cvel abrir \"highscores.bin\"", 198, 161);
		getch();
		return -1;
	}

	//Move o ponteiro para a área correspondente ao modo de jogo
	fseek(f, 10*(modo_jogo-1)*sizeof(TPont), SEEK_SET);
	i = 0;
	while(i < 10 && !aceito) {
		if(fread(&leitor[0], sizeof(TPont), 1, f) != 1) {
			aceito = 1;
		} else {
			if(leitor[0].score < score)
				aceito = 1;
		}
		i++;
	}

	if(aceito) {
		if(!printFromFile(20, 8, cores, "resources/congrats.txt")) {
			fclose(f); //Antes fecha o arquivo
			// Se não achou o arquivo, limpa a tela, printa um erro e confirma saída
	        textcolor(15);
	        clrscr();
	        printf("Erro 404: arquivo \"resources/congrats.txt\" n%co encontrado.\nConfira se o jogo est%c instalado corretamente.", 198, 160);
	        getch();
	        return -1;
		}
		gotoxy(22, 13);
		fgets(pont.nome_jogador, NOME_MAX, stdin); //Faz uma leitura com um número limitado de caractéres
		fflush(stdin);
		pont.nome_jogador[strlen(pont.nome_jogador)-1] = 0; //Tira o \n

		//Pega os outros dados
		strcpy(pont.nome_fase, nome_fase);
		pont.modo_jogo = modo_jogo;
		pont.score = score;

		//Pega a posição na qual deve ser gravada a nova pontuação
		pos = ftell(f)-sizeof(TPont);

		//Pega as pontuaçõe posteriores
		cont = 1;
		while(i < 10) {
			fread(&leitor[cont], sizeof(TPont), 1, f);
			cont++;
			i++;
		}

		//Volta o ponteiro para a posição a ser gravada
		fseek(f, pos, SEEK_SET);
		i -= cont;
		cont = 0;

		//Grava a nova pontuação
		fwrite(&pont, sizeof(TPont), 1, f);
		i++;

		//Empurra as outras para baixo
		while(i < 10) {
			fwrite(&leitor[cont], sizeof(TPont), 1, f);
			cont++;
			i++;
		}
	}
	fclose(f);

	if(vivo) {
		//Se terminou o jogo vivo mostra a tela de ganhador
		if(!printFromFile(1, 1, cores, "resources/youwin.txt")) {
			// Se não achou o arquivo, limpa a tela, printa um erro e confirma saída
		    textcolor(15);
		    clrscr();
		    printf("Erro 404: arquivo \"resources/youwin.txt\" n%co encontrado.\nConfira se o jogo est%c instalado corretamente.", 198, 160);
		    getch();
		    return -1;
		}
		textcolor(15);
		gotoxy(59, 21);
		printf("%d", score);
		getch();
		return 0;
	}
	if(!printFromFile(1, 1, cores, "resources/gameover.txt")) {
		// Se não achou o arquivo, limpa a tela, printa um erro e confirma saída
	    textcolor(15);
	    clrscr();
	    printf("Erro 404: arquivo \"resources/gameover.txt\" n%co encontrado.\nConfira se o jogo est%c instalado corretamente.", 198, 160);
	    getch();
	    return -1;
	}
	textcolor(15);
	gotoxy(60, 20);
	printf("%d", score);
	//Pega a resposta se o jogador deseja jogar novamente
	do {
	    escolha = tolower(getch());
	} while(escolha != 'n' && escolha != 's' && escolha != 27);
	if(escolha == 's')
		return 1;
	return 0;
}

/* Mostra a tela dos highscores, retorna 1 caso rodar normal, retorna 0 se ocorrer erro. */
int highScores() {
	TPont leitor, pontJ[10], pontC[10], pontS[10]; //Vetor de pontuações para o modo campanha(pontJ), custom(pontC) e survival(pontS)
	FILE *f;
	char texto[LARG];
	int i, j, k, x, y, contJ, contC, contS; //cont jogo(campanha), cont custom e cont survival, contadores de número de scores salvos para cada um

	TCores cores = {15, 15, 15, 'T', 0, 15, 'T', 0, 15, ' ', 15, 15, 'W', 15, 15, 15, 15, 15, 15, 15, 'T'};
	struct char_info holdscreen[2000];

	contJ = contC = contS = 0;

	f = fopen("highscores.bin", "r");
	if(f != NULL) {
		while(!feof(f)) {
			if(fread(&leitor, sizeof(TPont), 1, f) == 1) {
				if(leitor.modo_jogo == 1) {
					pontJ[contJ] = leitor;
					contJ++;
				} else if(leitor.modo_jogo == 2) {
					pontC[contC] = leitor;
					contC++;
				} else if(leitor.modo_jogo == 3) {
					pontS[contS] = leitor;
					contS++;
				}
			}
		}
	}
	fclose(f);
	//Salva a tela atual
    gettext(1, 1, LARG, ALT, holdscreen);
    textcolor(15);
	if(contJ == 0 && contC == 0 && contS == 0) {
		sprintf(texto, "Nenhuma pontua%c%co salva.", 135, 198);
		x = LARG/2 - strlen(texto)/2 - 2;
		for(i = 0; i < 2; i++) {
			y = ALT/2 - 2 + 4*i;
			gotoxy(x, y);
			putchar(' ');
			for(j = 0; j < strlen(texto) + 2; j++)
				putchar('=');
			putchar(' ');
		}
		y -= 3;
		for(i = 0; i < 3; i++) {
			gotoxy(x, y+i);
			putchar('|');
			for(j = 0; j < strlen(texto) + 2; j++)
				putchar(' ');
			putchar('|');
		}
		y++;
		x += 2;
		gotoxy(x, y);
		printf("%s", texto);
	} else {
		if(!printFromFile(1, 1, cores, "resources/highscores.txt")) {
	        // Se não achou o arquivo, limpa a tela e printa um erro
	        textcolor(15);
	        clrscr();
	        printf("Erro 404: arquivo \"resources/highscores.txt\" n%co encontrado.\nConfira se o jogo est%c instalado corretamente.", 198, 160);
	        getch();
	        return 0;
	    }
		for(i = 0; i < contJ; i++) {
			gotoxy(8, 5 + i*2);
			puts(pontJ[i].nome_jogador);
			gotoxy(27 - intNDigits(pontJ[i].score), 5 + i*2);
			printf("%d", pontJ[i].score);
		}
		for(i = 0; i < contS; i++) {
			gotoxy(34, 5 + i*2);
			puts(pontS[i].nome_jogador);
			gotoxy(53 - intNDigits(pontS[i].score), 5 + i*2);
			printf("%d", pontS[i].score);
		}
		for(i = 0; i < contC; i++) {
			gotoxy(60, 5 + i*2);
			puts(pontC[i].nome_jogador);
			gotoxy(79 - intNDigits(pontC[i].score), 5 + i*2);
			printf("%d", pontC[i].score);
		}
	}
	getch();
	//Restaura a tela
	puttext(1, 1, LARG, ALT, holdscreen);
	return 1;
}

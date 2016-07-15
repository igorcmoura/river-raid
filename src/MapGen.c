#include "MapGen.h"

#include <stdlib.h>

/* Recebe um n�mero de 0 a 100 com at� uma casa decimal e retorna verdadeiro com essa porcentagem de chance */
int randomTrue(float percent_true) {
    int r;
    r = rand()%1000;
    if (r < percent_true*10)
        return 1;
    else
        return 0;
}

/* Recebe as taxas de gera��o de cada entidade e retorna 'N', 'X' ou 'G' com base nelas. As taxas n�o devem, somadas, passar de 100 */
char randomEnemy(int navio, int heli, int gas) {
    int n;
    n = rand()%100;
    if (n < navio)
        return 'N';
    else
        if (n < navio+heli)
            return 'X';
        else
            if (n < navio+heli+gas)
                return 'G';
            else
                return 'I';
}

/* Recebe a altura e uma matriz padr�o e formata ela, removendo o lixo */
void limpaMapa(char mapa[][LARG], int alt) {
    int i, j;
    for (i = 0; i < alt; i++) {
        for (j = 0; j < MINT; j++)
            mapa[i][j] = 'T';
        for (; j < LARG - MINT; j++)
            mapa[i][j] = ' ';
        for (; j < LARG; j++)
            mapa[i][j] = 'T';
    }
}

/* Fun��o usada pela mapGen */
void buildBorders(char mapa[][LARG]) {
    // e - esquerda, ptoe - ponto anterior esquerda; d igual; dir - dire��o da varia��o da borda; var - varia��o da borda;
    int i, e, d, dir, var, ptoe, ptod, inverte = 0;

    // Gera a primeira, linha seed
    i = ALTMAX-1;
    // Lado esquerdo
    e = MINT;
    while (randomTrue(SEEDSIZE) && e < LARG/3) {
        mapa[i][e] = 'T';
        e++;
    }
    // Lado direito
    d = LARG-MINT;
    while (randomTrue(SEEDSIZE) && d > 2*LARG/3) {
        mapa[i][d] = 'T';
        d--;
    }
    ptoe = e;
    ptod = d;

    // Gera o resto das bordas
    for (i--; i >= 0; i--) {
        // Cada passagem de linha ocorre no sentido contr�rio da anterior, para n�o ter "bias" (a vers�o sem inverte estava criando mapas com muita terra na esquerda)
        if (!inverte) {
            // Lado esquerdo
            // Escolha da dire��o; se a �ltima linha n�o cresceu mais que a borda, aumenta a chance disso acontecer
            if (ptoe > MINT)
                dir = rand()%2;
            else
                dir = rand()%4;
            // Escolha do tamanho da varia��o
            var = rand()%VARMAX;
            if (dir == 0)
                var *= -1;
            // 'e' deve obedecer v�rias condi��es para o pr�ximo 'T' ser colocado: respeitar a varia��o e deixar espa�o o bastante � direita tanto na linha corrente quando na anterior
            e = MINT;
            while (e <= ptoe + var && ptod - e >= MINW && mapa[i][e+MINW] == ' ') {
                mapa[i][e] = 'T';
                e++;
            }
            // Lado direito (an�logo ao esquerdo)
            if (ptod < LARG-MINT)
                dir = rand()%2;
            else
                dir = rand()%4;
            var = rand()%VARMAX;
            if (dir > 0)
                var *= -1;
            d = LARG-MINT;
            while (d >= ptod + var && d - ptoe >= MINW && mapa[i][d-MINW] == ' ') {
                mapa[i][d] = 'T';
                d--;
            }
            inverte = 1;
        } else {
            // Lado direito
            if (ptod < LARG-MINT)
                dir = rand()%2;
            else
                dir = rand()%4;
            var = rand()%VARMAX;
            if (dir > 0)
                var *= -1;
            d = LARG-MINT;
            while (d >= ptod + var && d - ptoe >= MINW && mapa[i][d-MINW] == ' ') {
                mapa[i][d] = 'T';
                d--;
            }
            // Lado esquerdo
            if (ptoe > MINT)
                dir = rand()%2;
            else
                dir = rand()%4;
            var = rand()%VARMAX;
            if (dir == 0)
                var *= -1;
            e = MINT;
            while (e <= ptoe + var && ptod - e >= MINW && mapa[i][e+MINW] == ' ') {
                mapa[i][e] = 'T';
                e++;
            }
            inverte = 0;
        }
        ptoe = e-1;
        ptod = d+1;
    }
}

/* Fun��o usada pela mapGen */
void buildEnd(char mapa[][LARG]) {
    int i, j, xpe, xpd, yp, bsize, ptoe, ptod, ptoef, ptodf, lub, lue, cont_;
    float var;
    // xpe - X da ponte � esquerda (xpd an�logo); yp - Y da ponte; bsize - Bridge Size; lub - Lines Until Bridge (a partir do in�cio da EndZ); lue - Lines Until End (a partir da Ponte)
    // var - varia��o necess�ria para fazer a terra alcan�ar a ponte; cont_ - contador auxiliar
    // ptoe - ponto esquerdo no inicio da EndZ, ptod - direito; ptoef - ponto esquerdo no fim do mapa, ptodf - direito;

    // Acha ptoe e ptod
    i = ENDZ;
    j = MINT;
    while (mapa[i][j] == 'T')
        j++;
    ptoe = j;
    j = LARG-MINT;
    while (mapa[i][j] == 'T')
        j--;
    ptod = j;
    // Acha ptoef e ptodf
    i = 0;
    j = MINT;
    while (mapa[i][j] == 'T')
        j++;
    ptoef = j;
    j = LARG-MINT;
    while (mapa[i][j] == 'T')
        j--;
    ptodf = j;
    // Gera a largura da ponte ( 7 a 10 )
    bsize = rand()%4+7;
    // Y do mapa onde ficar� a Ponte: de 3 a 6, nas linhas (4 a 7 nos �ndices)
    yp = rand()%4+3;
    // Define lub e lue
    lub = ENDZ - (yp+1);
    lue = yp;
    // Define xpe e xpd (centraliza a ponte com base em ptoe e potd); esse processo pode deixar a ponte at� 1 mais estreita que o originalmente definido
    xpe = ptoe;
    xpd = ptod;
    if (ptod - ptoe > bsize) {
        while (xpd - xpe > bsize) {
            xpe++; xpd--;
        }
    }
    else {
        while (xpd - xpe < bsize) {
            xpe--; xpd++;
        }
    }

    // Usa os dados para construir o terreno ao redor...
    // Esquerda e direita antes da ponte
    var = (xpe - ptoe)/lub;
    for (i = ENDZ, cont_ = 1; i > yp; i--, cont_++)
        for (j = MINT; j < ptoe + var*cont_; j++)
            mapa[i][j] = 'T';
    var = (xpd - ptod)/lub;
    for (i = ENDZ, cont_ = 1; i > yp; i--, cont_++)
        for (j = LARG-MINT; j > ptod + var*cont_; j--)
            mapa[i][j] = 'T';
    // Esquerda e direita depois da ponte
    var = (xpe - ptoef)/lue;
    for (i = 0; i < yp; i++)
        for (j = MINT; j < ptoef + var*(i+1); j++)
            mapa[i][j] = 'T';
    var = (xpd - ptodf)/lue;
    for (i = 0; i < yp; i++)
        for (j = LARG-MINT; j > ptodf + var*(i+1); j--)
            mapa[i][j] = 'T';
    // ...E ent�o a ponte em si
    for (i = yp; i <= yp+1; i++) {
        for (j = 0; j < xpe; j++)
            mapa[i][j] = 'T';
        for (; j <= xpd; j++)
            mapa[i][j] = 'P';
        for (; j < LARG; j++)
            mapa[i][j] = 'T';
    }
}

/* Fun��o usada pela mapGen */
void buildIslands(char mapa[][LARG]) {
    // i e j s�o usados para navegar a matriz, as outras s�o auxiliares
    int i, j, h, k, l, alt, chance;


    // Varre toda a matriz e bota '#' nas �reas proibidas (in�cio e fim)
    for (i = 0; i < ENDZ; i++)
        for (j = MINT; j < LARG-MINT; j++)
            if (mapa[i][j] == ' ')
                mapa[i][j] = '#';
    for (i = ISLZ; i < ALTMAX; i++)
        for (j = MINT; j < LARG-MINT; j++)
            if (mapa[i][j] == ' ')
                mapa[i][j] = '#';
    // E nos pontos a partir dos quais n�o tem espa�o lateral para uma linha de ilha
    for (i = ENDZ; i < ISLZ; i++) {
        // Testes pela esquerda
        j = MINT;
        k = 1;
        while (mapa[i][j] == 'T')
            j++;
        while (mapa[i][j] == ' ' && k <= MINW+ISLMAXW) {
            mapa[i][j] = '#';
            k++;
            j++;
        }
        // Testes pela direita
        j = LARG-MINT;
        k = 1;
        while (mapa[i][j] == 'T')
            j--;
        k = 1;
        while (mapa[i][j] == ' ' && k <= MINW+ISLMAXW) {
            mapa[i][j] = '#';
            k++;
            j--;
        }
    }


    // Com os testes laterais prontos, prossegue para a coloca��o das ilhas em si
    i = ISLZ - ISLMINH;
    j = MINT + MINW;
    // Enquanto n�o varrer todo mapa, gera uma nova altura de ilha
    while (i > ENDZ) {
        alt = rand()%(ISLMAXH-ISLMINH+1)+ISLMINH;
        // E entra no loop que dura at� colocar essa ilha ou at� terminar o mapa
        while (alt && i > ENDZ) {

            // Testa se o ponto atual � v�lido para essa ponte
            h = 0;
            while (mapa[i+h][j] == ' ' && h < alt+MINW)
                h++;
            // Se � v�lido e a chance de criar ilha responde verdadeiro,
            if (h == alt+MINW && randomTrue(ISLSEED)) {
                // Cria a ilha: gera a primeira linha, (primeira e �ltima n�o podem crescer tanto quanto as do meio)
                h = 0;
                mapa[i][j] = 'I';
                // (lado esquerdo)
                k = 1; chance = ISLSCH;
                while (randomTrue(chance)) {
                    mapa[i+h][j-k] = 'I';
                    // (a conta no if � para adequar o mapa �s defini��es no in�cio desse .h)
                    if (chance > ISLSCH - (ISLCHDEC*(ISLMAXW-2)))
                        chance -= ISLCHDEC;
                    else
                        chance = 0;
                    k++;
                }
                // (lado direito)
                k = 1; chance = ISLSCH;
                while (randomTrue(chance)) {
                    mapa[i+h][j+k] = 'I';
                    if (chance > ISLSCH - (ISLCHDEC*(ISLMAXW-2)))
                        chance -= ISLCHDEC;
                    else
                        chance = 0;
                    k++;
                }
                // ent�o, em cada altura,
                for (h++; h < alt; h++) {
                    // gera o centro,
                    mapa[i+h][j] = 'I';
                    // gera o lado esquerdo, cuidando para respeitar uma certa varia��o
                    k = 1;
                    chance = ISLSCH;
                    while ((randomTrue(chance) && mapa[i+h-1][j-k+2] == 'I') || mapa[i+h-1][j-k-2] == 'I') {
                        mapa[i+h][j-k] = 'I';
                        if (chance > ISLSCH - (ISLCHDEC*(ISLMAXW-1)))
                            chance -= ISLCHDEC;
                        else
                            chance = 0;
                        k++;
                    }
                    // e repete o processo para o lado direito
                    k = 1;
                    chance = ISLSCH;
                    while ((randomTrue(chance) && mapa[i+h-1][j+k-2] == 'I') || mapa[i+h-1][j+k+2] == 'I') {
                        mapa[i+h][j+k] = 'I';
                        if (chance > ISLSCH - (ISLCHDEC*(ISLMAXW-1)))
                            chance -= ISLCHDEC;
                        else
                            chance = 0;
                        k++;
                    }
                }
                // e encerra fazendo a �ltima linha
                mapa[i+h][j] = 'I';
                // (lado esquerdo)
                k = 1; chance = ISLSCH;
                while ((randomTrue(chance) && mapa[i+h-1][j-k+2] == 'I') || mapa[i+h-1][j-k-2] == 'I') {
                    mapa[i+h][j-k] = 'I';
                    if (chance > ISLSCH - (ISLCHDEC*(ISLMAXW-2)))
                        chance -= ISLCHDEC;
                    else
                        chance = 0;
                    k++;
                }
                // (lado direito)
                k = 1; chance = ISLSCH;
                while ((randomTrue(chance) && mapa[i+h-1][j+k-2] == 'I') || mapa[i+h-1][j+k+2] == 'I') {
                    mapa[i+h][j+k] = 'I';
                    if (chance > ISLSCH - (ISLCHDEC*(ISLMAXW-2)))
                        chance -= ISLCHDEC;
                    else
                        chance = 0;
                    k++;
                }
                // corrige os lados da ilha com '#', para outra ilha n�o ficar grudada ou muito pr�xima dessa; '#'s na primeira linha apenas j� s�o o bastante
                // Pela esquerda
                k = 1;
                while (mapa[i][j-k] == 'I')
                    k++;
                l = k;
                while (mapa[i][j-l] == ' ' && l-k < MINW+ISLMAXW) {
                    mapa[i][j-l] = '#';
                    l++;
                }
                // E direita
                k = 1;
                while (mapa[i][j+k] == 'I')
                    k++;
                l = k;
                while (mapa[i][j+l] == ' ' && l-k < MINW+ISLMAXW) {
                    mapa[i][j+l] = '#';
                    l++;
                }
                // Zera a altura para sinalizar que a ilha foi posta no lugar
                alt = 0;
                // E incrementa o j na quantidade necess�ria para poder criar outra ilha, economizando de testar as pr�ximas vezes que necessariamente falhariam
                j += MINW+ISLMAXW;
            }
            else // Sen�o (se o ponto atual n�o � v�lido), incrementa o j
                j++;
            // Se o j chegou no fim da linha, reseta-o e decrementa i
            if (j >= LARG-MINT-MINW) {
                j = MINT + MINW;
                i--;
            }
        }
    }


    // Finalmente, retira os '#'s da matriz
    for (i = 0; i < ALTMAX; i++)
        for (j = MINT; j < LARG-MINT; j++)
            if (mapa[i][j] == '#')
                mapa[i][j] = ' ';
}

/* Recebe uma matriz padr�o e gera um mapa novo e random nela, com bordas, ilhas, ponte, e dentro das normas de seguran�a */
int mapGen(char mapa[][LARG]) {
    limpaMapa(mapa, ALTMAX);
    buildBorders(mapa);
    buildEnd(mapa);
    buildIslands(mapa);
    return 1;
}

/* Recebe uma matriz padr�o e um int para dificuldade (de 0 a 4 inclusos) e gera inimigos e postos de gasolina na matriz, de acordo com aquela dificuldade.
   Essa matriz deve estar apenas com bordas e ilhas.                                                                                                        */
int enemyGen(char mapa[][LARG], int dificuldade, int alt) {
    float taxa, nav_t, heli_t, gas_t;
    int i, j, k, h;
    char enemy = 0;
    // Define as taxas, de acordo com a dificuldade; nav_t, heli_t e gas_t devem somar 100%
    switch (dificuldade) {
        // Pac�fico
        case 0: taxa = 0.1;
                nav_t = 0;
                heli_t = 0;
                gas_t = 100;
                break;
        // F�cil
        case 1: taxa = 0.3;
                nav_t = 45;
                heli_t = 25;
                gas_t = 30;
                break;
        // Normal
        case 2: taxa = 0.5;
                nav_t = 45;
                heli_t = 35;
                gas_t = 20;
                break;
        // Dif�cil
        case 3: taxa = 0.8;
                nav_t = 40;
                heli_t = 45;
                gas_t = 15;
                break;
        // Lun�tico da Ponte
        case 4: taxa = 1;
                nav_t = 35;
                heli_t = 55;
                gas_t = 10;
                break;
    }

    // Varre toda a matriz, botando '#' nas �reas proibidas (in�cio e fim)
    for (i = 0; i < ENDZ; i++)
        for (j = MINT; j < LARG-MINT; j++)
            if (mapa[i][j] == ' ')
                mapa[i][j] = '#';
    for (i = alt-ENTZ; i < alt; i++)
        for (j = MINT; j < LARG-MINT; j++)
            if (mapa[i][j] == ' ')
                mapa[i][j] = '#';
    // Varre fazendo os testes laterais e colocando '#'s
    for (i = ENDZ; i < alt-ENTZ; i++)
        for (j = MINT; j < LARG-MINT; j++)
            if (mapa[i][j] == ' ')
                if ((mapa[i][j+ENTSPC] != ' ' && mapa[i][j+ENTSPC] != '#') || (mapa[i][j-ENTSPC] != ' ' && mapa[i][j-ENTSPC] != '#'))
                    mapa[i][j] = '#';
    // Varre fazendo os testes verticais e colocando '@'s (usa um char diferente porque sen�o iria preencher todos ' 's com '#'s)
    for (i = ENDZ; i < alt-ENTZ; i++)
        for (j = MINT; j < LARG-MINT; j++)
            if (mapa[i][j] == ' ') {
                h = -ENTSPC-1;
                do {
                    h++;
                    if (mapa[i+h][j] != ' ' && mapa[i+h][j] != '@')
                        mapa[i][j] = '@';
                } while ((mapa[i+h][j] == ' ' || mapa[i+h][j] == '@') && h < ENTSPC);
            }

    // Percorre toda a matriz para por inimigos
    i = ENDZ;
    j = MINT+ENTSPC;
    while (i < alt-ENTZ) {
        // Se � uma �rea v�lida e d� verdadeiro para colocar um inimigo, ou se um inimigo j� foi escolhido mas n�o p�de ser colocado antes...
        if (mapa[i][j] == ' ' && (randomTrue(taxa) || enemy)) {
            if (!enemy)
                enemy = randomEnemy(nav_t, heli_t, gas_t);
            // ...tenta colocar o inimigo
            switch (enemy) {
                case 'N':   // Navio: se as 3 �reas � direita tamb�m s�o v�lidas, bota marcadores de �rea inv�lida ao redor e o navio no centro; sen�o, incrementa j e segue com os testes
                    if (mapa[i][j+1] == ' ' && mapa[i][j+2] == ' ' && mapa[i][j+3] == ' ') {
                        for (h = -ENTSPC; h <= ENTSPC; h++)
                            for (k = -ENTSPC; k <= ENTSPC+3; k++)
                                if (mapa[i+h][j+k] == ' ')
                                    mapa[i+h][j+k] = '#';
                        for (k = 0; k < 4; k++)
                            mapa[i][j+k] = 'N';
                        enemy = 0;
                        j += 4+ENTSPC;
                    }
                    else
                        j++;
                    break;
                case 'G':   // Gasolina: se as 3 �reas abaixo s�o v�lidas, bota as coisas; sen�o j++
                    if (mapa[i+1][j] == ' ' && mapa[i+2][j] == ' ' && mapa[i+3][j] == ' ') {
                        for (h = -ENTSPC; h <= ENTSPC+3; h++)
                            for (k = -ENTSPC; k <= ENTSPC; k++)
                                if (mapa[i+h][j+k] == ' ')
                                    mapa[i+h][j+k] = '#';
                        for (h = 0; h < 4; h++)
                            mapa[i+h][j] = 'G';
                        enemy = 0;
                        j += ENTSPC;
                    }
                    else
                        j++;
                    break;
                case 'X':   // Helic�ptero: bota as coisas
                    for (h = -ENTSPC; h <= ENTSPC; h++)
                        for (k = -ENTSPC; k <= ENTSPC; k++)
                            if (mapa[i+h][j+k] == ' ')
                                mapa[i+h][j+k] = '#';
                    mapa[i][j] = 'X';
                    enemy = 0;
                    j += ENTSPC;
                    break;
                default:
                    enemy = 0;
                    j++;
            }
        // Se a �rea n�o era v�lida ou o resto deu falso tamb�m, apenas incrementa o j e tenta denovo
        } else
            j++;
        // Sempre testa para ver se chegou no final da linha
        if (j > LARG-(MINT+ENTSPC)) {
            j = MINT+ENTSPC;
            i++;
        }
    }

    // Finalmente, retira os '#'s e '@'s da matriz
    for (i = 0; i < alt; i++)
        for (j = MINT; j < LARG-MINT; j++)
            if (mapa[i][j] == '#' || mapa[i][j] == '@')
                mapa[i][j] = ' ';
    return 1;
}

/* Recebe uma matriz padr�o, o ponto esquerdo do fim da fase, o ponto direito do fim da fase e o ponto inicial onde a matriz deve ser processada
   Gera a extens�o da fase */
void buildExtension(char mapa[][LARG], int *eini, int *dini, int ini) {
	// e - esquerda, ptoe - ponto anterior esquerda; d igual; dir - dire��o da varia��o da borda; var - varia��o da borda;
	int i, e, d, ptoe, ptod, dir, var;
	// eini - ponto esquerdo inicial; dini - ponto direito inicial
	// ini - quantas linhas n�o precisam ser geradas, por j�
	ptoe = *eini-1;
	ptod = *dini+1;

	limpaMapa(mapa, ALT);

    // Gera as novas bordas
    for(i = ALT-1; i > ini; i--) {
        // Lado esquerdo
        // Escolha do tamanho da varia��o
        var = rand()%VARMAX;
        // Escolha da dire��o; se a �ltima linha passou de 1/3 da tela, volta; sen�o, escolhe ida ou volta, com mais chance para a volta
        if (ptoe > LARG/3)
            dir = 1;
        else
            dir = rand()%3;
        if (dir > 0)
            var *= -1;
        // 'e' deve obedecer v�rias condi��es para o pr�ximo 'T' ser colocado: respeitar a varia��o e deixar espa�o o bastante � direita tanto na linha corrente quando na anterior
        e = MINT;
        while (e <= ptoe + var && ptod - e >= MINW && mapa[i][e+MINW] == ' ') {
            mapa[i][e] = 'T';
            e++;
        }
        // Lado direito (an�logo ao esquerdo)
        var = rand()%VARMAX;
        if (ptod < 2*LARG/3)
            dir = 1;
        else
            dir = rand()%3;
        if (dir == 0)
            var *= -1;
        d = LARG-MINT;
        while (d >= ptod + var && d - ptoe >= MINW && mapa[i][d-MINW] == ' ') {
            mapa[i][d] = 'T';
            d--;
        }
        ptoe = e-1;
        ptod = d+1;
    }

    *eini = e;
    *dini = d;
}

/* Recebe uma matriz padr�o, o ponto esquerdo do fim da fase antiga, o ponto direito do fim da fase antiga, o ponto esquerdo do come�o da nova fase e o ponto direito do come�o da nova fase
   Gera a conex�o entre as fases */
void buildConnection(char mapa[][LARG], int ptoe, int ptod, int ptoef, int ptodf) {
	int i, j;
	float var;
    // var - varia��o necess�ria para fazer a terra alcan�ar as posi��es da nova fase;
    // ptoe - ponto esquerdo no fim da �ltima fase, ptod - direito; ptoef - ponto esquerdo no inicio da nova fase, ptodf - direito;

    limpaMapa(mapa, CONALT);

    // Constroi terreno do ponto final ao inicial
    var = (float)(ptoe - ptoef)/CONALT;
    for (i = 0; i < CONALT; i++)
        for (j = MINT; j < ptoef + var*(i+1); j++)
            mapa[i][j] = 'T';
    var = (float)(ptod - ptodf)/CONALT;
    for (i = 0; i < CONALT; i++)
        for (j = LARG-MINT; j > ptodf + var*(i+1); j--)
            mapa[i][j] = 'T';
}

#include "MapGen.h"

#include <stdlib.h>

/* Recebe um número de 0 a 100 com até uma casa decimal e retorna verdadeiro com essa porcentagem de chance */
int randomTrue(float percent_true) {
    int r;
    r = rand()%1000;
    if (r < percent_true*10)
        return 1;
    else
        return 0;
}

/* Recebe as taxas de geração de cada entidade e retorna 'N', 'X' ou 'G' com base nelas. As taxas não devem, somadas, passar de 100 */
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

/* Recebe a altura e uma matriz padrão e formata ela, removendo o lixo */
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

/* Função usada pela mapGen */
void buildBorders(char mapa[][LARG]) {
    // e - esquerda, ptoe - ponto anterior esquerda; d igual; dir - direção da variação da borda; var - variação da borda;
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
        // Cada passagem de linha ocorre no sentido contrário da anterior, para não ter "bias" (a versão sem inverte estava criando mapas com muita terra na esquerda)
        if (!inverte) {
            // Lado esquerdo
            // Escolha da direção; se a última linha não cresceu mais que a borda, aumenta a chance disso acontecer
            if (ptoe > MINT)
                dir = rand()%2;
            else
                dir = rand()%4;
            // Escolha do tamanho da variação
            var = rand()%VARMAX;
            if (dir == 0)
                var *= -1;
            // 'e' deve obedecer várias condições para o próximo 'T' ser colocado: respeitar a variação e deixar espaço o bastante à direita tanto na linha corrente quando na anterior
            e = MINT;
            while (e <= ptoe + var && ptod - e >= MINW && mapa[i][e+MINW] == ' ') {
                mapa[i][e] = 'T';
                e++;
            }
            // Lado direito (análogo ao esquerdo)
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

/* Função usada pela mapGen */
void buildEnd(char mapa[][LARG]) {
    int i, j, xpe, xpd, yp, bsize, ptoe, ptod, ptoef, ptodf, lub, lue, cont_;
    float var;
    // xpe - X da ponte à esquerda (xpd análogo); yp - Y da ponte; bsize - Bridge Size; lub - Lines Until Bridge (a partir do início da EndZ); lue - Lines Until End (a partir da Ponte)
    // var - variação necessária para fazer a terra alcançar a ponte; cont_ - contador auxiliar
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
    // Y do mapa onde ficará a Ponte: de 3 a 6, nas linhas (4 a 7 nos índices)
    yp = rand()%4+3;
    // Define lub e lue
    lub = ENDZ - (yp+1);
    lue = yp;
    // Define xpe e xpd (centraliza a ponte com base em ptoe e potd); esse processo pode deixar a ponte até 1 mais estreita que o originalmente definido
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
    // ...E então a ponte em si
    for (i = yp; i <= yp+1; i++) {
        for (j = 0; j < xpe; j++)
            mapa[i][j] = 'T';
        for (; j <= xpd; j++)
            mapa[i][j] = 'P';
        for (; j < LARG; j++)
            mapa[i][j] = 'T';
    }
}

/* Função usada pela mapGen */
void buildIslands(char mapa[][LARG]) {
    // i e j são usados para navegar a matriz, as outras são auxiliares
    int i, j, h, k, l, alt, chance;


    // Varre toda a matriz e bota '#' nas áreas proibidas (início e fim)
    for (i = 0; i < ENDZ; i++)
        for (j = MINT; j < LARG-MINT; j++)
            if (mapa[i][j] == ' ')
                mapa[i][j] = '#';
    for (i = ISLZ; i < ALTMAX; i++)
        for (j = MINT; j < LARG-MINT; j++)
            if (mapa[i][j] == ' ')
                mapa[i][j] = '#';
    // E nos pontos a partir dos quais não tem espaço lateral para uma linha de ilha
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


    // Com os testes laterais prontos, prossegue para a colocação das ilhas em si
    i = ISLZ - ISLMINH;
    j = MINT + MINW;
    // Enquanto não varrer todo mapa, gera uma nova altura de ilha
    while (i > ENDZ) {
        alt = rand()%(ISLMAXH-ISLMINH+1)+ISLMINH;
        // E entra no loop que dura até colocar essa ilha ou até terminar o mapa
        while (alt && i > ENDZ) {

            // Testa se o ponto atual é válido para essa ponte
            h = 0;
            while (mapa[i+h][j] == ' ' && h < alt+MINW)
                h++;
            // Se é válido e a chance de criar ilha responde verdadeiro,
            if (h == alt+MINW && randomTrue(ISLSEED)) {
                // Cria a ilha: gera a primeira linha, (primeira e última não podem crescer tanto quanto as do meio)
                h = 0;
                mapa[i][j] = 'I';
                // (lado esquerdo)
                k = 1; chance = ISLSCH;
                while (randomTrue(chance)) {
                    mapa[i+h][j-k] = 'I';
                    // (a conta no if é para adequar o mapa às definições no início desse .h)
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
                // então, em cada altura,
                for (h++; h < alt; h++) {
                    // gera o centro,
                    mapa[i+h][j] = 'I';
                    // gera o lado esquerdo, cuidando para respeitar uma certa variação
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
                // e encerra fazendo a última linha
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
                // corrige os lados da ilha com '#', para outra ilha não ficar grudada ou muito próxima dessa; '#'s na primeira linha apenas já são o bastante
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
                // E incrementa o j na quantidade necessária para poder criar outra ilha, economizando de testar as próximas vezes que necessariamente falhariam
                j += MINW+ISLMAXW;
            }
            else // Senão (se o ponto atual não é válido), incrementa o j
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

/* Recebe uma matriz padrão e gera um mapa novo e random nela, com bordas, ilhas, ponte, e dentro das normas de segurança */
int mapGen(char mapa[][LARG]) {
    limpaMapa(mapa, ALTMAX);
    buildBorders(mapa);
    buildEnd(mapa);
    buildIslands(mapa);
    return 1;
}

/* Recebe uma matriz padrão e um int para dificuldade (de 0 a 4 inclusos) e gera inimigos e postos de gasolina na matriz, de acordo com aquela dificuldade.
   Essa matriz deve estar apenas com bordas e ilhas.                                                                                                        */
int enemyGen(char mapa[][LARG], int dificuldade, int alt) {
    float taxa, nav_t, heli_t, gas_t;
    int i, j, k, h;
    char enemy = 0;
    // Define as taxas, de acordo com a dificuldade; nav_t, heli_t e gas_t devem somar 100%
    switch (dificuldade) {
        // Pacífico
        case 0: taxa = 0.1;
                nav_t = 0;
                heli_t = 0;
                gas_t = 100;
                break;
        // Fácil
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
        // Difícil
        case 3: taxa = 0.8;
                nav_t = 40;
                heli_t = 45;
                gas_t = 15;
                break;
        // Lunático da Ponte
        case 4: taxa = 1;
                nav_t = 35;
                heli_t = 55;
                gas_t = 10;
                break;
    }

    // Varre toda a matriz, botando '#' nas áreas proibidas (início e fim)
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
    // Varre fazendo os testes verticais e colocando '@'s (usa um char diferente porque senão iria preencher todos ' 's com '#'s)
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
        // Se é uma área válida e dá verdadeiro para colocar um inimigo, ou se um inimigo já foi escolhido mas não pôde ser colocado antes...
        if (mapa[i][j] == ' ' && (randomTrue(taxa) || enemy)) {
            if (!enemy)
                enemy = randomEnemy(nav_t, heli_t, gas_t);
            // ...tenta colocar o inimigo
            switch (enemy) {
                case 'N':   // Navio: se as 3 áreas à direita também são válidas, bota marcadores de área inválida ao redor e o navio no centro; senão, incrementa j e segue com os testes
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
                case 'G':   // Gasolina: se as 3 áreas abaixo são válidas, bota as coisas; senão j++
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
                case 'X':   // Helicóptero: bota as coisas
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
        // Se a área não era válida ou o resto deu falso também, apenas incrementa o j e tenta denovo
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

/* Recebe uma matriz padrão, o ponto esquerdo do fim da fase, o ponto direito do fim da fase e o ponto inicial onde a matriz deve ser processada
   Gera a extensão da fase */
void buildExtension(char mapa[][LARG], int *eini, int *dini, int ini) {
	// e - esquerda, ptoe - ponto anterior esquerda; d igual; dir - direção da variação da borda; var - variação da borda;
	int i, e, d, ptoe, ptod, dir, var;
	// eini - ponto esquerdo inicial; dini - ponto direito inicial
	// ini - quantas linhas não precisam ser geradas, por já
	ptoe = *eini-1;
	ptod = *dini+1;

	limpaMapa(mapa, ALT);

    // Gera as novas bordas
    for(i = ALT-1; i > ini; i--) {
        // Lado esquerdo
        // Escolha do tamanho da variação
        var = rand()%VARMAX;
        // Escolha da direção; se a última linha passou de 1/3 da tela, volta; senão, escolhe ida ou volta, com mais chance para a volta
        if (ptoe > LARG/3)
            dir = 1;
        else
            dir = rand()%3;
        if (dir > 0)
            var *= -1;
        // 'e' deve obedecer várias condições para o próximo 'T' ser colocado: respeitar a variação e deixar espaço o bastante à direita tanto na linha corrente quando na anterior
        e = MINT;
        while (e <= ptoe + var && ptod - e >= MINW && mapa[i][e+MINW] == ' ') {
            mapa[i][e] = 'T';
            e++;
        }
        // Lado direito (análogo ao esquerdo)
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

/* Recebe uma matriz padrão, o ponto esquerdo do fim da fase antiga, o ponto direito do fim da fase antiga, o ponto esquerdo do começo da nova fase e o ponto direito do começo da nova fase
   Gera a conexão entre as fases */
void buildConnection(char mapa[][LARG], int ptoe, int ptod, int ptoef, int ptodf) {
	int i, j;
	float var;
    // var - variação necessária para fazer a terra alcançar as posições da nova fase;
    // ptoe - ponto esquerdo no fim da última fase, ptod - direito; ptoef - ponto esquerdo no inicio da nova fase, ptodf - direito;

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

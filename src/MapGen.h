#ifndef MAPGEN_H
#define MAPGEN_H

#define ALTMAX 1000
#define ALT 25
#define LARG 80
#define MINT 5  // MINimum border Terrain
#define MINW 10 // MINimun water inbetween
#define ENDZ 10  //END Zone
#define ISLZ ALTMAX-15 // ISLands Zone
#define ENTZ 6 // ENTity Zone (essa quantidade de linhas acima do início da fase/fim da matriz)
#define VARMAX 4 // VARiação MAXima da borda (é esse número menos 1)
#define SEEDSIZE 90 // 0 <- mais aberta |primeira linha (seed)| mais fechada -> 100
#define ISLSEED 0.3 // ISLand SEED (taxa de semeação de ilhas)
#define ISLSCH 100 // ISLand Starting Chance (chance de ir pros lados)
#define ISLCHDEC 15 // ISLand CHance DECay
#define ISLMINH 4 // ISLand MINimun Height
#define ISLMAXH 10 // ISLand MAXimun Height
#define ISLMAXW 4 // ISLand MAXimun Widht (pra cada lado)
#define ENTSPC 2 // ENTity SPaCement (espaço ao redor de inimigos/postos de gasolina (antes de se mexerem, ao menos))(deve ser <= 3)
#define CONALT 5 // Altura da conexão

int randomTrue(float percent_true);
char randomEnemy(int navio, int heli, int gas);
void limpaMapa(char mapa[][LARG], int alt);
void buildBorders(char mapa[][LARG]);
void buildEnd(char mapa[][LARG]);
void buildIslands(char mapa[][LARG]);
int mapGen(char mapa[][LARG]);
int enemyGen(char mapa[][LARG], int dificuldade, int alt);
void buildExtension(char mapa[][LARG], int *eini, int *dini, int ini);
void buildConnection(char mapa[][LARG], int ptoe, int ptod, int ptoef, int ptodf);

#endif // MAPGEN_H

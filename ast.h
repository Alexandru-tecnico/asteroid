#include <stdio.h>

#define LARGURA_JANELA 0
#define ALTURA_JANELA 1

#define NUM_MAX_NIVEIS 20
/*Parametros que descrevem um nivel*/
#define VEL 0
#define T_GERAR 1
#define TAMANHO_MIN 2
#define TAMANHO_MAX 3
#define DISPAROS 4
#define MUDAR_NIVEL 5
/*Numero de parametros que descrevem um nivel*/
#define NUM_PARAMETROS_NIVEL 6

/*Protótipo da função que lê a informação do ficheiro das configurações (config.c)*/
void ler_config(int[], int[][NUM_PARAMETROS_NIVEL], int*);

/*Função responsável pelo jogo em si. A janela é aberta aqui. (jogo.c)*/
void jogar(int[], int[][NUM_PARAMETROS_NIVEL], int);

/*Funções que lidam com o guardar dos resultados (res.c)*/
void ler_nome(char[], FILE*);
int guardar_resultado(unsigned int);

#include "ast.h"
#include <g2.h>      /* Para trabalhar com as funções gráficas g2 */
#include <g2_X11.h>  /* Para abrir o dispositivo físico */
#include <unistd.h> /*Biblioteca com a função usleep*/
#include <stdlib.h>
#include <time.h>
#include <string.h>

/*Rato*/
#define X_RATO 0
#define Y_RATO 1
#define PROP_RATO 2

/*Asteroides*/
    /*Propriedades double*/
/*Posição do centro do asteroide*/
#define X_AST 0
#define Y_AST 1
#define R_AST 2 /*Raio do asteroide (sem ofensa para o asteroide)*/
#define D_PROP_AST 3
    /*Propriedades int*/
#define VIDAS_AST 0
#define I_PROP_AST 1
#define NUM_MAX_AST 1000

#define COR_NUM 1 /*cor da vida (preto)*/


/*Tamanho da font das letras no canto superior esquerdo*/
#define FONT_INF 20

/*Nave*/
#define X_NAVE 0
#define Y_NAVE 1 /*Posições do canto superior esquerdo*/
#define LARGURA_NAVE 2
#define ALTURA_NAVE 3
#define PROP_NAVE 4 
/*Velocidade da nave em pixeis por segundo*/
#define VEL_NAVE 500.0
/*Taxa de atualização do jogo em s*/
#define TAXA_ATUAL 0.1
/*Define a função PAUSAR que pausa X segundos*/
#define PAUSAR(X) usleep((int)(X*1e6))
/*Tamanho da font dos numeros nos asteroides*/
#define AST_FONT 20

int clique(unsigned int);
void abrir_janela(int*,int*, int[]);
void mover_nave(double nv[], double x_rato, double vel, int dim[]);
int mover_asteroides(double[][D_PROP_AST], unsigned int, int);
void remover_asteroide(double d_ast[][D_PROP_AST], int i_ast[][I_PROP_AST], unsigned int*n_ast, int pos);
unsigned int disparar(double nave[], double d_ast[][D_PROP_AST], int i_ast[][I_PROP_AST], unsigned int*n_ast);
void desenhar_nave(int,int, double[]);
void desenhar_disparo(int,int,double[], int[]);
void init_nave(double nv[], const int dim[]);
void desenhar_n_abatidos(const int dev,const int d, const int dim[], const unsigned int n_abatidos);
void desenhar_nivel(const int dev,const int d, const int dm[], const unsigned int nivel);
void desenhar_asteroides(int,int,double[][D_PROP_AST],int[][I_PROP_AST],unsigned int);
void gerar_asteroide(int[], int[], double[][D_PROP_AST],int[][I_PROP_AST],unsigned int*);
int i_aleatorio(int, int);
int calcular_turno_geracao(int);
void mensagem_prox_nivel(const int,const int ,int,int[]);
void mensagem_fim_jogo(const int,const int,int ,int[]);

/*Função que trata da lógica e dos gráficos do jogo*/
void jogar(int dim[], int niv[][NUM_PARAMETROS_NIVEL], int n_niveis)
{
	int gameover = 0, /*0 se o jogo não acabou, 1 se acabou*/
   	 	disparou = 0; /*1 se a nave disparou, 0 se não*/
	unsigned int estado, /*Número que representa as teclas que estão pressionadas*/
		num_ast = 0, /*Número de asteroides em existencia*/
		n_ast_abatidos=0, /*Número de asteroides abatido no nível atual*/
		n_ast_abatidos_total=0, /*Número total de asteroides abatidos (resultado)*/
		nivel_atual=0,
		i=0, /*Contador de turnos*/
		turno_geracao_ast; /*Turno em que o asteroide deve ser gerado*/
	
	int d, dev, i_ast[NUM_MAX_AST][I_PROP_AST];
	double rato[PROP_RATO], nave[PROP_NAVE], d_ast[NUM_MAX_AST][D_PROP_AST];
	abrir_janela(&d, &dev, dim); 
	init_nave(nave, dim); /*Inicializa a nave*/
	turno_geracao_ast = calcular_turno_geracao(niv[nivel_atual][T_GERAR]); /*Calcula o turno de geração do primeiro asteroide*/
	srand(time(NULL)); /*Inicializa a seed de geração dos números aleatórios com o tempo de inicio do jogo*/
	
	while(!gameover)
	{	
   		if (i >= turno_geracao_ast) /*Já chegou o momento de gerar um novo asteroide?*/
		{
			i = 0;
			gerar_asteroide(dim, niv[nivel_atual], d_ast, i_ast, &num_ast);
			turno_geracao_ast = calcular_turno_geracao(niv[nivel_atual][T_GERAR]);
		}
		/*Obtem a posição do rato e o estado dos butões*/
		g2_query_pointer(dev, &rato[X_RATO], &rato[Y_RATO], &estado);
		mover_nave(nave, rato[X_RATO], VEL_NAVE, dim);
		gameover = mover_asteroides(d_ast, num_ast, niv[nivel_atual][VEL]);
		g2_clear(d); /*Limpa os desenhos anteriores*/
    	if (clique(estado) && !disparou) /*Se o rato se encontra clicado e ainda não disparou*/
    	{
			n_ast_abatidos += disparar(nave, d_ast, i_ast, &num_ast);
    		desenhar_disparo(dev,d, nave, dim);
    		disparou = 1;
    	}
    	else if (!clique(estado))
			disparou = 0;	
    	desenhar_n_abatidos(dev,d, dim, n_ast_abatidos);
    	desenhar_nivel(dev,d,dim, nivel_atual);
    	desenhar_nave(d, dev, nave);
    	desenhar_asteroides(d, dev, d_ast, i_ast, num_ast);
		g2_flush(d);
    	if (n_ast_abatidos >= niv[nivel_atual][MUDAR_NIVEL])
    	{
    		nivel_atual++;
    		n_ast_abatidos_total += n_ast_abatidos;
    		n_ast_abatidos = 0;
    		
			if (nivel_atual >= n_niveis)
			{
			 	gameover = 1;
			}
			else
			{
				mensagem_prox_nivel(dev,d,nivel_atual,dim);
				estado = 0;
				/*Dá um momento para o jogador se aperceber que o jogo continua*/
				PAUSAR(1);
				/*Espera que o jogador clique para prosseguir o jogo para o próximo nivel*/
				do
				{	
					g2_query_pointer(dev, &rato[X_RATO], &rato[Y_RATO], &estado);
				}while(!clique(estado));
			}					
		}
		++i;
		PAUSAR(TAXA_ATUAL);
	}
	/*Adicionar ao total quaisquer asteroides abatidos durante o último nivel*/
	n_ast_abatidos_total += n_ast_abatidos;
	estado = 0; /*Para ter a certeza que o clique teve a intenção de sair*/
	/*Imprimir mensagem de fim de jogo e esperar pelo clique do rato para fechar a janela*/	
	mensagem_fim_jogo(dev,d,n_ast_abatidos_total ,dim);/*Imprime mensagem de fim de jogo*/
	PAUSAR(1);
    while(!clique(estado))
	{		
		g2_query_pointer(dev, &rato[X_RATO], &rato[Y_RATO], &estado);
	}
	if (clique(estado))
 		g2_close(d);
	guardar_resultado(n_ast_abatidos_total);
}

/*Desenha a informação do número de aseroides abatidos no nível atual*/
void desenhar_n_abatidos(const int dev,const int d, const int dim[], const unsigned int n_abatidos)
{
	char s[100]="";
	sprintf(s,"Asteroides abatidos neste nivel: %u", n_abatidos);
	g2_set_font_size(d, FONT_INF);
	g2_pen(d, g2_ink(dev, 1.0, 0.0, 0.0));
	g2_string(d, 5, dim[ALTURA_JANELA]-FONT_INF, s);
}

/*Desenha a informação do nivel na janela*/
void desenhar_nivel(const int dev,const int d, const int dm[], const unsigned int nivel)
{
	char s[100] = "";
	sprintf(s,"Nivel %d", nivel+1);
	g2_set_font_size(d, FONT_INF);
	g2_pen(d, g2_ink(dev, 1.0, 0.0, 0.0));/*Cor vermelha*/
	g2_string(d, 5, dm[ALTURA_JANELA]-(2*FONT_INF), s);
}

int calcular_turno_geracao(int t_gerar)
{
	return ((int) i_aleatorio(1.0, t_gerar*1000) / TAXA_ATUAL / 1000.0);
}

/*Gera um número aleatório entre a e b, [a,b]*/
int i_aleatorio(int a, int b)
{	
	if (a == b)
		return a;
	return (rand() % (b-a) + a);
}

/*Gera um asteroide e adiciona-o aos arrays dos asteroides*/
void gerar_asteroide(int dim_janela[],
			int prop_niv[],
			double d_asteroides[][D_PROP_AST], 
			int i_asteroides[][I_PROP_AST],
			unsigned int*n_ast
                    )
{
	int n = *n_ast;
	d_asteroides[n][R_AST] = (double)i_aleatorio(prop_niv[TAMANHO_MIN], prop_niv[TAMANHO_MAX]);
	d_asteroides[n][X_AST] = (double)i_aleatorio(d_asteroides[n][R_AST],
								(int)(dim_janela[LARGURA_JANELA] - d_asteroides[n][R_AST]));
	d_asteroides[n][Y_AST] = dim_janela[ALTURA_JANELA]+d_asteroides[n][R_AST];
	i_asteroides[n][VIDAS_AST] = i_aleatorio(1, prop_niv[DISPAROS]);
	(*n_ast)++;
}

/*Retorna 0 se nenhum asteroide atingiu o fundo da janela, 1 se pelo menos um atingiu*/
int mover_asteroides(double d_ast[][D_PROP_AST], unsigned int n_ast, int vel)
{
	register unsigned int i;
	int fundo; 
	double passo = (double)(vel * TAXA_ATUAL);
	for(i=0, fundo=0; i<n_ast; ++i)
	{
		if (((d_ast[i][Y_AST] -= passo)-d_ast[i][R_AST]) <= 0.0)
			fundo = 1;
	}
	return fundo;
}

/*Desenha os asteroides no ecrã*/
void desenhar_asteroides(int d,
                         int dev,
                         double d_asteroides[][D_PROP_AST], 
                         int i_asteroides[][I_PROP_AST],
                         unsigned int num_asteroides
                        )
{
    register int i;
    char vidas[5];
     /*Representa números até 9999*/
    
    g2_set_font_size(d, AST_FONT);
    for(i=0;i<num_asteroides;++i)
    {
    	
    	
    	
    	snprintf(vidas, 5, "%2d", i_asteroides[i][VIDAS_AST]);
        
        g2_pen(d, g2_ink(dev, 0.7, 0.7,0.3));/*cor acastanhada*/
        g2_filled_circle(d, d_asteroides[i][X_AST], d_asteroides[i][Y_AST], d_asteroides[i][R_AST]);
        g2_pen(d, COR_NUM);/*preto*/
        g2_circle(d, d_asteroides[i][X_AST], d_asteroides[i][Y_AST], d_asteroides[i][R_AST]);/*Contorno do asteroide*/
        
        g2_pen(d, COR_NUM);/*preto*/
        g2_string(d, (d_asteroides[i][X_AST]-AST_FONT/2),
		     (d_asteroides[i][Y_AST]-AST_FONT/2),
		     vidas
		 );
    }
}

/*Remove o asteroide que se encontra na posição pos dos arrays dos asteroides*/
void remover_asteroide(double d_ast[][D_PROP_AST], int i_ast[][I_PROP_AST], unsigned int*n_ast, int pos)
{
	register int i, j;
	--(*n_ast);
	for(i=pos; i<(*n_ast); ++i)
	{
		for (j=0; j < D_PROP_AST;++j)
		{
			d_ast[i][j] = d_ast[i+1][j];
		}
		for (j=0; j < I_PROP_AST;++j)
		{
			i_ast[i][j] = i_ast[i+1][j];
		}
	}
}

/*
Determina os asteroides que são atingidos pelo disparo e atualiza a sua informação
Retorna o número de asteroides abatidos a cada invocação da função
*/
unsigned int disparar(double nave[], double d_ast[][D_PROP_AST], int i_ast[][I_PROP_AST], unsigned int*n_ast)
{
	register int i;
	unsigned int abatidos=0;
	double x_disparo = nave[X_NAVE]+nave[LARGURA_NAVE] /2;
	double esq_ast, dir_ast;
	for(i=0; i < (*n_ast); ++i)
	{
		esq_ast = d_ast[i][X_AST] - d_ast[i][R_AST];
		dir_ast = d_ast[i][X_AST] + d_ast[i][R_AST];
		if ((x_disparo >= esq_ast) && (x_disparo <= dir_ast))
		{	
			abatidos++;
			--i_ast[i][VIDAS_AST];
			if (i_ast[i][VIDAS_AST] <= 0)
			{
				remover_asteroide(d_ast, i_ast, n_ast, i);
				--i; /*O asteroide seguinte passou para a posição atual*/
			}
		}
	}
	return abatidos;
}

/*Desenha o disparo na janela (linha vertical)*/
void desenhar_disparo(int dev,int d, double nave[], int dim[])
{
    double x_disparo = nave[X_NAVE]+nave[LARGURA_NAVE] /2;
   	g2_pen(d, g2_ink(dev, 1.0, 0.0, 0.0));/* Disparo vermelho*/
    g2_line(dev, x_disparo,
                 nave[Y_NAVE],
                 x_disparo,
                 dim[ALTURA_JANELA]
           );
}

/*Inicializa os parâmetros da nave*/
void init_nave(double nv[],const int dim[])
{    
    nv[LARGURA_NAVE] = 50.0;
    nv[ALTURA_NAVE] = 25.0;
    nv[X_NAVE] = (dim[LARGURA_JANELA] - nv[LARGURA_NAVE]) / 2;
    nv[Y_NAVE] = nv[ALTURA_NAVE];
}

/*Desenha a nave na janela*/
void desenhar_nave(int d, int dev, double nv[])
{	
	
		g2_pen(d, g2_ink(dev, 1.0, 0.0, 0.0));
    	g2_filled_rectangle(d, nv[X_NAVE], 
			nv[Y_NAVE],
			nv[X_NAVE] + nv[LARGURA_NAVE],
			nv[Y_NAVE] - nv[ALTURA_NAVE]
			);
}

/*Devolve 0x100 se o rato foi clicado, 0 caso contrário*/
int clique(unsigned int btn)
{
	/*O código do clique no rato é 0x100*/
	return (btn & 0x100);
}

void mover_nave(double nv[], double x_rato, double vel, int dim[])
{
    double x_centro = nv[X_NAVE] + nv[LARGURA_NAVE] / 2,
           dist_hor = x_rato - x_centro;
    const double step = vel * TAXA_ATUAL; /*Deslocamento da nave neste turno*/
    
    if (dist_hor > step)
        nv[X_NAVE] += step; /*Move a nave para a direita*/
    else if (dist_hor < -(step))
        nv[X_NAVE] -= step; /*Move a nave para a esquerda*/
    else /*A nave está a uma distancia inferior do rato que step (impede oscilações)*/
        nv[X_NAVE] = x_rato - nv[LARGURA_NAVE] / 2; /*O centro da nave fica com a posição do rato*/
    x_centro = nv[X_NAVE] + nv[LARGURA_NAVE] / 2; /*Atualiza o centro da nave*/
    if ( x_centro < 0.0) /*Saiu da janela pela esquerda*/
        nv[X_NAVE] = -(nv[LARGURA_NAVE] / 2);
    else if ( x_centro > dim[LARGURA_JANELA]) /*Saiu da janela pela direita*/
        nv[X_NAVE] = dim[LARGURA_JANELA] - nv[LARGURA_NAVE] / 2;
}

void abrir_janela(int*dv /*dispositivo virtual*/, int* df /*dispositivo fisico*/, int dim[])
{
    *df = g2_open_X11(dim[LARGURA_JANELA], dim[ALTURA_JANELA]);
    *dv = g2_open_vd();
    g2_clear(*dv);
    g2_attach(*dv, *df);
    g2_set_background(*dv,g2_ink(*df, 0.4, 0.1, 1.0));/* Define a cor do fundo como azul*/
}
void mensagem_prox_nivel(const int dev,const int d,int nivel_atual,int dim[])
{

				char s1[50]="";
				char p1[100]="";
				
				g2_pen(d, g2_ink(dev, 1.0, 0.0, 0.0));/*Imprime a 100% vermelho*/
				
				sprintf(s1,"Bem vindo ao nivel %d!",nivel_atual+1);
				g2_set_font_size(d, 30);
				g2_string(d, (dim[LARGURA_JANELA]/2)-125, dim[ALTURA_JANELA]/2, s1);
				
				sprintf(p1,"Clique para continuar");
				g2_set_font_size(d, 25);
				g2_string(d, (dim[LARGURA_JANELA]/2)-110, (dim[ALTURA_JANELA]/2)-30, p1);

}

void mensagem_fim_jogo(const int dev,const int d,int n_ast_abatidos_total,int dim[])
{
		char p[100]="";
		char f1[100]=""; 
		char f2[100]=""; 
		g2_pen(d, g2_ink(dev, 1.0, 0.0, 0.0));/*Imprime a 100% vermelho*/
		sprintf(f1,"Fim do Jogo!");
		g2_set_font_size(d, 25);
		g2_string(d, (dim[LARGURA_JANELA]/2)-85, dim[ALTURA_JANELA]/2+30, f1);
		sprintf(f2,"Pontos:%d",n_ast_abatidos_total);
		g2_set_font_size(d, 25);
		g2_string(d, (dim[LARGURA_JANELA]/2)-70, (dim[ALTURA_JANELA]/2), f2);
		
		sprintf(p,"Clique para continuar");
		g2_set_font_size(d, 25);
		g2_string(d, (dim[LARGURA_JANELA]/2)-120, (dim[ALTURA_JANELA]/2)-30, p);
	

}

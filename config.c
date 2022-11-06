/*
					Ficheiro com as Funções responsáveis por ler as configurações do jogo
*/

/*Biblioteca para a utilização da função exit(int)*/
#include <stdlib.h>
/*Biblioteca com a função isspace, que determina se um caratér é espaço em branco*/
#include <ctype.h>
/*Ficheiro com as definições e protótipos de funções utilizadas em vários ficheiros do jogo*/
#include "ast.h"

    /*Números de retorno do programa devido a erros*/
/*Erro na abertura do ficheiro das configurações*/
#define ERRO_CONFIG 1
/*Ficheiro não tem o formato necessário à sua correta leitura*/
#define FICHEIRO_MAL_FORMATADO 2
/*Número de níveis superior ao número máximo permitido*/
#define NIVEL_OVERFLOW 3
/*Número de níveis inferior ou igual a 0*/
#define NIVEL_UNDERFLOW 4

/*Número máximo de carateres que uma linha do ficheiro pode conter*/
#define MAX_LINHA 499
/*Nome do ficheiro com as configurações do jogo*/
#define FICHEIRO_CONFIG "config.txt"

/*Protótipos das funções que são apenas utilizadas neste ficheiro*/
void skip_comentarios(char*, FILE*);

/*Função responsável pela leitura das configurações do jogo presentes no ficheiro config.txt*/
void ler_config(int dim[], /*Vetor para o qual serão lidas as dimensões da janela*/
				int niveis[][NUM_PARAMETROS_NIVEL], /*Matriz com a informação de cada nível*/
				int*num_niveis /*Aponta para a variável que terá o número de níveis como valor*/
				)
{
    FILE * f_config; /*Apontador para o ficheiro das configurações*/
    char linha[MAX_LINHA+1]=""; /*String para a qual vão ser ccopiadas as linhas do ficheiro*/
    int lido, /*Guarda o número de conversões da função sscanf*/
    	i, /*Contador*/
    	n; /*Guarda o nível que está a ser lido*/
    /*Tenta abrir o ficheiro das configurações*/
    f_config = fopen(FICHEIRO_CONFIG, "r");
    if (f_config == NULL)
    {
    	/*Se não conseguiu abrir o ficheiro, termina o programa e emite uma mensagem de erro*/
        fprintf(stderr, "Erro ao abrir o ficheiro %s\n", FICHEIRO_CONFIG);
        exit(ERRO_CONFIG);
    }
    else
        printf("A ler ficheiro das configuracoes...\n");
    /*Salta os comentários iniciais*/
    skip_comentarios(linha, f_config);
    /*Lê as dimensões da janela*/
    lido = sscanf(linha, "%d %d ", &dim[LARGURA_JANELA], &dim[ALTURA_JANELA]);
    if (lido != 2)
    {
    	/*Ocorreu um erro na leitura das dimensões da janela*/
        fprintf(stderr, "Erro na leitura das dimensoes da janela\n");
        exit(FICHEIRO_MAL_FORMATADO);
    }
    else
        printf("Lidas dimensoes da janela.\n");
    skip_comentarios(linha, f_config);
    lido = sscanf(linha, "%d ", num_niveis);
    if (lido != 1)
    {
        fprintf(stderr, "Erro na leitura do numero de niveis\n");
        exit(FICHEIRO_MAL_FORMATADO);
    }
    if ((*num_niveis) > NUM_MAX_NIVEIS)
    {
        fprintf(stderr, "Número de níveis superior ao suportado pelo programa.\n");
        exit(NIVEL_OVERFLOW);
    }
    else if ((*num_niveis) <= 0)
    {
    	fprintf(stderr, "Número de níveis inferior ou igual a 0.\n");
        exit(NIVEL_UNDERFLOW);
    }
    
    skip_comentarios(linha, f_config);

    /*Lê as propriedades de cada nível*/
    for (i=0;i<(*num_niveis);i++)
    {
    	/*Lê o nivel ao qual se refere a linha*/
        lido = sscanf(linha, "%d", &n);
        if (lido != 1)
        {
            fprintf(stderr, "Erro na leitura das propriedades do nivel da linha %d\n", (i+1));
            exit(FICHEIRO_MAL_FORMATADO);
        }
        if (n<0 || n>=(*num_niveis) )
        {
            fprintf(stderr, "Erro. Numero do nivel nao esta dentro dos limites. "
                            "Número de niveis=%d, e o nivel= %d\n",
                    (*num_niveis), n);
            exit(FICHEIRO_MAL_FORMATADO);
        }
        /*Lê as propriedades do nivel n para a posição respetiva na matriz dos niveis.
        	%*d salta o primeiro número inteiro, pois já foi lido para n.
    	*/
        lido = sscanf(linha, "%*d %d %d %d %d %d %d",
                                &niveis[n][VEL],
                                &niveis[n][T_GERAR],
                                &niveis[n][TAMANHO_MIN],
                                &niveis[n][TAMANHO_MAX],
                                &niveis[n][DISPAROS],
                                &niveis[n][MUDAR_NIVEL]
                     );
        /*verifica se as propriedades foram lidas*/
        if (lido != NUM_PARAMETROS_NIVEL)
        {
            fprintf(stderr, "Erro. Nao foi possivel ler as propriedades do nivel %d\n", n);
            exit(FICHEIRO_MAL_FORMATADO);
        }
        /*Embora os niveis escritos no ficheiro comecem no zero,
        para o utilizador serão mostrados como se começassem no 1.
        */
        printf("Lido nivel %d.\n", (n+1));
        /*Se não foi lida a informação de todos os níveis, ler próxima linha*/
        if (i!=((*num_niveis)-1))
            skip_comentarios(linha, f_config);
        
    }
    
    /*Fecha o ficheiro*/
    fclose(f_config);
}

/*Salta os comentarios (linhas começadas por //) e linhas em branco.
	linha fica com a primeira linha não comentada e não nula.
	Cada linha do ficheiro não pode exceder o número máximo de carateres.
	Se existir um comentário no fim de uma linha com um parâmetro, a linha não é saltada
*/
void skip_comentarios(char linha[], FILE * f)
{
    register int i;
    while ( fgets(linha, (MAX_LINHA+1), f) != NULL /*Lê uma linha do ficheiro para linha 
    											e verifica se não chegou ao fim do ficheiro*/
			)
    {
    	/*Salta o espaço branco no início da linha*/
        for (i=0; linha[i] != '\0' && isspace(linha[i]); ++i);
        /*i tem o valor da posição do primeiro carater não nulo ou do '\0'*/
        if ( (linha[i] != '\0') /*A linha não é em branco*/
        	&& ((linha[i] != '/') || (linha[i+1] != '/')) /*A linha não é um comentário*/
        	)
            return; /*A função, quando é encontrada uma linha não em branco e não comentada*/
    }
    /*Se a execução da função chegou a este ponto, ou chegou-se ao fim do ficheiro,
    ou o ficheiro continha pelo menos uma linha com dimensão superior à permitida*/
    fprintf(stderr, "O ficheiro de configurações não segue o formato apropriado.\n");
    exit(FICHEIRO_MAL_FORMATADO);
}

/*Teste das funções de leitura do ficheiro das configurações*/
/*
int main(void)
{
    int dim[2], niveis[NUM_MAX_NIVEIS][NUM_PARAMETROS_NIVEL], n_niveis, i, j;
    void ler_config(int[], int[][NUM_PARAMETROS_NIVEL], int*);
    ler_config(dim, niveis, &n_niveis);
    printf("Dimensao da janela: %dx%d\n", dim[0], dim[1]);
    printf("Numero de niveis: %d\n", (n_niveis) );
    printf("        Vel     t_gerar tamanho_min tamanho_max    disparos mudar_nivel\n");
    for (i=0;i<n_niveis;++i)
    {
        for (j=0;j<NUM_PARAMETROS_NIVEL;j++)
        {
            printf("%11d ", niveis[i][j]);
        }
        printf("\n");
    }
    return 0;
}
*/

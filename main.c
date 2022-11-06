#include "ast.h"

/*Função principal do jogo*/
int main(void)
{
    int dim[2], /*Vetor que guarda as dimensões da janela*/
    	niveis[NUM_MAX_NIVEIS][NUM_PARAMETROS_NIVEL], /*Matriz que guarda as propriedades dos niveis*/
		num_niveis; /*Guarda o número de níveis*/
	/*Lê as configurações do jogo*/
    ler_config(dim, niveis, &num_niveis);
    /*Inicia o jogo*/
    jogar(dim, niveis, num_niveis);
    return 0;
}

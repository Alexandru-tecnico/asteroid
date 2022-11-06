#include <stdio.h>
#include <string.h>
#include <ctype.h>

/*Número máximo de carateres que o nome pode conter*/
#define NOME_MAX 99
/*Nome do ficheiro para o qual os resultados são guardados*/
#define RESULTADOS "resultados.txt"
/*Nome do ficheiro temporário para o qual os resultados são guardados*/
#define TEMP "temp.txt"

/*Funções utilizadas apenas neste ficheiro*/
void ler_nome(char[], FILE*);
void skip_linha(FILE*);

/*Função responsável por ler o nome do ficheiro dos resultados ou do teclado*/
void ler_nome(char nome[], FILE*f)
{
	register unsigned int i, j;
	char linha[NOME_MAX+1]="";
	fgets(linha,(NOME_MAX+1), f);
	for(i=0; linha[i] != '\0' && isspace(linha[i]) ; ++i); /*Salta os espaços brancos iniciais*/
	/*Copiar linha para nome*/
	j=0;
	while(linha[i] != '\0')
	{
		if (!isspace(linha[i]))
		{ /*Se não é espaço em branco, copia esse caratér*/
			nome[j] = linha[i];
			++i;
		}
		else
		{
			/*Se é espaço branco, insere um espaço e avança até ao próximo carater não branco ou o final*/
			nome[j] = ' ';
			for(; (linha[i] != '\0') && isspace(linha[i]); ++i);
		}
		++j;
	}
	if (j>=1)
		/*O nome do jogador não é só espaços brancos*/
		if (isspace(nome[j-1]))
			nome[j-1] = '\0'; /*Remove o espaço em branco no final da string*/
		else
			nome[j] = '\0'; /*Termina a string*/
	else
		/*È só espaços brancos*/
		strcpy(nome, "Anonimo");
}

/*
Função responsável por pedir o nome do jogador e guardá-lo juntamente com o resultado.
Parâmetros
	res - novo resultado a ser guardado
Retorna
	0 se não conseguiu gravar o resultado
	1 se conseguiu
*/
int guardar_resultado(unsigned int res)
{
	const char formato_escrita[] = "%5u %s\n", /*Formato de escrita do resultado e nome no ficheiro*/
			   formato_leitura[] = "%u"; /*Formato de leitura do resultado no ficheiro*/
	FILE*fr, /*Ficheiro de leitura*/
		*fw; /*Ficheiro de escrita*/
	unsigned int res_fich=0, /*Guarda um resultado guardado no ficheiro*/
				n_conv=0, /*Guarda o resultado da função scanf*/
				res_guardado=0; /*1 se o novo resultado foi guardado no ficheiro, 0 se não o foi*/
	char nome_fich[NOME_MAX+1]="", /*Nome guardado no ficheiro*/
		nome[NOME_MAX+1]=""; /*Nome do jogador que obteu o novo resultado*/
	printf("Parabens. Destruiu %u asteroides!\nPor favor, insira o seu nome:\n", res);
	/*Lê o nome do jogador*/
	ler_nome(nome, stdin);
	printf("A guardar resultado...\n");
	/*Tenta abrir o ficheiro dos resultados*/
	fr = fopen(RESULTADOS, "r");
	if (fr == NULL)
    {
    	/*Não foi possivel abrir o ficheiro dos resultados,
    	logo criar-se-á um novo ficheiro com esse nome*/
    	fw = fopen(RESULTADOS, "w");
    	if (fw == NULL)
    	{
    		/*Não foi possivel criar o ficheiro com os resultados*/
    		fprintf(stderr, "Não foi possivel guardar o resultado.\n");
    		return 0;
		}
		/*Guarda o novo resultado*/
		fprintf(fw, formato_escrita, res, nome);
		/*Encerra o ficheiro*/
		fclose(fw);
    }
    else
    {
    	/*Conseguiu-se abrir o ficheiro com os resultados antigos,
    	abir-se-á um ficheiro temporário para copiar esses resultados e
    	introduzir o novo por ordem decrescente*/
		fw = fopen(TEMP, "w");
    	if (fw == NULL)
    	{
    		/*Não foi possivel criar o ficheiro temporário*/
    		fprintf(stderr, "Não foi possivel guardar o resultado.\n");
    		/*Encerrar o ficheiro de leitura*/
    		fclose(fr);
    		return 0;
    	}
    	do {
    		/*Ler o resultado*/
    		n_conv = fscanf(fr, formato_leitura, &res_fich);
    		if (n_conv != 1) /*A linha não continha um resultado no formato certo*/
    			skip_linha(fr); /*Saltar essa linha*/
			else
			{
				/*Ler o nome do jogador com esse resultado*/
    			ler_nome(nome_fich, fr);
    			/*Se o novo resultado ainda não foi guardado e é superior ao que se
    			acabou de ler, guardar primeiro o novo resultado*/
 				if (!res_guardado && (res > res_fich) )
 				{
 					fprintf(fw, formato_escrita, res, nome);
 					res_guardado = 1; /*Registar que se guardou o novo resultado*/
				}
    			fprintf(fw, formato_escrita, res_fich, nome_fich);
			}
		} while (!feof(fr));
		/*Se o novo resultado é inferior a todos os outros, ainda não foi guardado,
		logo devemos guardá-lo*/
		if (!res_guardado)
			fprintf(fw, formato_escrita, res, nome);
		/*Encerrar ambos os ficheiros*/
		fclose(fr);
		fclose(fw);
		/*Mudar o nome do ficheiro temporário para o nome do ficheiro dos resultados*/
		rename(TEMP, RESULTADOS);
    }
    printf("Resultado guardado.\nObrigado por jogar %s.\n", nome);
	return 1;
}

/*Salta uma linha do ficheiro*/
void skip_linha(FILE*f)
{
	char c;
	while (((c=getc(f)) != EOF) && c != '\n');
}

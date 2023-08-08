#include "resumen.h"

void resumenCliente(char linea[100], char id[12], char suma[100], char nombre[100])
{
	int i = 0,j = 0, k = 0;
	while(linea[i] != '\t')
	{
		id[i] = linea[i];
		i++;
	}
	i++;
	while(linea[i] != '\n' && linea[i] != '\t' && linea[i] != ' ')
	{
		suma[j] = linea[i];
		i++;
		j++;
	}
	while(linea[i] != '\t' && linea[i] != '\n')
		i++;
	if(linea[i] != '\n')
	{
		i++;
		while(linea[i] != '\n' && linea[i] != '\t' && linea[i] != ' ')
		{
			nombre[k] = linea[i];
			i++;
			k++;
		}
	}
}

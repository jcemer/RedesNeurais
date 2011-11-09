//************************************************************************************************
//* UNIVERSIDADE FEDERAL DO RIO GRANDE DO SUL (UFRGS) - Campus do Vale                          **
//* Doutorado em Ciencia da Computacao - PPGC                                                   **
//* Doutorando: Milton Roberto Heinen - 00145752                                                **
//* Orientador: Paulo Martins Engel                                                             **
//* Simulador de redes neurais com o algoritmo backpropagation padrao (sem momentum)            **
//************************************************************************************************

//*************************************** Includes ***********************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "stlfn.h"


//************************************** Constantes **********************************************
#define MAX_LINHA 1024


//********************************** Variaveis globais *******************************************
static int iNumeroEntradas = 0;
static int iNumeroOcultos = 0;
static int iNumeroSaidas = 0;
static double **ppdPesoOculto = NULL;
static double *pdCampoOculto = NULL;
static double **ppdPesoSaida = NULL;


//*************************************** Funcoes ************************************************
int InicializarAnn(const char *szArqPesos)
{
  char vcLinha[MAX_LINHA + 1];
  char *szPalavra = NULL;
  FILE* fp = NULL;
  int i, j;

  // Salva os pesos
  if ((fp = fopen(szArqPesos, "r")) == NULL)
    return 0;

  // Busca o numero de entradas, ocultos e saidas
  fgets(vcLinha, MAX_LINHA, fp);
  szPalavra = strtok(vcLinha, " ");
  iNumeroEntradas = atoi(szPalavra);
  szPalavra = strtok('\0', " ");
  iNumeroOcultos = atoi(szPalavra);
  szPalavra = strtok('\0', " ");
  iNumeroSaidas = atoi(szPalavra);
  if (!iNumeroEntradas || !iNumeroOcultos || !iNumeroSaidas)
    return 0;
  
  // Aloca memoria para a camada oculta
  ppdPesoOculto = (double**) malloc(sizeof(double*) * iNumeroOcultos);
  for (i = 0; i < iNumeroOcultos; i++)
    ppdPesoOculto[i] = (double*) malloc(sizeof(double) * (iNumeroEntradas + 1));
  pdCampoOculto = (double*) malloc(sizeof(double) * iNumeroOcultos);

  // Aloca memoria para a camada de saida
  ppdPesoSaida = (double**) malloc(sizeof(double*) * iNumeroSaidas);
  for (i = 0; i < iNumeroSaidas; i++)
    ppdPesoSaida[i] = (double*) malloc(sizeof(double) * (iNumeroOcultos + 1));

  // Carrega os pesos da camada oculta
  for (i = 0; i < iNumeroOcultos && !feof(fp); i++) {
    fgets(vcLinha, MAX_LINHA, fp);
    szPalavra = strtok(vcLinha, " ");
    for (j = 0; j <= iNumeroEntradas && szPalavra; j++) {
      ppdPesoOculto[i][j] = atof(szPalavra);
      szPalavra = strtok('\0', " ");
    }
    if (j <= iNumeroEntradas) {
      fclose(fp);
      return 0;
    } 
  }
  if (i < iNumeroOcultos) {
    fclose(fp);
    return 0;
  } 

  // Carrega os pesos da camada de saida
  for (i = 0; i < iNumeroSaidas && !feof(fp); i++) {
    fgets(vcLinha, MAX_LINHA, fp);
    szPalavra = strtok(vcLinha, " ");
    for (j = 0; j <= iNumeroOcultos && szPalavra; j++) {
      ppdPesoSaida[i][j] = atof(szPalavra);
      szPalavra = strtok('\0', " ");
    }
    if (j <= iNumeroOcultos) {
      fclose(fp);
      return 0;
    }
  }
  fclose(fp);
  return (i < iNumeroSaidas ? 0 : 1);
}


void AtivarAnn(const double *pdEntrada, double *pdSaidaObtida)
{
  register int i, j;

  // Ativa a camada oculta
  for (i = 0; i < iNumeroOcultos; i++) {
    pdCampoOculto[i] = ppdPesoOculto[i][iNumeroEntradas];
    for (j = 0; j < iNumeroEntradas; j++)
      pdCampoOculto[i] += pdEntrada[j] * ppdPesoOculto[i][j];
    pdCampoOculto[i] = tanh(pdCampoOculto[i]);
  }

  // Ativa as saidas lineares
  for (i = 0; i < iNumeroSaidas; i++) {
    pdSaidaObtida[i] = ppdPesoSaida[i][iNumeroOcultos];
    for (j = 0; j < iNumeroOcultos; j++)
      pdSaidaObtida[i] += pdCampoOculto[j] * ppdPesoSaida[i][j];
  }
}


void FinalizarAnn()
{
  int i;

  // Desaloca a memoria da camada oculta
  if (pdCampoOculto != NULL) {
    free(pdCampoOculto);
    pdCampoOculto = NULL;
  }
  if (ppdPesoOculto != NULL) {
    for (i = 0; i < iNumeroOcultos; i++) {
      if (ppdPesoOculto[i] != NULL) {
        free(ppdPesoOculto[i]);
        ppdPesoOculto[i] = NULL;
      }
    }
    free(ppdPesoOculto);
    ppdPesoOculto = NULL;
  }

  // Desaloca a memoria da camada de saida
  if (ppdPesoSaida != NULL) {
    for (i = 0; i < iNumeroSaidas; i++) {
      if (ppdPesoSaida[i] != NULL) {
        free(ppdPesoSaida[i]);
        ppdPesoSaida[i] = NULL;
      }
    }
    free(ppdPesoSaida);
    ppdPesoSaida = NULL;
  }
}

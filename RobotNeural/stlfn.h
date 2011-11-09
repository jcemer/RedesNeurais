//************************************************************************************************
//* UNIVERSIDADE FEDERAL DO RIO GRANDE DO SUL (UFRGS) - Campus do Vale                          **
//* Doutorado em Ciencia da Computacao - PPGC                                                   **
//* Doutorando: Milton Roberto Heinen - 00145752                                                **
//* Orientador: Paulo Martins Engel                                                             **
//* Simulador de redes neurais com o algoritmo backpropagation padrao (sem momentum)            **
//************************************************************************************************
#ifndef STLFN_H
#define STLFN_H


//************************************** Prototipos **********************************************
int InicializarAnn(const char *szArqPesos);
void AtivarAnn(const double *pdEntrada, double *pdSaidaObtida);
void FinalizarAnn();

#endif

//************************************************************************************************
//* UNIVERSIDADE FEDERAL DO RIO GRANDE DO SUL (UFRGS) - Campus do Vale                          **
//* Doutorado em Ciencia da Computacao - PPGC                                                   **
//* Doutorando: Milton Roberto Heinen - 00145752                                                **
//* Orientador: Paulo Martins Engel                                                             **
//* Simulador de redes neurais com o algoritmo backpropagation padrao (sem momentum)            **
//************************************************************************************************

//*************************************** Includes ***********************************************
#include "environm.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "stlfn.h"


//********************************** Variaveis globais *******************************************
double *pdEntrada = NULL;
double *pdSaidaObtida = NULL;


//************************************* Funcao main **********************************************
int main(int argc, char* argv[]) 
{    
  int i;
  
  // Verifica os parametros
  if (argc < 4) {
    printf("USO: %s <server_address> <port_number> <file_weights.wts>\n", argv[0]);    
    getchar();
    return 0;
  }  
  
  // Carrega a rede neural, as entradas e as saidas
  InicializarAnn(argv[3]);
  pdEntrada = (double*) malloc(sizeof(double) * 8);
  pdSaidaObtida = (double*) malloc(sizeof(double) * 2);
  pdSaidaObtida[0] = pdSaidaObtida[1] = 0.0;
  
  // Cria e inicializa o ambiente
  environm::soccer::clientEnvironm environment;
  if (!environment.connect(argv[1], atoi(argv[2]))) {
    printf("Fail connecting to the SoccerMatch...\n");
    getchar();
    return 0;
  }

  // Laço de execução de ações
  for (;;) {
    // Obtem os dados do ambiente
    pdEntrada[0] = environment.getDistance();
    pdEntrada[1] = environment.getBallAngle();
    pdEntrada[2] = environment.getTargetAngle( environment.getOwnGoal() );
    pdEntrada[3] = environment.getCollision();
    pdEntrada[4] = environment.getObstacleAngle();
    pdEntrada[5] = environment.getSpin();
    pdEntrada[6] = pdSaidaObtida[0]; 
    pdEntrada[7] = pdSaidaObtida[1];
    // Ativa a rede neural
    AtivarAnn(pdEntrada, pdSaidaObtida);
    // Transmite ação do robô ao ambiente
    if (!environment.act(pdSaidaObtida[0], pdSaidaObtida[1])) {
      break;
    }
  }
  
  // Finaliza
  FinalizarAnn();
  if (pdSaidaObtida != NULL) 
    free(pdSaidaObtida);
  if (pdEntrada != NULL) 
    free(pdEntrada);
  return 0;
}

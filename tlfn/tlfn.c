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
#include <time.h>
#include <math.h>


//************************************** Constantes **********************************************
#define MAX_LINHA 1024
#define INIT_PESOS 0.01
#define PASSO 0.00001
#define MAX_EPOCAS 10000
#define FREQ_GENERAL 10
#define NUM_OCULTOS 5
#define FREQ_RELATOR 500


//**************************************** Macros ************************************************
#define QUADRADO(x) ((x) * (x))


//********************************** Variaveis globais *******************************************
int iNumeroEntradas = 0;
int iNumeroSaidas = 0;
int iNumeroRegistrosTreino = 0;
int iNumeroRegistrosGenera = 0;
int iNumeroOcultos = NUM_OCULTOS;
int iMaximoEpocas = MAX_EPOCAS;
int iFreqGeneral = FREQ_GENERAL;
int iFreqRelator = FREQ_RELATOR;
int iEncerrarAprendizado = 0;
int iRealizarAprendizado = 1;
unsigned long ulRandomSeed = 0;
double **ppdDatabaseTreino = NULL;
double **ppdDatabaseGenera = NULL;
double **ppdPesoOculto = NULL;
double *pdCampoOculto = NULL;
double **ppdPesoSaida = NULL;
double *pdSaidaObtida = NULL;
double *pdAjusteSaida = NULL;
double dInitPesos = INIT_PESOS;
double dPasso = PASSO;
char vcArquivoTreino[MAX_LINHA + 1];
char vcArquivoGenera[MAX_LINHA + 1];
char vcArquivoPesos[MAX_LINHA + 1];
char vcArquivoSaida[MAX_LINHA + 1];


//************************************** Prototipos **********************************************
void ProcessaLinhaComando(int argc, char *argv[]);
double **CarregarDatabase(const char *szNomeArquivo, int *iNumeroRegistros);
void AlocarMemoriaAnn();
void InicializarPesos();
void AlteraCamadaOculta(const int iNumNeronios);
double RealizarAprendizado();
inline void AtivarAnn(const double *pdRegistro);
inline void AjustarPesos(const double *pdRegistro);
inline int SaidaCorreta(const double *pdSaidaDesej);
inline double CalcularErroQuadrado(const double *pdSaidaDesej);
int CarregarPesos(const char *szNomeArquivo);
void SalvarPesos(const char *szNomeArquivo);
void MostrarPesos();
void TestarDatabase(double** ppdDatabase, const int iNumRegistros);
void GerarArquivoSaidas(double** ppdDatabase, const int iNumRegistros, const char *szNomeArquivo);
void DesalocarMemoriaAnn();
void DesalocarDatabase(double** ppdDatabase, int iNumRegistros);


//************************************* Funcao main **********************************************
int main (int argc, char *argv[])
{
  // Busca os parametros de treinamento na linha de comando
  if (argc < 3) {
    printf("Uso: %s <arquivo_sem_extensao> [-o num_ocultos=%d] [-p passo=%f] [-i init_pesos=%f] [-e max_epocas=%d] [-g freq_general=%d] [-r freq_rel=%d] [-s random_seed] [-t]\n", 
        argv[0], iNumeroOcultos, dPasso, dInitPesos, iMaximoEpocas, iFreqGeneral, iFreqRelator);
    printf("Pressione <enter> para encerrar...");
    getchar();
    return 0;
  }
  ProcessaLinhaComando(argc, argv);

  // Carrega as bases de dados
  sprintf(vcArquivoTreino, "%s.lrn", argv[1]);
  sprintf(vcArquivoGenera, "%s.tst", argv[1]);
  sprintf(vcArquivoPesos, "%s.wts", argv[1]);
  sprintf(vcArquivoSaida, "%s.out", argv[1]);
  if ((ppdDatabaseTreino = CarregarDatabase(vcArquivoTreino, &iNumeroRegistrosTreino)) == NULL)
    return 1;
  if ((ppdDatabaseGenera = CarregarDatabase(vcArquivoGenera, &iNumeroRegistrosGenera)) == NULL) {
    DesalocarDatabase(ppdDatabaseTreino, iNumeroRegistrosTreino);
    return 1;
  }

  // Inicializa o random seed
  if (!ulRandomSeed)
    ulRandomSeed = time(NULL);
  srand(ulRandomSeed);

  // Prepara a ANN
  AlocarMemoriaAnn();
  if (!iRealizarAprendizado) {
    // Carrega os pesos e testa o database
    if (!CarregarPesos(vcArquivoPesos))
      return 1;
    TestarDatabase(ppdDatabaseGenera, iNumeroRegistrosGenera);
  }
  else {
    InicializarPesos();  
    // Imprime os parametros da simulacao
    printf("*******************************************************\n");
    printf("* Arquivo de treinamento......: %-13s (%-5d) *\n", vcArquivoTreino, iNumeroRegistrosTreino);
    printf("* Arquivo de generalizacao....: %-13s (%-5d) *\n", vcArquivoGenera, iNumeroRegistrosGenera);
    printf("* Neuronios nas camadas.......: %-4d %-4d %-4d        *\n", iNumeroEntradas,
        iNumeroOcultos, iNumeroSaidas);
    printf("* Passo, init_pesos e epocas..: %7.5f %6.4f %6d *\n", dPasso, dInitPesos, iMaximoEpocas);
    printf("* Generalizacao e random seed.: %-5d %.10lu      *\n", iFreqGeneral, ulRandomSeed);
    printf("*******************************************************\n");

    // Realiza o aprendizado
    RealizarAprendizado();
  }

  // Finalizacao
  DesalocarMemoriaAnn();
  DesalocarDatabase(ppdDatabaseTreino, iNumeroRegistrosTreino);
  DesalocarDatabase(ppdDatabaseGenera, iNumeroRegistrosGenera);
  printf("Pressione <enter> para encerrar...");
  getchar();
  return 0;
}


//************************************** Demais funcoes ******************************************
void ProcessaLinhaComando(int argc, char *argv[])
{
  int i;

  // Busca os parametros da linha de comando
  for (i = 2; i < argc; i++) {
    if (argv[i][0] == '-' && (i < argc - 1 || argv[i][1] == 't')) {
      switch(argv[i][1]) {
        case 'o':
          iNumeroOcultos = atoi(argv[i + 1]);
          break;
        case 'i':
          dInitPesos = atof(argv[i + 1]);
          break;
        case 'p':
          dPasso = atof(argv[i + 1]);
          break;
        case 'e':
          iMaximoEpocas = atoi(argv[i + 1]);
          break;
        case 's':
          ulRandomSeed = atol(argv[i + 1]);
          break;
        case 'g':
          iFreqGeneral = atoi(argv[i + 1]);
          break;
        case 'r':
          iFreqRelator = atoi(argv[i + 1]);
          break;
        case 't':
          iRealizarAprendizado = 0;
          break;
      }
    }
  }
  if (iFreqRelator < iFreqGeneral)
    iFreqRelator = iFreqGeneral;
}


double **CarregarDatabase(const char *szNomeArquivo, int *iNumeroRegistros)
{
  FILE *fp = NULL;
  char vcLinha[MAX_LINHA + 1];
  char *szPalavra = NULL;
  int i, iCount;
  double **ppdDatabase = NULL;

  // Abre o arquivo
  if ((fp = fopen(szNomeArquivo, "r")) == NULL) {
    fprintf(stderr, "ERRO: Nao foi possivel abrir o database\n");
    return NULL;
  }

  // Busca o numero de entradas, saidas e registros
  fgets(vcLinha, MAX_LINHA, fp);
  szPalavra = strtok(vcLinha, " ");
  iNumeroEntradas = atoi(szPalavra);
  szPalavra = strtok('\0', " ");
  iNumeroSaidas = atoi(szPalavra);
  szPalavra = strtok('\0', " ");
  (*iNumeroRegistros) = atoi(szPalavra);

  // Aloca memoria para o database
  ppdDatabase = (double**) malloc(sizeof(double*) * (*iNumeroRegistros));
  for (i = 0; i < (*iNumeroRegistros); i++)
    ppdDatabase[i] = (double*) malloc(sizeof(double) * (iNumeroEntradas + iNumeroSaidas));

  // Le o arquivo e preenche o database em memoria
  for (iCount = 0; iCount < (*iNumeroRegistros); iCount++) {
    fgets(vcLinha, MAX_LINHA, fp);
    szPalavra = strtok(vcLinha, " ");
    for (i = 0; i < iNumeroEntradas + iNumeroSaidas; i++) {
      ppdDatabase[iCount][i] = atof(szPalavra);
      szPalavra = strtok('\0', " ");
    }
  }

  // Se chegar aqui eh porque deu tudo certo
  fclose(fp);
  return ppdDatabase;
}


void EmbaralharDatabase(double **ppdDatabase, int iNumRegistros)
{
  int i, iPosicao;
  double *pdAux;

  // Embaralha o database
  for (i = 0; i < iNumRegistros; i++) {
    do
      iPosicao = (int) ((double) rand() / RAND_MAX * iNumRegistros);
    while (iPosicao < 0 || iPosicao >= iNumRegistros);
    pdAux = ppdDatabase[i];
    ppdDatabase[i] = ppdDatabase[iPosicao];
    ppdDatabase[iPosicao] = pdAux;
  }
}


void AlocarMemoriaAnn()
{
  int i;

  // Aloca memoria para a camada oculta
  ppdPesoOculto = (double**) malloc(sizeof(double*) * iNumeroOcultos);
  for (i = 0; i < iNumeroOcultos; i++)
    ppdPesoOculto[i] = (double*) malloc(sizeof(double) * (iNumeroEntradas + 1));
  pdCampoOculto = (double*) malloc(sizeof(double) * iNumeroOcultos);

  // Aloca memoria para a camada de saida
  ppdPesoSaida = (double**) malloc(sizeof(double*) * iNumeroSaidas);
  for (i = 0; i < iNumeroSaidas; i++)
    ppdPesoSaida[i] = (double*) malloc(sizeof(double) * (iNumeroOcultos + 1));
  pdSaidaObtida = (double*) malloc(sizeof(double) * iNumeroSaidas);
  pdAjusteSaida = (double*) malloc(sizeof(double) * iNumeroSaidas);
}


void InicializarPesos()
{
  int i, j;

  // Inicializa os pesos ocultos e os passos iniciais
  for (i = 0; i < iNumeroOcultos; i++) {
    for (j = 0; j <= iNumeroEntradas; j++)
      ppdPesoOculto[i][j] = ((double) rand() / RAND_MAX - 0.5) * dInitPesos * 2.0;
  }

  // Inicializa os pesos de saida e os passos iniciais
  for (i = 0; i < iNumeroSaidas; i++) {
    for (j = 0; j <= iNumeroOcultos; j++)
      ppdPesoSaida[i][j] = ((double) rand() / RAND_MAX - 0.5) * dInitPesos * 2.0;
  }
}


void AlteraCamadaOculta(const int iNumNeronios)
{
  int i;

  // Desaloca a memoria dos neuronios individuais se a camada diminuir
  for (i = iNumNeronios; i < iNumeroOcultos; i++) {
    if (ppdPesoOculto[i] != NULL) {
      free(ppdPesoOculto[i]);
      ppdPesoOculto[i] = NULL;
    }
  }

  // Altera o tamanho da camada oculta
  ppdPesoOculto = (double**) realloc(ppdPesoOculto, sizeof(double*) * iNumNeronios);
  pdCampoOculto = (double*) realloc(pdCampoOculto, sizeof(double) * iNumNeronios);

   // Aloca a memoria dos neuronios individuais se a camada aumentar
  for (i = iNumeroOcultos; i < iNumNeronios; i++)
    ppdPesoOculto[i] = (double*) malloc(sizeof(double) * (iNumeroEntradas + 1));

  // Altera o numero de pesos na camada de saida
  for (i = 0; i < iNumeroSaidas; i++)
    ppdPesoSaida[i] = (double*) realloc(ppdPesoSaida[i], sizeof(double) * (iNumNeronios + 1));

  // Altera o tamanho da camada na variavel global
  iNumeroOcultos = iNumNeronios;
}


double RealizarAprendizado()
{
  register int k, l;
  int iMelhorEpoca = 0;
  double dErroMedioTreino = 0.0, dErroMedioTeste = 0.0, dMenorErro = 1.0e32;
  clock_t clInicio = clock();

  // Realiza o aprendizado neural
  for (l = 1; l <= iMaximoEpocas && !iEncerrarAprendizado; l++) {
    // Treina uma epoca
    EmbaralharDatabase(ppdDatabaseTreino, iNumeroRegistrosTreino);
    for (k = 0; k < iNumeroRegistrosTreino; k++) {
      AtivarAnn(ppdDatabaseTreino[k]);
      AjustarPesos(ppdDatabaseTreino[k]);
      // Calcula as estatisticas do treinamento na epoca de relatorio
      if (!(l % iFreqRelator))
        dErroMedioTreino += CalcularErroQuadrado(&ppdDatabaseTreino[k][iNumeroEntradas]);
    }

    // Teste de generalizacao
    if (!(l % iFreqGeneral)) {
      for (k = 0; k < iNumeroRegistrosGenera; k++) {
        // Ativa a ANN e verifica os acertos
        AtivarAnn(ppdDatabaseGenera[k]);
        dErroMedioTeste += CalcularErroQuadrado(&ppdDatabaseGenera[k][iNumeroEntradas]);
      }
      dErroMedioTeste /= (double) iNumeroSaidas * iNumeroRegistrosGenera;
      // Exibe as estatisticas
      if (!(l % iFreqRelator)) {
        printf("* EPOCA:%6d * TREINO: %9.6f * TESTE: %9.6f *\n", l,
            dErroMedioTreino / (double) (iNumeroSaidas * iNumeroRegistrosTreino), dErroMedioTeste);
      }
      // Verifica se foi a melhor epoca
      if (dMenorErro > dErroMedioTeste) {
        dMenorErro = dErroMedioTeste;
        iMelhorEpoca = l;
        // Salva as ativacoes e os pesos na melhor epoca
        GerarArquivoSaidas(ppdDatabaseGenera, iNumeroRegistrosGenera, vcArquivoSaida);
        SalvarPesos(vcArquivoPesos);
      }
      // Reinicializa as estatisticas
      dErroMedioTreino = dErroMedioTeste = 0.0;

      // Verifica a parada prematura
      if (l - iMelhorEpoca > iMaximoEpocas / 5) {
        iEncerrarAprendizado = 1;
      }
    }
  }

  // Relatorio final
  printf("*******************************************************\n");
  printf("* Melhor epoca: %-6d          MSE: %8.6f         *\n", iMelhorEpoca, dMenorErro);
  printf("* Tempo total de aprendizado:%7.2f segundos         *\n",
      (double) (clock() - clInicio) / CLOCKS_PER_SEC);
  printf("*******************************************************\n");

  // Retorna o erro MSE da melhor epoca
  return dMenorErro;
}


inline void AtivarAnn(const double *pdRegistro)
{
  register int i, j;

  // Ativa a camada oculta
  for (i = 0; i < iNumeroOcultos; i++) {
    pdCampoOculto[i] = ppdPesoOculto[i][iNumeroEntradas];
    for (j = 0; j < iNumeroEntradas; j++)
      pdCampoOculto[i] += pdRegistro[j] * ppdPesoOculto[i][j];
    pdCampoOculto[i] = tanh(pdCampoOculto[i]);
  }

  // Ativa as saidas lineares
  for (i = 0; i < iNumeroSaidas; i++) {
    pdSaidaObtida[i] = ppdPesoSaida[i][iNumeroOcultos];
    for (j = 0; j < iNumeroOcultos; j++)
      pdSaidaObtida[i] += pdCampoOculto[j] * ppdPesoSaida[i][j];
  }
}


inline void AjustarPesos(const double *pdRegistro)
{
  register int i, j;
  double dAjuste;

  // Calcula o ajuste das saidas
  for (i = 0; i < iNumeroSaidas; i++)
    pdAjusteSaida[i] = (pdRegistro[iNumeroEntradas + i] - pdSaidaObtida[i]);

  // Ajusta os pesos da camada oculta
  for (i = 0; i < iNumeroOcultos; i++) {
    dAjuste = 0.0;
    for (j = 0; j < iNumeroSaidas; j++)
      dAjuste += pdAjusteSaida[j] * ppdPesoSaida[j][i];
    dAjuste *= dPasso * (1.0 - QUADRADO(pdCampoOculto[i]));
    for (j = 0; j < iNumeroEntradas; j++)
      ppdPesoOculto[i][j] += dAjuste * pdRegistro[j];
    ppdPesoOculto[i][iNumeroEntradas] += dAjuste;
  }

  // Ajusta os pesos da camada de saida
  for (i = 0; i < iNumeroSaidas; i++) {
    dAjuste = dPasso * pdAjusteSaida[i];
    for (j = 0; j < iNumeroOcultos; j++)
      ppdPesoSaida[i][j] += dAjuste * pdCampoOculto[j];
    ppdPesoSaida[i][iNumeroOcultos] += dAjuste;
  }
}


inline double CalcularErroQuadrado(const double *pdSaidaDesej)
{
  register int i;
  double dErro = 0.0;

  // Calcula a soma do erro quadrado de todas as saidas
  for (i = 0; i < iNumeroSaidas; i++)
    dErro += QUADRADO(pdSaidaDesej[i] - pdSaidaObtida[i]);
  return dErro;
}


int CarregarPesos(const char *szNomeArquivo)
{
  FILE* fp = NULL;
  char vcLinha[MAX_LINHA + 1];
  char *szPalavra = NULL;
  int i, j;

  // Salva os pesos
  if ((fp = fopen(szNomeArquivo, "r")) == NULL)
    return 0;

  // Busca o numero de entradas, ocultos e saidas
  fgets(vcLinha, MAX_LINHA, fp);
  szPalavra = strtok(vcLinha, " ");
  iNumeroEntradas = atoi(szPalavra);
  szPalavra = strtok('\0', " ");
  iNumeroOcultos = atoi(szPalavra);
  szPalavra = strtok('\0', " ");
  iNumeroSaidas = atoi(szPalavra);

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


void SalvarPesos(const char *szNomeArquivo)
{
  FILE* fp = NULL;
  int i, j;

  // Salva os pesos
  if ((fp = fopen(szNomeArquivo, "w")) == NULL)
    return;
  fprintf(fp, "%d %d %d\n", iNumeroEntradas, iNumeroOcultos, iNumeroSaidas);

  // Salva os pesos da camada oculta
  for (i = 0; i < iNumeroOcultos; i++) {
    for (j = 0; j <= iNumeroEntradas; j++)
      fprintf(fp, "%.8f ", ppdPesoOculto[i][j]);
    fprintf(fp, "\n");
  }

  // Salva os pesos da camada de saida
  for (i = 0; i < iNumeroSaidas; i++) {
    for (j = 0; j <= iNumeroOcultos; j++)
      fprintf(fp, "%.8f ", ppdPesoSaida[i][j]);
    fprintf(fp, "\n");
  }
  fclose(fp);
}


void MostrarPesos()
{
  int i, j;

  // Mostra os pesos
  printf("%d %d %d\n", iNumeroEntradas, iNumeroOcultos, iNumeroSaidas);

  // Mostra os pesos da camada oculta
  for (i = 0; i < iNumeroOcultos; i++) {
    for (j = 0; j <= iNumeroEntradas; j++)
      printf("%.8f ", ppdPesoOculto[i][j]);
    printf("\n");
  }

  // Mostra os pesos da camada de saida
  for (i = 0; i < iNumeroSaidas; i++) {
    for (j = 0; j <= iNumeroOcultos; j++)
      printf("%.8f ", ppdPesoSaida[i][j]);
    printf("\n");
  }
}


void TestarDatabase(double** ppdDatabase, const int iNumRegistros)
{
  double dErroMedio = 0.0;
  int i;

  for (i = 0; i < iNumRegistros; i++) {
    AtivarAnn(ppdDatabase[i]);
    dErroMedio += CalcularErroQuadrado(&ppdDatabase[i][iNumeroEntradas]);
  }
  printf("MSE: %f\n", dErroMedio / (double) (iNumeroSaidas * iNumRegistros));
}


void GerarArquivoSaidas(double** ppdDatabase, const int iNumRegistros, const char *szNomeArquivo)
{
  int i, j;
  double dErroMedio = 0.0;
  FILE* fp = NULL;

  if ((fp = fopen(szNomeArquivo, "w")) == NULL)
    return;
  for (i = 0; i < iNumRegistros; i++) {
    AtivarAnn(ppdDatabase[i]);
    dErroMedio += CalcularErroQuadrado(&ppdDatabase[i][iNumeroEntradas]);
    for (j = 0; j < iNumeroSaidas; j++)
      fprintf(fp, "%f %f   ", ppdDatabase[i][iNumeroEntradas + j], pdSaidaObtida[j]);
    fprintf(fp, "\n");
  }
  fprintf(fp, "MSE: %f\n", dErroMedio / (double) (iNumeroSaidas * iNumRegistros));
  fclose(fp);
}


void DesalocarMemoriaAnn()
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
  if (pdSaidaObtida != NULL) {
    free(pdSaidaObtida);
    pdSaidaObtida = NULL;
  }
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
  if (pdAjusteSaida != NULL) {
    free(pdAjusteSaida);
    pdAjusteSaida = NULL;
  }
}


void DesalocarDatabase(double** ppdDatabase, int iNumRegistros)
{
  int i;

  // Desaloca a memoria do database
  if (ppdDatabase != NULL) {
    for (i = 0; i < iNumRegistros; i++) {
      if (ppdDatabase[i] != NULL) {
        free(ppdDatabase[i]);
        ppdDatabase[i] = NULL;
      }
    }
    free(ppdDatabase);
    ppdDatabase = NULL;
  }
}

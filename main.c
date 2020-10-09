#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define TAM_MAX 200
#define TAB_MAX 26

// TIPO COORDENADA
typedef struct
{
  int x;
  int y;
} posicao_e;

// TIPO TABULEIRO
typedef struct
{
  int inicializado;
  int pinos;
  int tabuleiro_jogo[TAB_MAX][TAB_MAX];
  int movimentosPossiveis[TAB_MAX][TAB_MAX];
  posicao_e tamanho;
  posicao_e pinoSelecionado;
} tabuleiro_e;

// FUNCOES
void imprimirTabuleiro(tabuleiro_e tabuleiro);
void imprimirUI(tabuleiro_e tabuleiro, char erro[200], char mensagem[200]);
void limparTerminal();
int coordNumeroParaLetra(int numero, char letra);
int moverPraCima(tabuleiro_e *tabuleiro);
int moverPraBaixo(tabuleiro_e *tabuleiro);
int moverPraEsquerda(tabuleiro_e *tabuleiro);
int moverPraDireita(tabuleiro_e *tabuleiro);
int ajudaMovimentos(tabuleiro_e *tabuleiro, int nMovimentos);
tabuleiro_e abrirDocumento(tabuleiro_e tabuleiro, char nomeArquivo[TAM_MAX]);
tabuleiro_e tabuleiroAleatorio(tabuleiro_e tabuleiro, char nomeArquivo[TAM_MAX]);
int salvarArquivo(tabuleiro_e tabuleiro, char nomeArquivo[TAM_MAX]);
void verificarMovimentosPossiveis(tabuleiro_e *tabuleiro);
int verificaVitoria(tabuleiro_e tabuleiro);

// MAIN
int main(int argc, char *argv[])
{
  tabuleiro_e tabuleiro;
  tabuleiro.inicializado = 0;
  tabuleiro.pinos = 0;

  int sair, numeroAjuda;
  sair = 0;

  char erro[TAM_MAX], nomeArquivo[TAM_MAX], comandoDigitado[TAM_MAX];

  erro[0] = '\0';

  // ACESSAR AQUIVO PELO ARGUMENTO ou criar um aleatorio
  if (argv[1] == NULL)
  {
    strcpy(nomeArquivo, "aleatorio.txt\0");
  }
  else
  {
    strcpy(nomeArquivo, argv[1]);
  }

  tabuleiro = abrirDocumento(tabuleiro, nomeArquivo);
  verificarMovimentosPossiveis(&tabuleiro);
  sair = verificaVitoria(tabuleiro);
  while (!sair)
  {
    imprimirUI(tabuleiro, erro, "Comando: ");
    strcpy(erro, "\0");

    fgets(comandoDigitado, TAM_MAX - 1, stdin);
    comandoDigitado[strlen(comandoDigitado) - 1] = '\0';

    if (strcmp(comandoDigitado, "sair") == 0)
    {
      sair = 1;
    }
    else if (strncmp(comandoDigitado, "salvar", 6) == 0)
    {
      if (strlen(comandoDigitado) > 7)
      {
        if (strcmp(&comandoDigitado[strlen(comandoDigitado) - 4], ".txt") == 0)
        {
          strcpy(nomeArquivo, &comandoDigitado[7]);

          if (salvarArquivo(tabuleiro, nomeArquivo))
          {
            strcpy(erro, "Salvo com Sucesso!!");
          }
          else
          {
            strcpy(erro, "Erro: Nao foi possivel salvar o arquivo!!");
          }
        }
        else
        {
          strcpy(erro, "Erro: Nao foi colocado a extensao do arquivo txt!!");
        }
      }
      else
      {
        strcpy(erro, "Erro: Nao foi colocado o nome do arquivo!!");
      }
    }
    else if (strncmp(comandoDigitado, "ajuda", 5) == 0)
    {
      if (comandoDigitado[5] == ' ' && comandoDigitado[6] >= '1' && comandoDigitado[6] <= '9')
      {
        numeroAjuda = atoi(&comandoDigitado[6]);
        if (ajudaMovimentos(&tabuleiro, numeroAjuda))
        {
          strcat(erro, "Ajuda finalizada!");
        }
        else
        {

          sair = 1;
        }
      }
      else
      {
        strcat(erro, "Erro: nao foi digitado o numero de movimentos de ajuda!");
      }
    }
    else if (comandoDigitado[1] == ' ')
    {
      tabuleiro.pinoSelecionado.x = coordNumeroParaLetra(0, comandoDigitado[2]);
      tabuleiro.pinoSelecionado.y = coordNumeroParaLetra(0, comandoDigitado[3]);
      if (comandoDigitado[4] == '\0' && (tabuleiro.pinoSelecionado.x >= 0 && tabuleiro.pinoSelecionado.x <= tabuleiro.tamanho.x) && (tabuleiro.pinoSelecionado.y >= 0 && tabuleiro.pinoSelecionado.y <= tabuleiro.tamanho.y))
      {
        switch (comandoDigitado[0])
        {
        case 'c':
        {
          if (moverPraCima(&tabuleiro))
          {
            strcpy(erro, "Pino ");
            strcat(erro, &comandoDigitado[2]);
            strcat(erro, " movido para cima com sucesso!");
          }
          else
          {
            strcpy(erro, "Nao eh possivel mover esse pino para cima!");
          }
          break;
        }
        case 'b':
        {
          if (moverPraBaixo(&tabuleiro))
          {
            strcpy(erro, "Pino ");
            strcat(erro, &comandoDigitado[2]);
            strcat(erro, " movido para baixo com sucesso!");
          }
          else
          {
            strcpy(erro, "Nao eh possivel mover esse pino para baixo!");
          };
          break;
        }
        case 'e':
        {
          if (moverPraEsquerda(&tabuleiro))
          {
            strcpy(erro, "Pino ");
            strcat(erro, &comandoDigitado[2]);
            strcat(erro, " movido para a esquerda com sucesso!");
          }
          else
          {
            strcpy(erro, "Nao eh possivel mover esse pino para a esquerda!");
          }
          break;
        }
        case 'd':
        {

          if (moverPraDireita(&tabuleiro))
          {
            strcpy(erro, "Pino ");
            strcat(erro, &comandoDigitado[2]);
            strcat(erro, " movido para a direita com sucesso!");
          }
          else
          {
            strcpy(erro, "Nao eh possivel mover esse pino para a direita!");
          }
          break;
        }
        default:
        {
          strcpy(erro, "Comando nao reconhecido...");
          break;
        }
        }

        verificarMovimentosPossiveis(&tabuleiro);
        sair = verificaVitoria(tabuleiro);
      }
      else
      {
        strcpy(erro, "Erro: Esse pino nao existe!!");
      }
    }
    else
    {
      strcpy(erro, "Comando nao reconhecido...");
    }
  }

  printf("\n");
  return 0;
}

int verificaVitoria(tabuleiro_e tabuleiro)
{
  int i, j, somaMatrizMovimentos;
  somaMatrizMovimentos = 0;

  if (tabuleiro.pinos == 1)
  {
    imprimirUI(tabuleiro, "\0", "\0");
    printf("\033[0;32mVoce ganhou!! Parabens!!\nAgora so resta um...\033[0m");
    return 1;
  }
  else
  {
    for (i = 0; i < tabuleiro.tamanho.x; i++)
    {
      for (j = 0; j < tabuleiro.tamanho.y; j++)
      {
        somaMatrizMovimentos += tabuleiro.movimentosPossiveis[i][j];
      }
    }

    if (somaMatrizMovimentos > 0)
    {
      return 0;
    }
    else
    {
      imprimirUI(tabuleiro, "\0", "\0");
      printf("\033[0;31mIhh meu parceiro, voce perdeu!! Que pena...\nNão existem mais movimentos possiveis...\033[0m");
      return 1;
    }
  }
}

// INICIAR DOCUMENTO
tabuleiro_e abrirDocumento(tabuleiro_e tabuleiro, char nomeArquivo[TAM_MAX])
{
  int i, j;
  char erro[TAM_MAX];
  FILE *file_ptr;

  erro[0] = '\0';
  srand(time(0));

  if ((file_ptr = fopen(nomeArquivo, "r")) != NULL && strcmp(nomeArquivo, "aleatorio.txt") != 0) //VERIFICA SE O AQUIVO JA ESXITE, SE SIM LE AS INFORMACOES DELE
  {
    fscanf(file_ptr, "%d %d", &tabuleiro.tamanho.x, &tabuleiro.tamanho.y);

    for (i = 0; i < tabuleiro.tamanho.x; i++)
    {
      for (j = 0; j < tabuleiro.tamanho.y; j++)
      {
        fscanf(file_ptr, "%d", &tabuleiro.tabuleiro_jogo[i][j]);
        tabuleiro.pinos += (tabuleiro.tabuleiro_jogo[i][j] == 1);
      }
    }
    tabuleiro.inicializado = 1;
    fclose(file_ptr);
  }
  else if (strcmp(nomeArquivo, "aleatorio.txt") != 0) // SE NAO CRIA ELE E PEDE AS INFORMACOES
  {
    printf("Digite o valor para N e M: ");
    scanf("%d %d", &tabuleiro.tamanho.x, &tabuleiro.tamanho.y);
    getchar();

    for (i = 0; i < tabuleiro.tamanho.x; i++)
    {
      for (j = 0; j < tabuleiro.tamanho.y; j++)
      {
        tabuleiro.tabuleiro_jogo[i][j] = -1;
      }
    }
    tabuleiro.pinoSelecionado.x = tabuleiro.pinoSelecionado.y = 0;

    for (i = 0; i < tabuleiro.tamanho.x; i++)
    {
      for (j = 0; j < tabuleiro.tamanho.y; j++)
      {
        do
        {
          imprimirUI(tabuleiro, erro, "Digite o VALOR da posicao: ");
          scanf("%d", &tabuleiro.tabuleiro_jogo[i][j]);
          getchar();
          if (tabuleiro.tabuleiro_jogo[i][j] < -1 || tabuleiro.tabuleiro_jogo[i][j] > 1)
          {
            strcpy(erro, "So sao aceitos os VALORES: -1, 1 ou 0");
            tabuleiro.tabuleiro_jogo[i][j] = -1;
          }
          else
          {
            tabuleiro.pinos += (tabuleiro.tabuleiro_jogo[i][j] == 1);
            strcpy(erro, "\0");
            break;
          }
        } while (1);

        if (j == (tabuleiro.tamanho.y - 1))
        {
          tabuleiro.pinoSelecionado.x = i + 1;
          tabuleiro.pinoSelecionado.y = 0;
        }
        else
        {
          tabuleiro.pinoSelecionado.x = i;
          tabuleiro.pinoSelecionado.y = j + 1;
        }
      }
    }

    if (!salvarArquivo(tabuleiro, nomeArquivo))
    {
      imprimirUI(tabuleiro, "OCORREU ALGUM ERRO NO ARQUIVO", "FECHANDO O PROGRAMA...\n");
      exit(1);
    }

    tabuleiro.inicializado = 1;
  }
  else
  {
    tabuleiro.tamanho.x = rand() % 26 + 1;
    tabuleiro.tamanho.y = rand() % 26 + 1;

    for (i = 0; i < tabuleiro.tamanho.x; i++)
    {
      for (j = 0; j < tabuleiro.tamanho.y; j++)
      {
        tabuleiro.tabuleiro_jogo[i][j] = (rand() % 3) - 1;
        tabuleiro.pinos += (tabuleiro.tabuleiro_jogo[i][j] == 1);
        tabuleiro.inicializado = 1;
      }
    }

    if (!salvarArquivo(tabuleiro, nomeArquivo))
    {
      imprimirUI(tabuleiro, "OCORREU ALGUM ERRO NO ARQUIVO", "FECHANDO O PROGRAMA...\n");
      exit(1);
    }
  }

  return tabuleiro;
}

// SALVAR ARQUIVO
int salvarArquivo(tabuleiro_e tabuleiro, char nomeArquivo[TAM_MAX])
{
  int i, j;
  FILE *file_ptr;

  file_ptr = fopen(nomeArquivo, "w");

  fprintf(file_ptr, "%d %d\n\n", tabuleiro.tamanho.x, tabuleiro.tamanho.y);

  for (i = 0; i < tabuleiro.tamanho.x; i++)
  {
    for (j = 0; j < tabuleiro.tamanho.y; j++)
    {
      fprintf(file_ptr, "%2d ", tabuleiro.tabuleiro_jogo[i][j]);
    }
    fprintf(file_ptr, "\n");
  }

  fclose(file_ptr);
  return 1;
}

// CONSTROI A MATRIZ DE MOVIMENTOS
// CIMA = 1       BAIXO = 2
// ESQUERDA = 4   DIREITA = 8
void verificarMovimentosPossiveis(tabuleiro_e *tabuleiro)
{
  int x, y, movimentosPossiveis;
  //FILE *file_ptr;
  //file_ptr = fopen("movimentos.txt", "w");
  for (x = 0; x < tabuleiro->tamanho.x; x++)
  {
    for (y = 0; y < tabuleiro->tamanho.y; y++)
    {
      movimentosPossiveis = 0;
      if (tabuleiro->tabuleiro_jogo[x][y] == 1)
      {
        if ((x > 1) && tabuleiro->tabuleiro_jogo[x - 1][y] == 1 && tabuleiro->tabuleiro_jogo[x - 2][y] == 0)
        {
          movimentosPossiveis += 1;
        }
        if ((x < (tabuleiro->tamanho.x - 2)) && tabuleiro->tabuleiro_jogo[x + 1][y] == 1 && tabuleiro->tabuleiro_jogo[x + 2][y] == 0)
        {
          movimentosPossiveis += 2;
        }

        if ((y > 1) && tabuleiro->tabuleiro_jogo[x][y - 1] == 1 && tabuleiro->tabuleiro_jogo[x][y - 2] == 0)
        {
          movimentosPossiveis += 4;
        }

        if ((y < (tabuleiro->tamanho.y - 2)) && tabuleiro->tabuleiro_jogo[x][y + 1] == 1 && tabuleiro->tabuleiro_jogo[x][y + 2] == 0)
        {
          movimentosPossiveis += 8;
        }
      }
      tabuleiro->movimentosPossiveis[x][y] = movimentosPossiveis;
      //fprintf(file_ptr, "%2d ", movimentosPossiveis);
    }
    //fprintf(file_ptr, "\n");
  }
  //fclose(file_ptr);
}


// AI MOVENDO PINOS
int ajudaMovimentos(tabuleiro_e *tabuleiro, int nMovimentos)
{
  int i, j, n, k, direcoesPossiveis[4], valorMovimentoPossivel, movimentoSelecionado, tamanhoTexto;
  char textoMovimento[TAM_MAX];
  srand(time(0));
  for (n = 0; n < nMovimentos; n++)
  {
    i = 0;
    while (i != 4)
    {
      direcoesPossiveis[i] = 0;
      i++;
    }
    movimentoSelecionado = 0;
    for (i = 0; i < tabuleiro->tamanho.x; i++)
    {
      for (j = 0; j < tabuleiro->tamanho.y; j++)
      {
        if (tabuleiro->movimentosPossiveis[i][j] != 0)
        {
          valorMovimentoPossivel = tabuleiro->movimentosPossiveis[i][j];

          for (k = 0; valorMovimentoPossivel > 0; k++)
          {
            direcoesPossiveis[k] = valorMovimentoPossivel % 2;
            valorMovimentoPossivel = valorMovimentoPossivel / 2;
          }

          while (direcoesPossiveis[movimentoSelecionado] != 1)
          {
            movimentoSelecionado = rand() % 4;
          }

          tabuleiro->pinoSelecionado.x = i;
          tabuleiro->pinoSelecionado.y = j;

          strcpy(textoMovimento, "O movimento ");
          strcat(textoMovimento, "\033[1;34m");

          switch (movimentoSelecionado)
          {
          case 0:
          {
            moverPraCima(tabuleiro);
            strcat(textoMovimento, "c ");
            break;
          }
          case 1:
          {
            moverPraBaixo(tabuleiro);
            strcat(textoMovimento, "b ");
            break;
          }
          case 2:
          {
            moverPraEsquerda(tabuleiro);
            strcat(textoMovimento, "e ");
            break;
          }
          case 3:
          {
            moverPraDireita(tabuleiro);
            strcat(textoMovimento, "d ");
            break;
          }
          }
          tamanhoTexto = strlen(textoMovimento);
          textoMovimento[tamanhoTexto] = coordNumeroParaLetra(i, 0);
          textoMovimento[tamanhoTexto + 1] = coordNumeroParaLetra(j, 0);
          textoMovimento[tamanhoTexto + 2] = 0;
          strcat(textoMovimento, "\033[0;31m");
          strcat(textoMovimento, " foi realizado!");

          i = tabuleiro->tamanho.x;
          j = tabuleiro->tamanho.y;
        }
      }
    }
    verificarMovimentosPossiveis(tabuleiro);
    imprimirUI(*tabuleiro, textoMovimento, "Aperte ENTER para continuar... ");
    getchar();
    if (verificaVitoria(*tabuleiro))
    {
      return 0;
    }
  }
  return 1;
}

// MOVIMENTOS CIMA, BAIXO, ESQUERDA E DIREITA
int moverPraCima(tabuleiro_e *tabuleiro)
{
  int x, y, movimentoPossivel;
  x = tabuleiro->pinoSelecionado.x;
  y = tabuleiro->pinoSelecionado.y;

  movimentoPossivel = tabuleiro->movimentosPossiveis[x][y];

  if ((x > 1) && (movimentoPossivel == 1 || movimentoPossivel == 3 || movimentoPossivel == 5 || movimentoPossivel == 7 || movimentoPossivel == 9 || movimentoPossivel == 11 || movimentoPossivel == 13 || movimentoPossivel == 15))
  {
    tabuleiro->tabuleiro_jogo[x][y] = 0;
    tabuleiro->tabuleiro_jogo[x - 1][y] = 0;
    tabuleiro->tabuleiro_jogo[x - 2][y] = 1;
    tabuleiro->pinos--;
    return 1;
  }
  else
  {
    return 0;
  }
}

int moverPraBaixo(tabuleiro_e *tabuleiro)
{
  int x, y, movimentoPossivel;
  x = tabuleiro->pinoSelecionado.x;
  y = tabuleiro->pinoSelecionado.y;

  movimentoPossivel = tabuleiro->movimentosPossiveis[x][y];

  if ((x < (tabuleiro->tamanho.x - 2)) && (movimentoPossivel == 2 || movimentoPossivel == 3 || movimentoPossivel == 6 || movimentoPossivel == 7 || movimentoPossivel == 10 || movimentoPossivel == 11 || movimentoPossivel == 14 || movimentoPossivel == 15))
  {
    tabuleiro->tabuleiro_jogo[x][y] = 0;
    tabuleiro->tabuleiro_jogo[x + 1][y] = 0;
    tabuleiro->tabuleiro_jogo[x + 2][y] = 1;
    tabuleiro->pinos--;
    return 1;
  }
  else
  {
    return 0;
  }
}

int moverPraEsquerda(tabuleiro_e *tabuleiro)
{
  int x, y, movimentoPossivel;
  x = tabuleiro->pinoSelecionado.x;
  y = tabuleiro->pinoSelecionado.y;

  movimentoPossivel = tabuleiro->movimentosPossiveis[x][y];

  if ((y > 1) && (movimentoPossivel == 4 || movimentoPossivel == 5 || movimentoPossivel == 6 || movimentoPossivel == 7 || movimentoPossivel == 12 || movimentoPossivel == 13 || movimentoPossivel == 14 || movimentoPossivel == 15))
  {
    tabuleiro->tabuleiro_jogo[x][y] = 0;
    tabuleiro->tabuleiro_jogo[x][y - 1] = 0;
    tabuleiro->tabuleiro_jogo[x][y - 2] = 1;
    tabuleiro->pinos--;
    return 1;
  }
  else
  {
    return 0;
  }
}

int moverPraDireita(tabuleiro_e *tabuleiro)
{
  int x, y, movimentoPossivel;
  x = tabuleiro->pinoSelecionado.x;
  y = tabuleiro->pinoSelecionado.y;

  movimentoPossivel = tabuleiro->movimentosPossiveis[x][y];

  if ((y < (tabuleiro->tamanho.y - 2)) && (movimentoPossivel == 8 || movimentoPossivel == 9 || movimentoPossivel == 10 || movimentoPossivel == 11 || movimentoPossivel == 12 || movimentoPossivel == 13 || movimentoPossivel == 14 || movimentoPossivel == 15))
  {
    tabuleiro->tabuleiro_jogo[x][y] = 0;
    tabuleiro->tabuleiro_jogo[x][y + 1] = 0;
    tabuleiro->tabuleiro_jogo[x][y + 2] = 1;
    tabuleiro->pinos--;
    return 1;
  }
  else
  {
    return 0;
  }
}

// ORGANIZA A UI PARA IMPRIMIR NA TELA
void imprimirUI(tabuleiro_e tabuleiro, char erro[200], char mensagem[200])
{
  limparTerminal();

  imprimirTabuleiro(tabuleiro);

  printf("\033[0;31m");
  printf("%s\n", erro);

  printf("\033[0m");
  printf("%s", mensagem);
  printf("\033[0;37m");
}

// IMPRIME O TABULEIRO
void imprimirTabuleiro(tabuleiro_e tabuleiro)
{
  int i, j;

  //CRIA AS POSICOES DE COLUNA
  printf("\033[1;36m#  ");
  for (i = 0; i < tabuleiro.tamanho.y; i++)
  {
    printf(" %c", coordNumeroParaLetra(i, 0));
  }
  printf("\t\033[0;33mResta%c %3d pino%c\n\n", 109 * (tabuleiro.pinos > 1), tabuleiro.pinos, 115 * (tabuleiro.pinos > 1));

  //CRIA AS POSICOES DE LINHA E IMPRIME OS VALORES DA MATRIZ
  for (i = 0; i < tabuleiro.tamanho.x; i++)
  {
    printf("\033[1;36m%c  ", coordNumeroParaLetra(i, 0));
    for (j = 0; j < tabuleiro.tamanho.y; j++)
    {
      printf("\033[0;36m");
      switch (tabuleiro.tabuleiro_jogo[i][j])
      {
      case -1:
      {
        if ((tabuleiro.inicializado == 0) && (tabuleiro.pinoSelecionado.x == i && tabuleiro.pinoSelecionado.y == j))
        {
          printf(" _");
        }
        else
        {
          printf("  ");
        }
        break;
      }
      case 1:
      {
        printf(" o");
        break;
      }
      case 0:
      {
        printf("\033[1;36m .\033[0;36m");
        break;
      }
      }
    }

    printf("\n");
  }
  printf("\n");
}

// LIMPA O TERMINAL DEPENDENDO DA VERSAO DO OS
void limparTerminal()
{
  //DETERMINA SE O SISTEMA EH UNIX OU WINDOWS
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
  system("clear");
#endif

#if defined(_WIN32) || defined(_WIN64)
  system("cls");
#endif
}

// TRANSFORMA LETRAS EM NUMERO E NUMERO EM LETRA
int coordNumeroParaLetra(int numero, char letra)
{
  if (numero == 0 && letra == 0)
  {
    return 65;
  }
  else if (letra == 0)
  {
    return numero + 65;
  }
  else
  {
    return letra - 65;
  }
}

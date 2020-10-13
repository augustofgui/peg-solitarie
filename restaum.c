/************************************************************/
/*  Resta Um, jogo feito em C                               */
/*  Augusto Ferreira Guilarducci                            */
/*  20.1.4012 - Turma 41                                    */
/************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define TAM_MAX 200
#define TAB_MAX 26
#define NUM_PRA_LETRA 65

#define ANSI_RESET "\x1b[0m" // desativa os efeitos anteriores
#define ANSI_BOLD "\x1b[1m"  // coloca o texto em negrito
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_WHITE "\x1b[37m"

// Macros para facilitar o uso
#define BOLD(string) ANSI_BOLD string ANSI_RESET
#define BLUE(string) ANSI_COLOR_BLUE string ANSI_RESET
#define RED(string) ANSI_COLOR_RED string ANSI_RESET
#define GREEN(string) ANSI_COLOR_GREEN string ANSI_RESET
#define CYAN(string) ANSI_COLOR_CYAN string ANSI_RESET
#define WHITE(string) ANSI_COLOR_WHITE string ANSI_RESET

// Caracteres uteis para tabelas
#define TAB_HOR "\u2501" // ━ (horizontal)
#define TAB_VER "\u2503" // ┃ (vertical)
#define TAB_TL "\u250F"  // ┏ (top-left)
#define TAB_BL "\u2517" // ┗ (bottom-left)
#define TAB_TR "\u2513" // ┓ (top-right)
#define TAB_BR "\u251B" // ┛ (bottom-right)

/*---------------------------------------------------------*/

/********** Structs ****************************************/
/*== tipo coordenada para salvar a posição na matriz x,y ==*/
typedef struct
{
  int x;
  int y;
} COORDENADA;

/*== tipo movimento para mostrar os pinos usados num pulo =*/
typedef struct
{
  COORDENADA pino_inicial;
  COORDENADA pino_meio;
  COORDENADA pino_destino;
} MOVIMENTO;

/*== tipo do jogo para salvar tudo que o jogo registra ====*/
typedef struct
{
  int num_pinos, foi_inicializado, tamanho_linhas, tamanho_colunas, eh_oh_fim, tamanho_lista_movimentos, num_movimentos_possiveis;
  int **tabuleiro_jogo;
  COORDENADA pino_selecionado;
  MOVIMENTO *lista_de_movimentos, *lista_de_movimentos_ia;
} RESTA_UM;

/*---------------------------------------------------------*/

/******** Funções ******************************************/
/*==== Inicio do jogo e Manutenção ========================*/
void inicializar_var_jogo(RESTA_UM *jogo);
void alocar_matriz(int ***matriz, int tam_linha, int tam_coluna);
void liberar_memoria_jogo(RESTA_UM *jogo);

/*==== Manipulação Tabuleiro ==============================*/
void ler_valores_tabela(RESTA_UM *jogo, FILE *ponteiro_arquivo);
void cria_tabuleiro_aleatorio(RESTA_UM *jogo);
int ler_tabuleiro_arquivo(RESTA_UM *jogo, char *nome_arquivo);
int salvar_tabuleiro(RESTA_UM *jogo, char *nome_arquivo);

/*==== Verificação de Derrota/Vitoria =====================*/
int tabuleiro_valido(RESTA_UM *jogo);
/*---- Inteligência Artificial ----------------------------*/
int decidir_movimento_ia(RESTA_UM *jogo, int limite_index_ia, int index_ia, int salvar_movimento);

/*==== Movimento ==========================================*/
MOVIMENTO cria_movimento(int posicao_x, int posicao_y, int direcao_x, int direcao_y);
void reinicia_lista_movimentos(RESTA_UM *jogo);
void faz_movimento(RESTA_UM *jogo, MOVIMENTO movimento);
void desfaz_movimento(RESTA_UM *jogo, MOVIMENTO movimento);
int movimento_valido(RESTA_UM *jogo, MOVIMENTO movimento_teste);
int cria_lista_movimentos(RESTA_UM *jogo);
int gerenciar_movimento(RESTA_UM *jogo, char *comando_digitado);

/*==== Imprimir na Tela ===================================*/
void imprimir_lista_movimentos(RESTA_UM *jogo);
void imprimir_tabuleiro_jogo(RESTA_UM *jogo);
void imprimir_msg_de_fim(char *tipo_de_fim);
void imprimir_movimentos_ajuda(RESTA_UM *jogo);

/*---------------------------------------------------------*/

/***** Main e a Definição de Funções ***********************/
int main(int argc, char *argv[])
{
  /*==== Criação do Tabuleiro e Validação do jogo =========*/
  /*---- Declaração de Variaveis --------------------------*/
  RESTA_UM jogo;
  char nome_arquivo[TAM_MAX], comando_digitado[TAM_MAX];

  inicializar_var_jogo(&jogo); // Inicializa as variaveis do tabuleiro Resta Um

  /*---- Verifica se foi digitado um Argumento ------------*/
  if (argc >= 2)
  {
    strcpy(nome_arquivo, argv[1]);
    if (!ler_tabuleiro_arquivo(&jogo, nome_arquivo))
    {
      printf(RED("\nErro: Esse arquivo não existe...\n")); // Não existe esse arquivo
    }
  }
  else
  {
    /*---- Cria um tabuleiro de maneira aleatoria ---------*/
    srand(time(0));
    jogo.tamanho_linhas = rand() % 26 + 1;
    jogo.tamanho_colunas = rand() % 26 + 1;

    alocar_matriz(&jogo.tabuleiro_jogo, jogo.tamanho_linhas, jogo.tamanho_colunas);

    ler_valores_tabela(&jogo, NULL); // Envia um nome de aquivo nulo
  }

  /*-- Confirma que o jogo foi inicializado corretamente --*/
  if (!jogo.foi_inicializado)
  {
    return 1;
  }

  if (!tabuleiro_valido(&jogo)) // Verifica se o tabuleiro é valido
  {
    // Derrota instantânea caso o tabuleiro nao seja valido
    imprimir_tabuleiro_jogo(&jogo);
    imprimir_msg_de_fim("derrota");
    jogo.eh_oh_fim = 1;
  }

  else if (!cria_lista_movimentos(&jogo)) // Verifica se há movimentos possíveis
  {
    // Derrota instantânea caso não haja movimentos
    imprimir_tabuleiro_jogo(&jogo);
    jogo.eh_oh_fim = 1;
    imprimir_msg_de_fim("derrota");
  }

  /*-- Cria lista de movimentos da inteligência artifical -*/
  jogo.lista_de_movimentos_ia = malloc(jogo.num_pinos * sizeof(MOVIMENTO));

  /*==== Jogo =============================================*/
  /*---- Inicio do loop de jogo ---------------------------*/
  while (!jogo.eh_oh_fim) // sai caso seja o fim do jogo - variável do tipo RESTA_UM
  {
    /*-- A Cada Novo Turno --------------------------------*/
    imprimir_tabuleiro_jogo(&jogo);

    if (jogo.num_pinos == 1) // Verifica se o jogador ganhou
    {
      imprimir_msg_de_fim("vitoria");
      jogo.eh_oh_fim = 1;
    }

    /*---- Ve se o jogador ainda pode ganhar com 5  -----*/
    /*---- movimentos no futuro, usando backtracking ----*/
    else if (decidir_movimento_ia(&jogo, 5, 0, 0))
    {

      /*---- Pede um comando pro jogador e formata ------*/
      printf("\nDigite seu comando: ");
      fgets(comando_digitado, TAM_MAX - 1, stdin);
      comando_digitado[strlen(comando_digitado) - 1] = '\0'; // Retira o ENTER (\n)

      /*---- Jogador digitou sair -----------------------*/
      if (strcmp(comando_digitado, "sair") == 0)
      {
        printf(RED("Jogo fechado, sem salvar...\n"));
        jogo.eh_oh_fim = 1;
      }

      /*---- Jogador digitou salvar ---------------------*/
      else if (strncmp(comando_digitado, "salvar", 6) == 0)
      {
        // Verifica se o jogador digitou um NOME para o arquivo
        if (comando_digitado[6] == '\0' || (comando_digitado[6] == ' ' && comando_digitado[7] == '\0'))
        {
          printf(RED("Erro: Nao foi colocado o nome do arquivo!!\n"));
        }

        // Verifica se o jogador digitou a EXTENSÂO do arquivo
        else if (strcmp(&comando_digitado[strlen(comando_digitado) - 4], ".txt") != 0)
        {
          printf(RED("Erro: Nao foi colocado a extensao do arquivo!!\n"));
        }

        // Salva o tabuleiro no arquivo .txt
        else if (salvar_tabuleiro(&jogo, &comando_digitado[7])) /* <--- Envio o ponteiro da onde começa o nome do arquivo */
        {
          printf(GREEN("Tabuleiro salvo no arquivo "));
          printf(BLUE("%s"), &comando_digitado[7]);
          printf(GREEN(" com sucesso!!"));
        }
      }

      /*---- Jogador digitou ajuda ----------------------*/
      else if (strncmp(comando_digitado, "ajuda", 5) == 0)
      {
        // Verifica se o jogador digitou uma N para a ajuda
        if (comando_digitado[6] == '\0' || (comando_digitado[6] == ' ' && comando_digitado[7] == '\0'))
        {
        }

        /* Realiza a ajuda usando a mesma função que verifica a derrota, */
        /* a diferença é que nesse caso ela registra os movimentos.      */
        // Utilizando o backtracking, faz n movimentos.
        else if (decidir_movimento_ia(&jogo, atoi(&comando_digitado[6]), 0, 1))
        {
          imprimir_movimentos_ajuda(&jogo);
          printf(GREEN(BOLD("Ajuda finalizada!\n")));
        }

        // Caso a ajuda tenha levado a derrota
        else
        {
          imprimir_movimentos_ajuda(&jogo);
          imprimir_tabuleiro_jogo(&jogo);
          imprimir_msg_de_fim("derrota");
          jogo.eh_oh_fim = 1;
        }
      }

      /*---- Jogador digitou movimento ou um comando desconhecido ---*/
      else
      {
        // Verifica se o jogador digitou algum movimento
        switch (comando_digitado[0])
        {
        case 'c':
        case 'b':
        case 'e':
        case 'd':
        {
          // Verifica se o comando foi digitado no formato: m CD
          if (comando_digitado[1] == ' ' && comando_digitado[4] == '\0'                                             /**/
              && (comando_digitado[2] >= 'A' && comando_digitado[2] <= (jogo.tamanho_linhas + NUM_PRA_LETRA - 1))   /**/
              && (comando_digitado[3] >= 'A' && comando_digitado[3] <= (jogo.tamanho_colunas + NUM_PRA_LETRA - 1))) /**/
          {
            // Envia o movimento para ser alisado e realizado
            if (gerenciar_movimento(&jogo, comando_digitado))
            {
              printf(GREEN("Movimento %c %c%c realizado com sucesso...\n"), comando_digitado[0], comando_digitado[2], comando_digitado[3]);
            }
            // Movimento inválido
            else
            {
              printf(RED("Erro: Movimento invalido!!\n"));
            }
          }
          else
          { // Posição inexistente do tabuleiro
            printf(RED("Erro: Posicao inexistente!!\n"));
          }
          break;
        }

        // Jogador digitou um comando desconhecido
        default:
        { //
          printf(RED("Erro: Comando nao reconhecido!!\n"));
        }
        }
      }
    }
    else
    {
      // Derrota no caso a não houver como ganhar mais
      imprimir_msg_de_fim("derrota");
      jogo.eh_oh_fim = 1;
    }
  }

  /*==== Finaliza programa e libera a memoria =============*/
  liberar_memoria_jogo(&jogo);
  printf("\n");
  return 0;
}

void imprimir_movimentos_ajuda(RESTA_UM *jogo)
{
  int i;
  i = 0;
  MOVIMENTO fim_lista = cria_movimento(0, 0, 0, 0);
  char direcao;
  while (memcmp(&jogo->lista_de_movimentos_ia[i], &fim_lista, sizeof(MOVIMENTO)))
  {
    faz_movimento(jogo, jogo->lista_de_movimentos_ia[i]);
    imprimir_tabuleiro_jogo(jogo);
    printf(BLUE("O Movimento "));

    if (jogo->lista_de_movimentos_ia[i].pino_inicial.x > jogo->lista_de_movimentos_ia[i].pino_meio.x)
    {
      direcao = 'c';
    }
    else if (jogo->lista_de_movimentos_ia[i].pino_inicial.x < jogo->lista_de_movimentos_ia[i].pino_meio.x)
    {
      direcao = 'b';
    }
    else if (jogo->lista_de_movimentos_ia[i].pino_inicial.y > jogo->lista_de_movimentos_ia[i].pino_meio.y)
    {
      direcao = 'e';
    }
    else
    {
      direcao = 'd';
    }

    printf(CYAN("%c %c"), direcao, jogo->lista_de_movimentos_ia[i].pino_inicial.x + NUM_PRA_LETRA);
    printf(CYAN("%c"), jogo->lista_de_movimentos_ia[i].pino_inicial.y + NUM_PRA_LETRA);
    printf(BLUE(" foi executado com sucesso..."));

    printf("\n");
    i++;
  }
}
void imprimir_lista_movimentos(RESTA_UM *jogo)
{
  int i;

  for (i = 0; i < jogo->num_movimentos_possiveis; i++)
  {
    printf("[%d]: PI: %d, %d  PM: %d, %d  PD: %d, %d\n", i, jogo->lista_de_movimentos[i].pino_inicial.x, jogo->lista_de_movimentos[i].pino_inicial.y, jogo->lista_de_movimentos[i].pino_meio.x, jogo->lista_de_movimentos[i].pino_meio.y, jogo->lista_de_movimentos[i].pino_destino.x, jogo->lista_de_movimentos[i].pino_destino.y);
  }
}

void inicializar_var_jogo(RESTA_UM *jogo)
{
  jogo->num_pinos = 0;
  jogo->foi_inicializado = 0;
  jogo->eh_oh_fim = 0;
  jogo->tamanho_lista_movimentos = 0;
}

void alocar_matriz(int ***matriz, int tam_linha, int tam_coluna)
{
  int i;
  (*matriz) = malloc(tam_linha * sizeof(int *));
  for (i = 0; i < tam_linha; i++)
  {
    (*matriz)[i] = malloc(tam_coluna * sizeof(int));
  }
}

void ler_valores_tabela(RESTA_UM *jogo, FILE *ponteiro_arquivo)
{
  int i, j;
  srand(time(0));

  for (i = 0; i < jogo->tamanho_linhas; i++)
  {
    for (j = 0; j < jogo->tamanho_colunas; j++)
    {
      if (ponteiro_arquivo != NULL)
      {
        fscanf(ponteiro_arquivo, "%d", &jogo->tabuleiro_jogo[i][j]);
      }
      else
      {
        jogo->tabuleiro_jogo[i][j] = (rand() % 3) - 1;
      }
      jogo->num_pinos += (jogo->tabuleiro_jogo[i][j] == 1);
    }
  }
  jogo->foi_inicializado = 1;
}

int ler_tabuleiro_arquivo(RESTA_UM *jogo, char *nome_arquivo)
{
  FILE *ponteiro_arquivo;
  if ((ponteiro_arquivo = fopen(nome_arquivo, "r")) != NULL)
  {

    fscanf(ponteiro_arquivo, "%d %d", &jogo->tamanho_linhas, &jogo->tamanho_colunas);

    alocar_matriz(&jogo->tabuleiro_jogo, jogo->tamanho_linhas, jogo->tamanho_colunas);

    ler_valores_tabela(jogo, ponteiro_arquivo);

    fclose(ponteiro_arquivo);
    return 1;
  }
  else
  {
    return 0;
  }
}

void liberar_memoria_jogo(RESTA_UM *jogo)
{
  int i;

  for (i = 0; i < jogo->tamanho_linhas; i++)
  {
    free(jogo->tabuleiro_jogo[i]);
  }
  free(jogo->tabuleiro_jogo);
  free(jogo->lista_de_movimentos);
  free(jogo->lista_de_movimentos_ia);
}

int salvar_tabuleiro(RESTA_UM *jogo, char *nome_arquivo)
{

  int i, j;
  FILE *ponteiro_arquivo;

  ponteiro_arquivo = fopen(nome_arquivo, "w");

  fprintf(ponteiro_arquivo, "%2d %2d\n\n", jogo->tamanho_linhas, jogo->tamanho_colunas);

  for (i = 0; i < jogo->tamanho_linhas; i++)
  {
    for (j = 0; j < jogo->tamanho_colunas; j++)
    {
      fprintf(ponteiro_arquivo, "%2d ", jogo->tabuleiro_jogo[i][j]);
    }
    fprintf(ponteiro_arquivo, "\n");
  }

  fclose(ponteiro_arquivo);
  return 1;
}

int tabuleiro_valido(RESTA_UM *jogo)
{
  int i, j, pino_meio_valido, tamanho_lista_movimentos, movimento_possiveis_atual;

  tamanho_lista_movimentos = 0;

  for (i = 0; i < jogo->tamanho_linhas; i++)
  {
    for (j = 0; j < jogo->tamanho_colunas; j++)
    {
      pino_meio_valido = 0;
      movimento_possiveis_atual = 0;
      if (jogo->tabuleiro_jogo[i][j] != -1)
      {
        if ((i - 2 >= 0) && jogo->tabuleiro_jogo[i - 1][j] != -1 && jogo->tabuleiro_jogo[i - 2][j] != -1)
        {
          tamanho_lista_movimentos++;
          movimento_possiveis_atual++;
        }

        if ((i + 2 < jogo->tamanho_linhas) && jogo->tabuleiro_jogo[i + 1][j] != -1 && jogo->tabuleiro_jogo[i + 2][j] != -1)
        {
          tamanho_lista_movimentos++;
          movimento_possiveis_atual++;
        }

        if ((j - 2 >= 0) && jogo->tabuleiro_jogo[i][j - 1] != -1 && jogo->tabuleiro_jogo[i][j - 2] != -1)
        {
          tamanho_lista_movimentos++;
          movimento_possiveis_atual++;
        }

        if ((j + 2 < jogo->tamanho_colunas) && jogo->tabuleiro_jogo[i][j + 1] != -1 && jogo->tabuleiro_jogo[i][j + 2] != -1)
        {
          tamanho_lista_movimentos++;
          movimento_possiveis_atual++;
        }

        if ((j > 0) && jogo->tabuleiro_jogo[i][j] == 1 && (j < jogo->tamanho_colunas - 1) && (jogo->tabuleiro_jogo[i][j - 1] != -1 && jogo->tabuleiro_jogo[i][j + 1] != -1))
        {
          pino_meio_valido++;
        }

        if ((i > 0) && jogo->tabuleiro_jogo[i][j] == 1 && (i < jogo->tamanho_linhas - 1) && (jogo->tabuleiro_jogo[i - 1][j] != -1 && jogo->tabuleiro_jogo[i + 1][j] != -1))
        {
          pino_meio_valido++;
        }

        if (jogo->tabuleiro_jogo[i][j] == 1 && movimento_possiveis_atual == 0 && pino_meio_valido == 0)
        {

          return 0;
        }
      }
    }
  }

  jogo->lista_de_movimentos = malloc(tamanho_lista_movimentos * sizeof(MOVIMENTO));
  jogo->tamanho_lista_movimentos = tamanho_lista_movimentos;

  if (tamanho_lista_movimentos == 0)
  {
    return 0;
  }

  return 1;
}

void imprimir_tabuleiro_jogo(RESTA_UM *jogo)
{
  int i, j;

  printf("\n\n\t");
  for (j = 0; j < jogo->tamanho_colunas - 5; j++)
  {
    printf(WHITE("  "));
  }
  printf(WHITE("Resta%c %3d pino%c\n"), 109 * (jogo->num_pinos > 1), jogo->num_pinos, 115 * (jogo->num_pinos > 1));
  printf(WHITE("\t" TAB_TL TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR));

  for (j = 0; j < jogo->tamanho_colunas; j++)
  {
    printf(WHITE(TAB_HOR TAB_HOR));
  }
  printf(WHITE(TAB_TR "\n\t" TAB_VER "    "));

  for (j = 0; j < jogo->tamanho_colunas; j++)
  {
    printf(WHITE(BOLD(" %c")), j + NUM_PRA_LETRA);
  }
  printf(WHITE("  " TAB_VER "\n\t" TAB_VER "    "));

  for (j = 0; j < jogo->tamanho_colunas; j++)
  {
    printf("  ");
  }
  printf("  " TAB_VER "\n");

  for (i = 0; i < jogo->tamanho_linhas; i++)
  {

    printf(WHITE(BOLD("\t" TAB_VER " %c  ")), i + NUM_PRA_LETRA);
    for (j = 0; j < jogo->tamanho_colunas; j++)
    {
      switch (jogo->tabuleiro_jogo[i][j])
      {
      case -1:
      {
        printf("  ");
        break;
      }
      case 1:
      {
        printf(WHITE(BOLD(" o")));
        break;
      }
      case 0:
      {
        printf(WHITE(BOLD(" .")));
        break;
      }
      }
    }
    printf(WHITE("  " TAB_VER "\n"));
  }

  printf(WHITE("\t" TAB_BL TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR));

  for (j = 0; j < jogo->tamanho_colunas; j++)
  {
    printf(WHITE(TAB_HOR TAB_HOR));
  }
  printf(WHITE(TAB_BR "\n"));
}

MOVIMENTO cria_movimento(int posicao_x, int posicao_y, int direcao_x, int direcao_y)
{
  MOVIMENTO movimento_criado;

  movimento_criado.pino_inicial.x = posicao_x;
  movimento_criado.pino_inicial.y = posicao_y;

  movimento_criado.pino_meio.x = posicao_x + direcao_x;
  movimento_criado.pino_meio.y = posicao_y + direcao_y;

  movimento_criado.pino_destino.x = posicao_x + (2 * direcao_x);
  movimento_criado.pino_destino.y = posicao_y + (2 * direcao_y);

  return movimento_criado;
}

void reinicia_lista_movimentos(RESTA_UM *jogo)
{
  int i;

  for (i = 0; i < jogo->tamanho_lista_movimentos; i++)
  {
    jogo->lista_de_movimentos[i] = cria_movimento(0, 0, 0, 0);
  }
}

int cria_lista_movimentos(RESTA_UM *jogo)
{
  int i, j, total_movimentos_possiveis, contador_lista_movimentos;
  FILE *ponteiro_arquivo;
  ponteiro_arquivo = fopen("movimentos.txt", "w");
  total_movimentos_possiveis = 0;
  contador_lista_movimentos = 0;

  reinicia_lista_movimentos(jogo);

  for (i = 0; i < jogo->tamanho_linhas; i++)
  {
    for (j = 0; j < jogo->tamanho_colunas; j++)
    {
      if (jogo->tabuleiro_jogo[i][j] != -1)
      {
        if ((i - 2 >= 0) && jogo->tabuleiro_jogo[i - 1][j] != -1 && jogo->tabuleiro_jogo[i - 2][j] != -1)
        {
          total_movimentos_possiveis++;
          jogo->lista_de_movimentos[contador_lista_movimentos] = cria_movimento(i, j, -1, 0);
          fprintf(ponteiro_arquivo, "[%d]: PI: %d, %d  PM: %d, %d  PD: %d, %d\n", contador_lista_movimentos, jogo->lista_de_movimentos[contador_lista_movimentos].pino_inicial.x, jogo->lista_de_movimentos[contador_lista_movimentos].pino_inicial.y, jogo->lista_de_movimentos[contador_lista_movimentos].pino_meio.x, jogo->lista_de_movimentos[contador_lista_movimentos].pino_meio.y, jogo->lista_de_movimentos[contador_lista_movimentos].pino_destino.x, jogo->lista_de_movimentos[contador_lista_movimentos].pino_destino.y);
          contador_lista_movimentos++;
        }

        if ((i + 2 < jogo->tamanho_linhas) && jogo->tabuleiro_jogo[i + 1][j] != -1 && jogo->tabuleiro_jogo[i + 2][j] != -1)
        {
          total_movimentos_possiveis++;
          jogo->lista_de_movimentos[contador_lista_movimentos] = cria_movimento(i, j, 1, 0);
          fprintf(ponteiro_arquivo, "[%d]: PI: %d, %d  PM: %d, %d  PD: %d, %d\n", contador_lista_movimentos, jogo->lista_de_movimentos[contador_lista_movimentos].pino_inicial.x, jogo->lista_de_movimentos[contador_lista_movimentos].pino_inicial.y, jogo->lista_de_movimentos[contador_lista_movimentos].pino_meio.x, jogo->lista_de_movimentos[contador_lista_movimentos].pino_meio.y, jogo->lista_de_movimentos[contador_lista_movimentos].pino_destino.x, jogo->lista_de_movimentos[contador_lista_movimentos].pino_destino.y);
          contador_lista_movimentos++;
        }

        if ((j - 2 >= 0) && jogo->tabuleiro_jogo[i][j - 1] != -1 && jogo->tabuleiro_jogo[i][j - 2] != -1)
        {
          total_movimentos_possiveis++;
          jogo->lista_de_movimentos[contador_lista_movimentos] = cria_movimento(i, j, 0, -1);
          fprintf(ponteiro_arquivo, "[%d]: PI: %d, %d  PM: %d, %d  PD: %d, %d\n", contador_lista_movimentos, jogo->lista_de_movimentos[contador_lista_movimentos].pino_inicial.x, jogo->lista_de_movimentos[contador_lista_movimentos].pino_inicial.y, jogo->lista_de_movimentos[contador_lista_movimentos].pino_meio.x, jogo->lista_de_movimentos[contador_lista_movimentos].pino_meio.y, jogo->lista_de_movimentos[contador_lista_movimentos].pino_destino.x, jogo->lista_de_movimentos[contador_lista_movimentos].pino_destino.y);
          contador_lista_movimentos++;
        }

        if ((j + 2 < jogo->tamanho_colunas) && jogo->tabuleiro_jogo[i][j + 1] != -1 && jogo->tabuleiro_jogo[i][j + 2] != -1)
        {
          total_movimentos_possiveis++;
          jogo->lista_de_movimentos[contador_lista_movimentos] = cria_movimento(i, j, 0, 1);
          fprintf(ponteiro_arquivo, "[%d]: PI: %d, %d  PM: %d, %d  PD: %d, %d\n", contador_lista_movimentos, jogo->lista_de_movimentos[contador_lista_movimentos].pino_inicial.x, jogo->lista_de_movimentos[contador_lista_movimentos].pino_inicial.y, jogo->lista_de_movimentos[contador_lista_movimentos].pino_meio.x, jogo->lista_de_movimentos[contador_lista_movimentos].pino_meio.y, jogo->lista_de_movimentos[contador_lista_movimentos].pino_destino.x, jogo->lista_de_movimentos[contador_lista_movimentos].pino_destino.y);
          contador_lista_movimentos++;
        }
      }
    }
  }

  jogo->num_movimentos_possiveis = contador_lista_movimentos;
  fclose(ponteiro_arquivo);

  if (total_movimentos_possiveis > 0)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

int gerenciar_movimento(RESTA_UM *jogo, char *comando_digitado)
{
  int i, direcao_x, direcao_y, temp_x, temp_y;
  MOVIMENTO movimento_atual;

  temp_x = (int)comando_digitado[2] - NUM_PRA_LETRA;
  temp_y = (int)comando_digitado[3] - NUM_PRA_LETRA;

  switch (comando_digitado[0])
  {
  case 'c':
  {
    direcao_x = -1;
    direcao_y = 0;

    break;
  }
  case 'b':
  {
    direcao_x = 1;
    direcao_y = 0;

    break;
  }
  case 'e':
  {
    direcao_x = 0;
    direcao_y = -1;

    break;
  }
  case 'd':
  {
    direcao_x = 0;
    direcao_y = 1;

    break;
  }
  default:
  {
  }
  }

  movimento_atual = cria_movimento(temp_x, temp_y, direcao_x, direcao_y);

  for (i = 0; i < jogo->tamanho_lista_movimentos; i++)
  {
    if (memcmp(&jogo->lista_de_movimentos[i], &movimento_atual, sizeof(MOVIMENTO)) == 0)
    {
      if (movimento_valido(jogo, movimento_atual))
      {
        faz_movimento(jogo, movimento_atual);
        return 1;
      }
    }
  }

  return 0;
}

void faz_movimento(RESTA_UM *jogo, MOVIMENTO movimento)
{
  jogo->tabuleiro_jogo[movimento.pino_inicial.x][movimento.pino_inicial.y] = 0;
  jogo->tabuleiro_jogo[movimento.pino_meio.x][movimento.pino_meio.y] = 0;
  jogo->tabuleiro_jogo[movimento.pino_destino.x][movimento.pino_destino.y] = 1;
  jogo->num_pinos--;
}

void desfaz_movimento(RESTA_UM *jogo, MOVIMENTO movimento)
{
  jogo->tabuleiro_jogo[movimento.pino_inicial.x][movimento.pino_inicial.y] = 1;
  jogo->tabuleiro_jogo[movimento.pino_meio.x][movimento.pino_meio.y] = 1;
  jogo->tabuleiro_jogo[movimento.pino_destino.x][movimento.pino_destino.y] = 0;
  jogo->num_pinos++;
}

int movimento_valido(RESTA_UM *jogo, MOVIMENTO movimento_teste)
{
  int inicial_x, inicial_y, meio_x, meio_y, destino_x, destino_y;

  inicial_x = movimento_teste.pino_inicial.x;
  inicial_y = movimento_teste.pino_inicial.y;

  meio_x = movimento_teste.pino_meio.x;
  meio_y = movimento_teste.pino_meio.y;

  destino_x = movimento_teste.pino_destino.x;
  destino_y = movimento_teste.pino_destino.y;

  if (jogo->tabuleiro_jogo[inicial_x][inicial_y] == 1 && jogo->tabuleiro_jogo[meio_x][meio_y] == 1 && jogo->tabuleiro_jogo[destino_x][destino_y] == 0)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

int decidir_movimento_ia(RESTA_UM *jogo, int limite_index_ia, int index_ia, int salvar_movimento)
{
  int i;

  if (jogo->num_pinos == 1 || index_ia == limite_index_ia)
  {
    jogo->lista_de_movimentos_ia[index_ia] = cria_movimento(0, 0, 0, 0);
    return 1;
  }
  else
  {
    for (i = 0; i < jogo->num_movimentos_possiveis; i++)
    {
      if (movimento_valido(jogo, jogo->lista_de_movimentos[i]))
      {
        faz_movimento(jogo, jogo->lista_de_movimentos[i]);

        if (decidir_movimento_ia(jogo, limite_index_ia, index_ia + 1, salvar_movimento))
        {
          jogo->lista_de_movimentos_ia[index_ia] = jogo->lista_de_movimentos[i];
          desfaz_movimento(jogo, jogo->lista_de_movimentos[i]);
          return 1;
        }

        desfaz_movimento(jogo, jogo->lista_de_movimentos[i]);
      }
    }
    return 0;
  }
}

void imprimir_msg_de_fim(char *tipo_de_fim)
{
  if (strcmp(tipo_de_fim, "derrota") == 0)
  {
    printf(RED(BOLD("\tDERROTA!!\n")));
    printf(RED("Não há mais como ganhar parceiro, desista... :c\n"));
  }
  else if (strcmp(tipo_de_fim, "vitoria") == 0)
  {
    printf(GREEN(BOLD("\tVITORIA!!\n")));
    printf(GREEN("Voce ganhou!! Parabens amigo :D\n"));
  }
}

/***********************************************************/
/*  Resta Um, jogo feito em C                              */
/*  Augusto Ferreira Guilarducci                           */
/*  20.1.4012 - Turma 41                                   */
/***********************************************************/

/*==== Includes  ==========================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/*==== Defines  ===========================================*/
#define TAM_MAX 200      // Tamanho maximo de Texto
#define NUM_PRA_LETRA 65 // Numero ASCII de A, para transformar numero em letra e vice-versa

// Cores
#define ANSI_RESET "\x1b[0m" // desativa os efeitos anteriores
#define ANSI_BOLD "\x1b[1m"  // coloca o texto em negrito
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_WHITE "\x1b[37m"

// Macros para facilitar o uso das cores
#define BOLD(string) ANSI_BOLD string ANSI_RESET
#define BLUE(string) ANSI_COLOR_BLUE string ANSI_RESET
#define RED(string) ANSI_COLOR_RED string ANSI_RESET
#define GREEN(string) ANSI_COLOR_GREEN string ANSI_RESET
#define CYAN(string) ANSI_COLOR_CYAN string ANSI_RESET
#define WHITE(string) ANSI_COLOR_WHITE string ANSI_RESET

// Caracteres para tabelas
#define TAB_HOR "\u2501" // ━ (horizontal)
#define TAB_VER "\u2503" // ┃ (vertical)
#define TAB_TL "\u250F"  // ┏ (top-left)
#define TAB_BL "\u2517"  // ┗ (bottom-left)
#define TAB_TR "\u2513"  // ┓ (top-right)
#define TAB_BR "\u251B"  // ┛ (bottom-right)

/*---------------------------------------------------------*/

/********** Structs ****************************************/
/*== Tipo coordenada para salvar a posição na matriz x,y ==*/
typedef struct
{
  int x;
  int y;
} COORDENADA;

/*== Tipo movimento para mostrar os pinos usados num pulo =*/
typedef struct
{
  COORDENADA pino_inicial;
  COORDENADA pino_meio;
  COORDENADA pino_destino;
} MOVIMENTO;

/*== Tipo do jogo para salvar tudo que o jogo registra ====*/
typedef struct
{
  int num_pinos, foi_inicializado, tamanho_linhas, tamanho_colunas, eh_oh_fim, tamanho_lista_movimentos;
  int **tabuleiro_jogo;
  MOVIMENTO *lista_de_movimentos, *lista_de_movimentos_ia;
} RESTA_UM;

/*---------------------------------------------------------*/

/******** Funções ******************************************/
/*==== Inicio do jogo e Manutenção ========================*/
void alocar_matriz(int ***matriz, int tam_linha, int tam_coluna);
void inicializar_var_jogo(RESTA_UM *jogo);
void liberar_memoria_jogo(RESTA_UM *jogo);

/*==== Manipulação Tabuleiro ==============================*/
void ler_valores_tabela(RESTA_UM *jogo, FILE *ponteiro_arquivo);
int ler_tabuleiro_arquivo(RESTA_UM *jogo, char *nome_arquivo);
int salvar_tabuleiro(RESTA_UM *jogo, char *nome_arquivo);
void criar_tabuleiro_aleatorio(RESTA_UM *jogo, int tipo_tabuleiro);

/*==== Verificação de Derrota/Vitoria =====================*/
int tabuleiro_valido(RESTA_UM *jogo);

/*---- Inteligência Artificial ----------------------------*/
int decidir_movimento_ia(RESTA_UM *jogo, int limite_index_ia, int index_ia, int salvar_movimento);

/*==== Movimento ==========================================*/
MOVIMENTO cria_movimento(int posicao_x, int posicao_y, int direcao_x, int direcao_y);
void faz_movimento(RESTA_UM *jogo, MOVIMENTO movimento);
void desfaz_movimento(RESTA_UM *jogo, MOVIMENTO movimento);
int movimento_valido(RESTA_UM *jogo, MOVIMENTO movimento_teste);
int cria_lista_movimentos(RESTA_UM *jogo);
int gerenciar_movimento(RESTA_UM *jogo, char *comando_digitado);

/*==== Imprimir na Tela ===================================*/
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
  int erro;
  char nome_arquivo[TAM_MAX], comando_digitado[TAM_MAX];
  erro = 0;
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
    int tipo_tabuleiro_aleatorio;

    srand(time(0));

    tipo_tabuleiro_aleatorio = rand() % 5;

    switch (tipo_tabuleiro_aleatorio)
    {
    case 0:
    case 1:
    {

      jogo.tamanho_linhas = 7;
      jogo.tamanho_colunas = 7;

      break;
    }

    case 2:
    case 3:
    {

      jogo.tamanho_linhas = 9;
      jogo.tamanho_colunas = 9;

      break;
    }

    case 4:
    {

      jogo.tamanho_linhas = 8;
      jogo.tamanho_colunas = 8;

      break;
    }
    }

    alocar_matriz(&jogo.tabuleiro_jogo, jogo.tamanho_linhas, jogo.tamanho_colunas);

    criar_tabuleiro_aleatorio(&jogo, tipo_tabuleiro_aleatorio);
  }

  /*-- Confirma que o jogo foi inicializado corretamente --*/
  if (!jogo.foi_inicializado)
  {
    return 0;
  }

  if (!tabuleiro_valido(&jogo)) // Verifica se o tabuleiro é valido
  {
    // Derrota instantânea caso o tabuleiro nao seja valido
    imprimir_tabuleiro_jogo(&jogo);
    imprimir_msg_de_fim("derrota");
    jogo.eh_oh_fim = 1;
    return 0;
  }

  else if (!cria_lista_movimentos(&jogo)) // Verifica se há movimentos possíveis
  {
    // Derrota instantânea caso não haja movimentos
    imprimir_tabuleiro_jogo(&jogo);
    jogo.eh_oh_fim = 1;
    imprimir_msg_de_fim("derrota");
    return 0;
  }

  /*-- Cria lista de movimentos da inteligência artifical -*/
  jogo.lista_de_movimentos_ia = malloc(jogo.num_pinos * sizeof(MOVIMENTO));

  /*-- Imprime mensagem de boas vindas --------------------*/
  printf(BLUE(BOLD("\n\tBem vindo ao meu Resta Um\n\t por Augusto Guilarducci\n")));
  printf(RED(BOLD("\nAviso: ")));
  printf(RED("um comando de ajuda muito grande pode\ndemorar alguns segundos para ser executado..."));

  /*==== Jogo =============================================*/
  /*---- Inicio do loop de jogo ---------------------------*/
  while (!jogo.eh_oh_fim) // sai caso seja o fim do jogo - variável do tipo RESTA_UM
  {
    /*-- A Cada Novo Turno --------------------------------*/
    if (erro == 0)
    {
      imprimir_tabuleiro_jogo(&jogo);
    }

    erro = 0;

    if (jogo.num_pinos == 1) // Verifica se o jogador ganhou
    {
      imprimir_msg_de_fim("vitoria");
      jogo.eh_oh_fim = 1;
    }

    /*---- Ve se o jogador ainda pode ganhar com 10  -----*/
    /*---- movimentos no futuro, usando backtracking ----*/
    else if (decidir_movimento_ia(&jogo, 10, 0, 0))
    {

      /*---- Pede um comando pro jogador e formata ------*/
      printf(WHITE("\nDigite seu comando: "));
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
          erro = 1;
          printf(RED("Erro: Nao foi colocado o nome do arquivo!!\n"));
        }

        // Verifica se o jogador digitou a EXTENSÂO do arquivo
        else if (strcmp(&comando_digitado[strlen(comando_digitado) - 4], ".txt") != 0)
        {
          erro = 1;
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
          erro = 1;
          printf(RED("Erro: Nao foi colocado o numero de ajudas!!\n"));
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
              erro = 1;
              printf(RED("Erro: Movimento invalido!!\n"));
            }
          }
          else
          { // Posição inexistente do tabuleiro
            erro = 1;
            printf(RED("Erro: Posicao inexistente!!\n"));
          }
          break;
        }

        // Jogador digitou um comando desconhecido
        default:
        {
          erro = 1;
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

/*---------------------------------------------------------*/
/*==== Inicio do jogo e Manutenção ========================*/
/*---------------------------------------------------------*/

void alocar_matriz(int ***matriz, int tam_linha, int tam_coluna) // Aloca uma matriz na memoria
{
  int i;
  (*matriz) = malloc(tam_linha * sizeof(int *));
  for (i = 0; i < tam_linha; i++)
  {
    (*matriz)[i] = malloc(tam_coluna * sizeof(int));
  }
}

void inicializar_var_jogo(RESTA_UM *jogo) // Inicia variáveis do jogo
{
  jogo->num_pinos = 0;
  jogo->foi_inicializado = 0;
  jogo->eh_oh_fim = 0;
  jogo->tamanho_lista_movimentos = 0;
}

void liberar_memoria_jogo(RESTA_UM *jogo) // Libera a memoria aloca no jogo
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

/*---------------------------------------------------------*/
/*==== Manipulação Tabuleiro ==============================*/
/*---------------------------------------------------------*/

void ler_valores_tabela(RESTA_UM *jogo, FILE *ponteiro_arquivo) // Le os valores do tabuleiro
{
  int i, j; // Criação de variáveis

  for (i = 0; i < jogo->tamanho_linhas; i++)
  {
    for (j = 0; j < jogo->tamanho_colunas; j++)
    {
      /*- Caso a função tenha recebido um arquivo, ler valores do arquivo -*/
      if (ponteiro_arquivo != NULL)
      {
        fscanf(ponteiro_arquivo, "%d", &jogo->tabuleiro_jogo[i][j]);
      }

      /*- Caso a função tenha recebido NULL, ou seja, utilizar valores aleatórios -*/

      jogo->num_pinos += (jogo->tabuleiro_jogo[i][j] == 1); // Caso tenha lido um pino, adiciona +1 ao número total de pinos
    }
  }
  jogo->foi_inicializado = 1; // Confirma que o tabuleiro foi inicializado
}

void criar_tabuleiro_aleatorio(RESTA_UM *jogo, int tipo_tabuleiro)
{
  int i, j;

  switch (tipo_tabuleiro)
  {
  case 0: // TABULEIRO PADRÃO
  {

    int tabuleiro_aleatorio[7][7] = {{-1, -1,  1,  1,  1, -1, -1, }, {-1, -1,  1,  1,  1, -1, -1, }, { 1,  1,  1,  1,  1,  1,  1, }, { 1,  1,  1,  0,  1,  1,  1, }, { 1,  1,  1,  1,  1,  1,  1, }, {-1, -1,  1,  1,  1, -1, -1, }, {-1, -1,  1,  1,  1, -1, -1, }};

    for (i = 0; i < jogo->tamanho_linhas; i++)
    {
      for (j = 0; j < jogo->tamanho_colunas; j++)
      {

        jogo->tabuleiro_jogo[i][j] = tabuleiro_aleatorio[i][j];
        jogo->num_pinos += (jogo->tabuleiro_jogo[i][j] == 1); // Caso tenha lido um pino, adiciona +1 ao número total de pinos
      }
    }
    
    break;
  };

  case 1: // TABULEIRO FRANCÊS/EUROPEU
 {

    int tabuleiro_aleatorio[7][7] = {{-1, -1,  1,  1,  1, -1, -1, }, {-1,  1,  1,  1,  1,  1, -1, }, { 1,  1,  1,  0,  1,  1,  1, }, { 1,  1,  1,  1,  1,  1,  1, }, { 1,  1,  1,  1,  1,  1,  1, }, {-1,  1,  1,  1,  1,  1, -1, }, {-1, -1,  1,  1,  1, -1, -1, }};

    for (i = 0; i < jogo->tamanho_linhas; i++)
    {
      for (j = 0; j < jogo->tamanho_colunas; j++)
      {

        jogo->tabuleiro_jogo[i][j] = tabuleiro_aleatorio[i][j];
        jogo->num_pinos += (jogo->tabuleiro_jogo[i][j] == 1); // Caso tenha lido um pino, adiciona +1 ao número total de pinos
      }
    }
    
    break;
  };

  case 2:
 {

    int tabuleiro_aleatorio[9][9] = {{-1, -1, -1,  1,  1,  1, -1, -1, -1 }, {-1, -1, -1,  1,  1,  1, -1, -1, -1 }, {-1, -1, -1,  1,  1,  1, -1, -1, -1 }, { 1,  1,  1,  1,  1,  1,  1,  1,  1 }, { 1,  1,  1,  1,  0,  1,  1,  1,  1 }, { 1,  1,  1,  1,  1,  1,  1,  1,  1 }, {-1, -1, -1,  1,  1,  1, -1, -1, -1 }, {-1, -1, -1,  1,  1,  1, -1, -1, -1 }, {-1, -1, -1,  1,  1,  1, -1, -1, -1 }};

    for (i = 0; i < jogo->tamanho_linhas; i++)
    {
      for (j = 0; j < jogo->tamanho_colunas; j++)
      {

        jogo->tabuleiro_jogo[i][j] = tabuleiro_aleatorio[i][j];
        jogo->num_pinos += (jogo->tabuleiro_jogo[i][j] == 1); // Caso tenha lido um pino, adiciona +1 ao número total de pinos
      }
    }
    
    break;
  };

  case 3:
  {

    int tabuleiro_aleatorio[9][9] = {{-1, -1, -1, -1,  1, -1, -1, -1, -1 }, {-1, -1, -1,  1,  1,  1, -1, -1, -1 }, {-1, -1,  1,  1,  1,  1,  1, -1, -1 }, { -1,  1,  1,  1,  1,  1,  1,  1,  -1 }, { 1,  1,  1,  1,  0,  1,  1,  1,  1 }, { -1,  1,  1,  1,  1,  1,  1,  1,  -1 }, {-1, -1,  1,  1,  1,  1,  1, -1, -1 }, {-1, -1, -1,  1,  1,  1, -1, -1, -1 }, {-1, -1, -1, -1,  1, -1, -1, -1, -1 }};

    for (i = 0; i < jogo->tamanho_linhas; i++)
    {
      for (j = 0; j < jogo->tamanho_colunas; j++)
      {

        jogo->tabuleiro_jogo[i][j] = tabuleiro_aleatorio[i][j];
        jogo->num_pinos += (jogo->tabuleiro_jogo[i][j] == 1); // Caso tenha lido um pino, adiciona +1 ao número total de pinos
      }
    }
    
    break;
  };

  case 4:
  {

    int tabuleiro_aleatorio[8][8] = {{-1, -1,  1,  1,  1, -1, -1, -1}, {-1, -1,  1,  1,  1, -1, -1, -1}, {-1, -1,  1,  1,  1, -1, -1, -1}, { 1,  1,  1,  1,  1,  1,  1,  1}, { 1,  1,  1,  0,  1,  1,  1,  1}, { 1,  1,  1,  1,  1,  1,  1,  1}, { -1,  -1,  1,  1,  1,  -1,  -1,  -1}, { -1,  -1,  1,  1,  1,  -1,  -1,  -1}};

    for (i = 0; i < jogo->tamanho_linhas; i++)
    {
      for (j = 0; j < jogo->tamanho_colunas; j++)
      {

        jogo->tabuleiro_jogo[i][j] = tabuleiro_aleatorio[i][j];
        jogo->num_pinos += (jogo->tabuleiro_jogo[i][j] == 1); // Caso tenha lido um pino, adiciona +1 ao número total de pinos
      }
    }
    
    break;
  };
  }

  jogo->foi_inicializado = 1;
}

int ler_tabuleiro_arquivo(RESTA_UM *jogo, char *nome_arquivo) // Lida com o arquivo digitado como argumento
{
  FILE *ponteiro_arquivo; // Criação do ponteiro de arquivo

  /*--- Verifica se o arquivo existe ----------------------------------*/
  if ((ponteiro_arquivo = fopen(nome_arquivo, "r")) != NULL)
  {

    fscanf(ponteiro_arquivo, "%d %d", &jogo->tamanho_linhas, &jogo->tamanho_colunas); // Le N e M do tabuleiro

    alocar_matriz(&jogo->tabuleiro_jogo, jogo->tamanho_linhas, jogo->tamanho_colunas);

    ler_valores_tabela(jogo, ponteiro_arquivo); // Envia o ponteiro do arquivo para ler os valores do tabuleiro

    fclose(ponteiro_arquivo);
    return 1;
  }

  /*--- Caso o arquivo não exista -------------------------------------*/
  else
  {
    return 0;
  }
}

int salvar_tabuleiro(RESTA_UM *jogo, char *nome_arquivo) // Comando de salvar o tabuleiro no arquivo
{
  int i, j; // Criação de variáveis

  FILE *ponteiro_arquivo; // Criação do ponteiro de arquivo

  ponteiro_arquivo = fopen(nome_arquivo, "w"); // Abrir o arquivo recebido: nome_arquivo

  /*--- Imprime o tamanho do tabuleiro no arquivo ---------------------*/
  fprintf(ponteiro_arquivo, "%2d %2d\n\n", jogo->tamanho_linhas, jogo->tamanho_colunas);

  /*--- Imprime o valores do tabuleiro no arquivo ---------------------*/
  for (i = 0; i < jogo->tamanho_linhas; i++)
  {
    for (j = 0; j < jogo->tamanho_colunas; j++)
    {
      fprintf(ponteiro_arquivo, "%2d ", jogo->tabuleiro_jogo[i][j]);
    }
    fprintf(ponteiro_arquivo, "\n"); // Pula uma linha no arquivo
  }

  fclose(ponteiro_arquivo); // Fecha o arquivo
  return 1;
}

/*---------------------------------------------------------*/
/*==== Verificação de Derrota/Vitoria =====================*/
/*---------------------------------------------------------*/

int tabuleiro_valido(RESTA_UM *jogo) // Verifica se o tabuleiro
{

  int i, j, pino_meio_valido, tamanho_lista_movimentos, movimento_possiveis_atual; // Criação de variáveis

  tamanho_lista_movimentos = 0; // Variável que registra quantos movimentos são possíveis no total

  /*--- Percorre o tabuleiro todo e registra cada movimento possível ---*/
  for (i = 0; i < jogo->tamanho_linhas; i++)
  {
    for (j = 0; j < jogo->tamanho_colunas; j++)
    {
      pino_meio_valido = 0;          // Variável que define se um pino sem movimentos pode ser comido ou não
      movimento_possiveis_atual = 0; // Variável que registra quantos movimentos esse posição pode realizar

      /*--- Caso seja uma parte do tabuleiro ---------------------------*/
      if (jogo->tabuleiro_jogo[i][j] != -1)
      {
        // Movimento para cima válido
        if ((i - 2 >= 0) && jogo->tabuleiro_jogo[i - 1][j] != -1 && jogo->tabuleiro_jogo[i - 2][j] != -1)
        {
          tamanho_lista_movimentos++;
          movimento_possiveis_atual++;
        }

        // Movimento para baixo válido
        if ((i + 2 < jogo->tamanho_linhas) && jogo->tabuleiro_jogo[i + 1][j] != -1 && jogo->tabuleiro_jogo[i + 2][j] != -1)
        {
          tamanho_lista_movimentos++;
          movimento_possiveis_atual++;
        }

        // Movimento para a esquerda válido
        if ((j - 2 >= 0) && jogo->tabuleiro_jogo[i][j - 1] != -1 && jogo->tabuleiro_jogo[i][j - 2] != -1)
        {
          tamanho_lista_movimentos++;
          movimento_possiveis_atual++;
        }

        // Movimento para a direita válido
        if ((j + 2 < jogo->tamanho_colunas) && jogo->tabuleiro_jogo[i][j + 1] != -1 && jogo->tabuleiro_jogo[i][j + 2] != -1)
        {
          tamanho_lista_movimentos++;
          movimento_possiveis_atual++;
        }

        // Peça pode ser comida horizontalmente
        if ((j > 0) && jogo->tabuleiro_jogo[i][j] == 1 && (j < jogo->tamanho_colunas - 1) && (jogo->tabuleiro_jogo[i][j - 1] != -1 && jogo->tabuleiro_jogo[i][j + 1] != -1))
        {
          pino_meio_valido++;
        }

        // Peça pode ser comida verticalmente
        if ((i > 0) && jogo->tabuleiro_jogo[i][j] == 1 && (i < jogo->tamanho_linhas - 1) && (jogo->tabuleiro_jogo[i - 1][j] != -1 && jogo->tabuleiro_jogo[i + 1][j] != -1))
        {
          pino_meio_valido++;
        }

        // Caso exista um PINO que não pode ser acessado ou que não pode ser comido, define um tabuleiro inválido
        if (jogo->tabuleiro_jogo[i][j] == 1 && movimento_possiveis_atual == 0 && pino_meio_valido == 0)
        {
          return 0; // Tabuleiro inválido
        }
      }
    }
  }

  /*- Aloca lista de movimento que registra todos os movimentos possíveis -*/
  jogo->lista_de_movimentos = malloc(tamanho_lista_movimentos * sizeof(MOVIMENTO));
  jogo->tamanho_lista_movimentos = tamanho_lista_movimentos;

  /*--- Caso não haja movimento possíveis --------------------------*/
  if (tamanho_lista_movimentos == 0)
  {
    return 0; // Tabuleiro inválido
  }

  return 1; // Tabuleiro válido
}

/*---- Inteligência Artificial ----------------------------*/
/*   A função de inteligência artificial decide o melhor   */
/* movimento, usando um algoritimo de backtracking. Ele    */
/* procura pelo melhor movimento até que o jogo tenha sido */
/* ganho ou tenha atingido o numero de movimentos pedido,  */
/* dado pelo parametro limite_index_ia. A função pode ser  */
/* usada para verificação também, basta definir com o      */
/* parametro salvar_movimento.                             */
/*---------------------------------------------------------*/

int decidir_movimento_ia(RESTA_UM *jogo, int limite_index_ia, int index_ia, int salvar_movimento) // Função de ajuda
{
  int i; // Criação de variáveis

  /*---- Condição de saída do loop de backtracking --------*/
  if (jogo->num_pinos == 1 || index_ia == limite_index_ia) // Volta caso o jogo tenha sido ganho ou tenha atingido o numero de movimentos
  {
    jogo->lista_de_movimentos_ia[index_ia] = cria_movimento(0, 0, 0, 0); // Marca o fim da lista de movimentos com nulo
    return 1;
  }
  else
  {
    /*---- Testa todos os movimentos possíveis ------------*/
    for (i = 0; i < jogo->tamanho_lista_movimentos; i++)
    {
      if (movimento_valido(jogo, jogo->lista_de_movimentos[i])) // Verifica se o movimento for válido
      {
        faz_movimento(jogo, jogo->lista_de_movimentos[i]);

        /*- Chama função recursivamente com novo movimento */
        if (decidir_movimento_ia(jogo, limite_index_ia, index_ia + 1, salvar_movimento)) // Caso o movimente leva a solução
        {
          jogo->lista_de_movimentos_ia[index_ia] = jogo->lista_de_movimentos[i]; // Salva movimento feito na lista
          desfaz_movimento(jogo, jogo->lista_de_movimentos[i]);                  // Desfaz pois outra função os executa e imprime o resultado
          return 1;
        }

        desfaz_movimento(jogo, jogo->lista_de_movimentos[i]);
      }
    }
    return 0;
  }
}

/*---------------------------------------------------------*/
/*==== Movimento ==========================================*/
/*---------------------------------------------------------*/

MOVIMENTO cria_movimento(int posicao_x, int posicao_y, int direcao_x, int direcao_y) // Cria um movimento
{
  MOVIMENTO movimento_criado; // Criação de variáveis

  // Define o pino inical
  movimento_criado.pino_inicial.x = posicao_x;
  movimento_criado.pino_inicial.y = posicao_y;

  // Define o pino do meio
  movimento_criado.pino_meio.x = posicao_x + direcao_x;
  movimento_criado.pino_meio.y = posicao_y + direcao_y;

  // Define o pino de destino
  movimento_criado.pino_destino.x = posicao_x + (2 * direcao_x);
  movimento_criado.pino_destino.y = posicao_y + (2 * direcao_y);

  return movimento_criado;
}

int cria_lista_movimentos(RESTA_UM *jogo) // Cria a lista de movimento possíveis
{
  int i, j, contador_lista_movimentos; // Criação de variáveis

  contador_lista_movimentos = 0; // Contador da lista, e conta quantos movimento são possíveis

  /*-- Percorre o tabuleiro e registra na lista os movimentos possíveis --*/
  for (i = 0; i < jogo->tamanho_linhas; i++)
  {
    for (j = 0; j < jogo->tamanho_colunas; j++)
    {
      if (jogo->tabuleiro_jogo[i][j] != -1) // Se for parte do tabuleiro
      {
        // CIMA
        if ((i - 2 >= 0) && jogo->tabuleiro_jogo[i - 1][j] != -1 && jogo->tabuleiro_jogo[i - 2][j] != -1)
        {
          // Salva movimento na lista
          jogo->lista_de_movimentos[contador_lista_movimentos] = cria_movimento(i, j, -1, 0);
          contador_lista_movimentos++;
        }

        // BAIXO
        if ((i + 2 < jogo->tamanho_linhas) && jogo->tabuleiro_jogo[i + 1][j] != -1 && jogo->tabuleiro_jogo[i + 2][j] != -1)
        {
          // Salva movimento na lista
          jogo->lista_de_movimentos[contador_lista_movimentos] = cria_movimento(i, j, 1, 0);
          contador_lista_movimentos++;
        }

        // ESQUERDA
        if ((j - 2 >= 0) && jogo->tabuleiro_jogo[i][j - 1] != -1 && jogo->tabuleiro_jogo[i][j - 2] != -1)
        {
          // Salva movimento na lista
          jogo->lista_de_movimentos[contador_lista_movimentos] = cria_movimento(i, j, 0, -1);
          contador_lista_movimentos++;
        }

        // DIREITA
        if ((j + 2 < jogo->tamanho_colunas) && jogo->tabuleiro_jogo[i][j + 1] != -1 && jogo->tabuleiro_jogo[i][j + 2] != -1)
        {
          // Salva movimento na lista
          jogo->lista_de_movimentos[contador_lista_movimentos] = cria_movimento(i, j, 0, 1);
          contador_lista_movimentos++;
        }
      }
    }
  }

  // Salva o numero de movimentos possiveis
  jogo->tamanho_lista_movimentos = contador_lista_movimentos;

  if (contador_lista_movimentos == 0) // Caso não haja nenhum movimento
  {
    return 0; // Derrota instantânea
  }

  return 1;
}

int gerenciar_movimento(RESTA_UM *jogo, char *comando_digitado) // Gerencia o movimento pedido pelo jogador
{
  // Criação de variáveis
  int i, direcao_x, direcao_y, temp_x, temp_y;
  MOVIMENTO movimento_atual;

  /*- Transforma char em int, letra para numero, do comando -*/
  temp_x = (int)comando_digitado[2] - NUM_PRA_LETRA; // Subtrai 65
  temp_y = (int)comando_digitado[3] - NUM_PRA_LETRA; // Subtrai 65

  /*---------------------------*/
  /*  Pega a direcao digitada  */
  /*---------------------------*/
  /* CIMA     = -1 na linha    */
  /* BAIXO    = +1 na linha    */
  /* ESQUERDA = -1 na coluna   */
  /* DIREITA  = +1 na coluna   */
  /*---------------------------*/

  switch (comando_digitado[0])
  {
  case 'c': // CIMA
  {
    direcao_x = -1;
    direcao_y = 0;

    break;
  }
  case 'b': // BAIXO
  {
    direcao_x = 1;
    direcao_y = 0;

    break;
  }
  case 'e': // ESQUERDA
  {
    direcao_x = 0;
    direcao_y = -1;

    break;
  }
  case 'd': // DIREITA
  {
    direcao_x = 0;
    direcao_y = 1;

    break;
  }
  }

  // Cria o movimento do jogador
  movimento_atual = cria_movimento(temp_x, temp_y, direcao_x, direcao_y);

  /*---- Percorre toda a lista de movimentos ----------------*/
  for (i = 0; i < jogo->tamanho_lista_movimentos; i++)
  {
    /*---- Ve se o movimento do jogador existe na lista -----*/
    if (memcmp(&jogo->lista_de_movimentos[i], &movimento_atual, sizeof(MOVIMENTO)) == 0)
    {
      if (movimento_valido(jogo, movimento_atual)) //Verifica se o movimento é válido
      {
        faz_movimento(jogo, movimento_atual); // Faz e registra o movimento
        return 1;
      }
    }
  }

  return 0;
}

void faz_movimento(RESTA_UM *jogo, MOVIMENTO movimento) // Faz o movimento
{
  //   Pino inicial pula por cima do pino do meio, chegando no destino.
  // Removendo os pinos inciais e do meio e colocando um pino no destino
  jogo->tabuleiro_jogo[movimento.pino_inicial.x][movimento.pino_inicial.y] = 0;
  jogo->tabuleiro_jogo[movimento.pino_meio.x][movimento.pino_meio.y] = 0;
  jogo->tabuleiro_jogo[movimento.pino_destino.x][movimento.pino_destino.y] = 1;
  jogo->num_pinos--; // Retira um pino
}

void desfaz_movimento(RESTA_UM *jogo, MOVIMENTO movimento) // Desfaz o movimento
{
  //   Pino no destino pula por cima do pino do meio, retornando ao ponto inicial
  // e colocando o pino do meio de volta
  jogo->tabuleiro_jogo[movimento.pino_inicial.x][movimento.pino_inicial.y] = 1;
  jogo->tabuleiro_jogo[movimento.pino_meio.x][movimento.pino_meio.y] = 1;
  jogo->tabuleiro_jogo[movimento.pino_destino.x][movimento.pino_destino.y] = 0;
  jogo->num_pinos++;
}

int movimento_valido(RESTA_UM *jogo, MOVIMENTO movimento_teste) // Verifica se o movimento é válido
{
  int inicial_x, inicial_y, meio_x, meio_y, destino_x, destino_y; // Criação de variáveis

  // Váriaveis temporárias para reduzir e melhorar a leitura
  inicial_x = movimento_teste.pino_inicial.x;
  inicial_y = movimento_teste.pino_inicial.y;

  meio_x = movimento_teste.pino_meio.x;
  meio_y = movimento_teste.pino_meio.y;

  destino_x = movimento_teste.pino_destino.x;
  destino_y = movimento_teste.pino_destino.y;

  /*---- Verifica se o movimento é válido -------------------*/
  if (jogo->tabuleiro_jogo[inicial_x][inicial_y] == 1 && jogo->tabuleiro_jogo[meio_x][meio_y] == 1 && jogo->tabuleiro_jogo[destino_x][destino_y] == 0)
  {
    return 1;
  }

  return 0; // Movimento inválido
}

/*---------------------------------------------------------*/
/*==== Imprimir na Tela ===================================*/
/*---------------------------------------------------------*/

void imprimir_movimentos_ajuda(RESTA_UM *jogo) // Imprime os movimentos feito pelo comando ajuda
{
  // Declaração de Variaveis
  int i;
  char direcao;
  MOVIMENTO fim_lista;

  // Iniciação de Variaveis
  i = 0;
  fim_lista = cria_movimento(0, 0, 0, 0); // Cria um movimento nulo, usado para achar o fim da lista

  /*---- Imprime o movimento feito pela ajuda -------------*/
  // Enquanto a lista não chegar no movimento nulo, que marca o fim da lista
  while (memcmp(&jogo->lista_de_movimentos_ia[i], &fim_lista, sizeof(MOVIMENTO)))
  {
    faz_movimento(jogo, jogo->lista_de_movimentos_ia[i]);
    imprimir_tabuleiro_jogo(jogo); // Imprime o tabuleiro modificado

    printf(BLUE("O Movimento ")); // Imprime o movimento realizado

    /*---- Acha a direção do movimento --------------------*/
    if (jogo->lista_de_movimentos_ia[i].pino_inicial.x > jogo->lista_de_movimentos_ia[i].pino_meio.x)
    {
      direcao = 'c'; // CIMA
    }
    else if (jogo->lista_de_movimentos_ia[i].pino_inicial.x < jogo->lista_de_movimentos_ia[i].pino_meio.x)
    {
      direcao = 'b'; // BAIXO
    }
    else if (jogo->lista_de_movimentos_ia[i].pino_inicial.y > jogo->lista_de_movimentos_ia[i].pino_meio.y)
    {
      direcao = 'e'; // ESQUERDA
    }
    else
    {
      direcao = 'd'; // DIREITA
    }

    printf(CYAN("%c %c"), direcao, jogo->lista_de_movimentos_ia[i].pino_inicial.x + NUM_PRA_LETRA); // Transforma o numero em letra adicionando 65
    printf(CYAN("%c"), jogo->lista_de_movimentos_ia[i].pino_inicial.y + NUM_PRA_LETRA);             // Transforma o número em letra adicionando 65
    printf(BLUE(" foi executado com sucesso..."));
    printf("\n");

    i++; // Adiciona 1 pro contador
  }
}

void imprimir_tabuleiro_jogo(RESTA_UM *jogo) // Imprime o tabuleiro e as pinos
{
  int i, j; // Declaração de Variaveis

  /*---- Imprime quantos pinos faltam -------------------*/
  printf("\n\n\n\t");
  for (j = 0; j < jogo->tamanho_colunas - 5; j++)
  {
    printf(WHITE("  "));
  }
  // Imprime RESTA ou RESTAM dependendo se há 1 ou mais pinos, plural
  printf(WHITE("Resta%c %3d pino%c\n"), 109 * (jogo->num_pinos > 1), jogo->num_pinos, 115 * (jogo->num_pinos > 1));

  /*---- Imprime o tabuleiro ----------------------------*/

  // Linha Superior
  printf(WHITE("\t" TAB_TL TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR));
  for (j = 0; j < jogo->tamanho_colunas; j++)
  {
    printf(WHITE(TAB_HOR TAB_HOR));
  }
  printf(WHITE(TAB_HOR TAB_HOR TAB_TR "\n"));

  // Linha com as marcações de coluna
  printf(WHITE("\t" TAB_VER "    "));
  for (j = 0; j < jogo->tamanho_colunas; j++)
  {
    printf(WHITE(BOLD(" %c")), j + NUM_PRA_LETRA);
  }
  printf(WHITE("    " TAB_VER "\n"));

  // Linha de espaço
  printf(WHITE("\t" TAB_VER "    "));
  for (j = 0; j < jogo->tamanho_colunas + 2; j++)
  {
    printf("  ");
  }
  printf(WHITE(TAB_VER "\n"));

  // Tabuleiro e Marcações de linha
  for (i = 0; i < jogo->tamanho_linhas; i++)
  {
    // Marcações de linha
    printf(WHITE(BOLD("\t" TAB_VER " %c  ")), i + NUM_PRA_LETRA);
    for (j = 0; j < jogo->tamanho_colunas; j++)
    {
      switch (jogo->tabuleiro_jogo[i][j])
      {
      case -1:
      {
        printf("  "); // Não faz parte do tabuleiro -> -1
        break;
      }
      case 1:
      {
        printf(WHITE(BOLD(" o"))); // Pino -> 1
        break;
      }
      case 0:
      {
        printf(WHITE(BOLD(" ."))); // Casa sem pino -> 0
        break;
      }
      }
    }
    printf(WHITE("    " TAB_VER "\n"));
  }

  // Linha de espaço
  printf(WHITE("\t" TAB_VER "    "));
  for (j = 0; j < jogo->tamanho_colunas + 2; j++)
  {
    printf("  ");
  }
  printf(WHITE(TAB_VER "\n"));

  // Linha Inferior
  printf(WHITE("\t" TAB_BL TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR));

  for (j = 0; j < jogo->tamanho_colunas + 1; j++)
  {
    printf(WHITE(TAB_HOR TAB_HOR));
  }
  printf(WHITE(TAB_BR "\n"));
}

void imprimir_msg_de_fim(char *tipo_de_fim) // Imprime mensagem de fim, Vitoria ou Derrota
{
  // Derrota
  if (strcmp(tipo_de_fim, "derrota") == 0)
  {
    printf(RED(BOLD("\tDERROTA!!\n")));
    // Escolhe 1 de 3 frases de derrota
    switch (rand() % 3)
    {
    case 0:
    {
      printf(RED("\tNão há mais como ganhar parceiro, desista... :c\n"));
      break;
    }
    case 1:
    {
      printf(RED("\tÉ pois é, não da pra ganhar todas amigão...:c\n"));
      break;
    }
    case 2:
    {
      printf(RED("\tAgora só resta tentar denovo né... :c\n"));
      break;
    }
    }
  }

  // Vitoria
  else if (strcmp(tipo_de_fim, "vitoria") == 0)
  {
    printf(GREEN(BOLD("\tVITORIA!!\n")));
    printf(GREEN("\tVoce ganhou!! Parabens amigo :D\n"));
  }
}

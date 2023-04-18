/*
    SISTEMAS OPERACIONAIS - TRABALHO 01
    Implementação de Shell

    Eduardo José de Souza Magalhães, RA: 122038
    Felipe Baz Mitsuishi, RA: 140867
    Rafael Braga Ennes, RA: 156357
    Stefanie Soares da Silva, RA: 133808
*/

//================DECLARAÇÕES=============

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define PIPE '|'
#define ECOMERCIAL '&'
#define REDIRECIONA '<'
#define SAIDA '>'
#define AND_OPTION 1
#define OR_OPTION 2
#define BACKGROUND 1

#define MAX_ARG_SIZE 50

int background_flag = 0;

int condicao_atual_option = 0;

//=======================================

// Função que imprime a matriz de argumentos
void imprime_argv(char **argv) {
  printf("imprime argv\n");
  for (int i = 0; argv[i] != NULL; i++) {
    printf("%s ", argv[i]);
  }
  printf("\n");
}

// Função que retorna a posição de um caractere
int posicao_char(char character, int i, char **argv) {
  while (argv[i]) {
    if (*argv[i] == character) {
      return i;
    }
    i++;
  }
  return -1;
}

// Função que conta a quantidade de caracteres de uma string
int conta_char(char character, char **argv) {
  int c = 0, i = 0;
  while (argv[i]) {
    if (*argv[i] == character) {
      c++;
    }
    i++;
  }
  return c;
}

// Função que encontra a posição do comando condicional (|| ou &&)
int pos_cmd_cond(char **argv) {
  int i = 0;
  while (argv[i]) {
    if ((argv[i][0] == ECOMERCIAL && argv[i][1] == ECOMERCIAL) ||
        (argv[i][0] == PIPE && argv[i][1] == PIPE))
      return i;
    i++;
  }
  return -1;
}

// Função que executa um comando simples ou BG
int executar_cmd(char **argv) {
  pid_t pid;
  pid = fork();

  if (pid == 0) {
    execvp(argv[0], argv);
    return 0;
  } else if (pid > 0) {
    int status;
    if (background_flag != BACKGROUND) {
      waitpid(pid, &status, 0);
    }
    return WEXITSTATUS(status);
  } else {
    return -1;
  }
}

// Função que executa comandos condicionais
int executar_cmd_cond(char **argv, int posicao) {
  char **condicao_comando;
  if (posicao != -1) {
    condicao_comando = &argv[posicao + 1];
    condicao_atual_option =
        (argv[posicao][0] == ECOMERCIAL) ? AND_OPTION : OR_OPTION;
    argv[posicao] = NULL;
  } else {
    condicao_atual_option = 0;
  }

  int status = executar_cmd(argv);

  if ((condicao_atual_option == AND_OPTION && status == 0) ||
      (condicao_atual_option == OR_OPTION && status != 0)) {
    int pos = pos_cmd_cond(condicao_comando);
    return executar_cmd_cond(condicao_comando, pos);
  }

  return status;
}

//Função que executa comandos PIPE
int fd[j][2]; //descritor de arquivos
    int i = 0, aux_in = STDIN_FILENO, aux_out = STDOUT_FILENO, n;
    n = posicao_char(PIPE, i, argv);
    char **cmd = &argv[i];

    if (n != -1)
        cmd [n-i] = NULL; // desconsidera o pipe e pegar apenas o comando
   
    for (i=0; i<j; i++)
    {
        //Não é o último processo
        if(i != j-1)
        {
            if(pipe(fd[i])<0)
            {
                perror("Pipe");
                exit(-1);
            }
        }
       
        //Child - Processo filho
        if(fork()==0)
        {
            close (fd[i-1][0]); //fecha descritor de leitura
            dup2(aux_in, aux_in); //duplica pipe sobre entrada padrao

            if (i < j)
                dup2(fd[1][1], aux_out); // enquanto tiver j-1 instruções, vai duplicar a o canal de saída

            execvp(cmd[0], cmd); // ponteiro de instrução
            return 0;
        }
        //Processo pai
        else if (fork() > 0 )
        {
            close(fd[i-1][0]); //fecha leitura
            close(fd[i-1][1]); //fecha escrita
            waitpid(-1, NULL, 0); // pai aguarda os filhos finalizarem
        }
        else
        {
            perror("Fork");
            exit(-1);
        }

    }
    return 0;
}

// Função que encontra a posição do redirecionador de saída (>>)
int pos_red_append(char **argv) {
  int i = 0;
  while (argv[i]) {
    if ((argv[i][0] == SAIDA && argv[i][1] == SAIDA))
      return i;
    i++;
  }
  return -1;
}

// Função que executa comandos de redirecionamento
void executar_cmd_redirecionamento(char **argv, int j) {
  int n = posicao_char(REDIRECIONA, 0, argv);
  char *arquivo = argv[n + 1];
  FILE *input_file = fopen(arquivo, "r");

  if (input_file == NULL) {
    printf("Erro ao abrir arquivo de entrada: %s\n", arquivo);
    exit(1);
  }

  // Redirecione a entrada padrão para o arquivo de entrada
  if (freopen(arquivo, "r", stdin) == NULL) {
    printf("Erro ao redirecionar entrada para o arquivo: %s\n", arquivo);
    exit(1);
  }

  fclose(input_file);

  // verificar se o operador de saída é '>' ou '>>'
  n = pos_red_append(argv);
  if (n > 0) {
    // Fazer append das novas informações do texto
    FILE *output_file = fopen(argv[n + 1], "a");
    // Imprime ordenado no novo arquivo
    if (output_file == NULL) {
      printf("Erro ao abrir arquivo de saída: %s\n", argv[n + 1]);
      exit(1);
    }

    // Redirecione a saída padrão para o arquivo de saída
    if (freopen(argv[n + 1], "a", stdout) == NULL) {
      printf("Erro ao redirecionar saída para o arquivo: %s\n", argv[n + 1]);
      exit(1);
    }

    // Execute o comando sort
    system("sort");

    fclose(output_file);
  } else {
    // Verificar se existe o operador '>'
    n = posicao_char(SAIDA, 0, argv);
    if (n > 0) {
      // Sobrescreve o que estava escrito, caso o arquivo já existisse
      FILE *output_file = fopen(argv[n + 1], "w");
      // Imprime ordenado no novo arquivo
      if (output_file == NULL) {
        printf("Erro ao abrir arquivo de saída: %s\n", argv[n + 1]);
        exit(1);
      }

      // Redirecione a saída padrão para o arquivo de saída
      if (freopen(argv[n + 1], "w", stdout) == NULL) {
        printf("Erro ao redirecionar saída para o arquivo: %s\n", argv[n + 1]);
        exit(1);
      }
      // Execute o comando sort
      system("sort");

      fclose(output_file);
    }
  }
}

int executar_cmd_red_pipe(char **argv) {

  int fd[j][2]; //descritor de arquivos
    int i = 0, aux_in = STDIN_FILENO, aux_out = STDOUT_FILENO, n;
    n = posicao_char(PIPE, i, argv);
    char **cmd = &argv[i];

    if (n != -1)
        cmd [n-i] = NULL; // desconsidera o pipe e pegar apenas o comando
   
    for (i=0; i<j; i++)
    {
        //Não é o último processo
        if(i != j-1)
        {
            if(pipe(fd[i])<0)
            {
                perror("Pipe");
                exit(-1);
            }
        }
       
        //Child - Processo filho
        if(fork()==0)
        {
            close (fd[i-1][0]); //fecha descritor de leitura
            dup2(aux_in, aux_in); //duplica pipe sobre entrada padrao

            if (i < j)
                dup2(fd[1][1], aux_out); // enquanto tiver j-1 instruções, vai duplicar a o canal de saída

            execvp(cmd[0], cmd); // ponteiro de instrução
            return 0;
        }
        //Processo pai
        else if (fork() > 0 )
        {
            close(fd[i-1][0]); //fecha leitura
            close(fd[i-1][1]); //fecha escrita
            waitpid(-1, NULL, 0); // pai aguarda os filhos finalizarem
        }
        else
        {
            perror("Fork");
            exit(-1);
        }

    }
    return 0;
}

// Função principal
int main(int argc, char **argv) {
  // Caso o número de argumentos passados seja igual a 1, não houveram comandos
  // inseridos, o primeiro argumento é o nome do programa
  if (argc == 1) {
    printf("Os comandos não foram inseridos\n");
    return 0;
  }

  char **comando = &argv[1];

  int posicao = pos_cmd_cond(comando);
  // Se a variável posição for maior que zero, quer dizer que há um comando
  // condicional
  if (posicao > 0) {
    executar_cmd_cond(comando, posicao);
    // Se não tiver, procure pelo pipe
  } else {
    int n = conta_char(PIPE, comando);
    int b = conta_char(REDIRECIONA, comando);
    // Se a variável n for maior que zero é porque há, pelo menos, um pipe
    if (n > 0 && b <= 0) {
      executar_cmd_pipe(comando, n);
    } else {
      n = conta_char(REDIRECIONA, comando);
      // Verificar se existe o operador ‘<’
      // Caso exista, deverá fazer o redirecionamento da entrada
      if (n > 0) {
        int b = conta_char(PIPE, comando);
        // Verifica se existe o simbolo "|" para realizar a combinacao de
        // operadores
        if (b > 0) {
          executar_cmd_red_pipe(comando);
          // Funcao para redirecionar com op
        } else {
          executar_cmd_redirecionamento(comando, n);
        }
      } else {
        // Caso não tenha, procure por comandos de background
        n = conta_char(ECOMERCIAL, comando);
        // Caso exista algum, o valor da variável n será maior que 0
        if (n > 0) {
          background_flag = BACKGROUND;
          posicao = posicao_char(ECOMERCIAL, 0, comando);
          comando[posicao] = NULL;
        }
        executar_cmd(comando);
      }
    }
  }
  return 0;
}


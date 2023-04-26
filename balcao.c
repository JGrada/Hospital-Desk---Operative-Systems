//
// Created by User on 07/01/2022.
//

#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "balcao.h"

#define FIFO_SRV "canal"
#define FIFO_CLI "filho"

char* Balcao_Classicador(int b_c[2], int c_b[2]){

    do{ //Escrita do Balcao para o Classificador:::::::::::::::::::::::::::Processo Pai

        printf("Insira quais os sintomas que o utente sente: \n");
        scanf(" %24[^\n]", info);

        if(!strcmp(info, "fim"))
        {
            break;
        }

        strcat(info, "\n");

        write(b_c[1], info, strlen(info)); //para onde vai, o que vai, tamanho do que vai
        read(c_b[0], info, 24); //lemos do classificador a info, com o tamanho 24;

        printf("%s \n", strtok(info, "\n"));


    }while(strcmp(info, "fim"));


    return info;
}



int main(){
    int b_c[2]; // Balcao para classificador
    int c_b[2]; // Classificador para balcao
    int p;
    int s, r;
    int state;
    char info[25];


    s = pipe(b_c);
    r = pipe(c_b);
    p = fork();

    if (s == -1)
    {
        printf("Não foi possível realizar o pretendido \n");
        exit(1);
    }

    if (r == -1)
    {
        printf("Não foi possível realizar o pretendido \n");
        exit(1);
    }

    if (p == 0) //Abrir classificador::::::::::::::::Processo Filho
    {
        close(b_c[1]); //fecha a escrita do balcao para o classificador
        close(c_b[0]); //fecha a leitura do balcao para o classificador


        close(STDIN_FILENO);

        dup(b_c[0]); //copia para a posicao do STDIN o conteudo que estava na posicao [0]
        close(b_c[0]); // fecha

        close(STDOUT_FILENO);

        dup(c_b[1]); // copia para o STDOUT o que tinha lido
        close(c_b[1]); //fecha

        execl("classificador", "classificador", NULL); //Executa o Classificador

    }
    Balcao_Classicador(b_c[1], c_b[0]);

    write(b_c[1], "#fim\n", strlen("#fim\n"));


    wait(&state);
    printf("O classificador acabou com o seguinte estado %d: \n", WEXITSTATUS(state));

    exit(0);


}



void Balcao_Cliente() {

    int a[2];
    int b[2];

    char *infoBalcao;
    infoBalcao = Balcao_Classicador(a, b);



    int fd, fd_retorno, n;
    char str[30];
    servico s;

    //verificar se o fifo existe

    if (access(FIFO_SRV, F_OK) != 0) {
        printf("O fifo nao existe");
        exit(1);
    }

    //criar o meu fifo
    s.pid = getpid();
    sprintf(str, FIFO_CLI, s.pid);
    mkfifo(str, 0600);

    //abrir o fifo
    fd = open(FIFO_SRV, o_WRonly);
    printf("Abri o fifo");

    //escrever do fifo
    s.pid = getpid();

    do {
        printf("%s \n", infoBalcao);
        n = write(fd, &s, sizeof(servico));
        //receber resposta
    } while (1);

    //fechar fifo
    close(fd);
    printf("Fifo fechado");

    //apagar o meu fifo (unlink)
    unlink(str);

    exit(0);
}




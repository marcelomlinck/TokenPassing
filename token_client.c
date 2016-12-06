/*  File: token_server.c         */
/*  Author: Marcelo Melo Linck   */
/*  Version: 1.5                 */
/*  Date: 09/18/2013             */
/*  marcelo.linck@acad.pucrs.br  */


#include <stdio.h>
#include <stdlib.h>
#include <rpc/rpc.h>
#include <string.h>
#include <math.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>

#include "token.h"
#define loop 5       // Número de vezes em que o cliente irá acessar a SC


//Cliente é padrão para todos os servidores, os parametros passados são o IP local e o ID


main(int argc, char *argv[])
{
	CLIENT *client;
	int ID = atoi(argv[2]);    //ID passada por parametro definindo a operação

	int sleeptime;
	int i;

	int *ans;
	void *pvoid;
	char *localIP;

	if (argc != 3)
	{
		printf("Usage: %s localIP ID\n", argv[0]);
		exit(1);
	}
	localIP = argv[1];

	printf("\n\t-Starting Client-\n");

	sleep(5);           

	//Inicia o cliente e tenta conexão

	if (!(client = clnt_create(localIP, SCPROG, SCVERS, "tcp")))  //Inicia conexão com servidor
	{
		clnt_pcreateerror(localIP);
		return(1);
	}
	
	for (i=0;i<loop;i++)          //Inicia uma chamada para acesso "loop" vezes
	{
	    	srand(time(NULL));

		sleeptime = rand()%15;	//Gera um tempo aleatório entre chamadas
                                //Com uma variação de 1 a 15 segundos
	   	sleep(sleeptime);

		printf("***Client requires access to the Critic Session!\n"); 

		// Cliente necessita acesso a seesão crítica, e faz a solicitação
		while(1)
		{
		    sleep(1);
		    ans = writerequest_1(pvoid,client);    //solicitação de acesso a SC ao servidor
		    if(*ans == 0) break;
		}
		printf("Client Authorized. Accessing...");

        
                                                    //Processo para escrever no arquivo
		FILE *file;
		char line[15];
		char line2[15];
		int newValue;
		if ((file = fopen("/home/Linux/Desktop/Shared/arquivo.txt", "a+")) == NULL) //abre arquivo - INFORMAR CAMINHO PARA O ARQUIVO AQUI!
		{
		    printf("\n\nFile failed to open. Check directory permissions or open the program as a SuperUser\n");
		    writerelease_1(pvoid,client);
		    return 1;
		}

		while (!feof(file))
		{
		    fgets(line,sizeof(line),file);
		}

		sleep(1);
		printf("\nRead from file: %s", line);
		switch (ID)                                         //Le ID e define qual operação a ser realizada
		{
			case 0:
				printf("\nClient will add 10 to the previous value.");
				newValue = atoi(line) + 10;                                 //calcula
				printf("\nWritting new value: %d\n", newValue);
				sprintf(line2, "%d\n", newValue);
				fputs("+10\n", file);                                       //escreve operação realizada
	    			fputs(line2, file);                                     //escreve no arquivo
				break;
			case 1:
				printf("\nClient will sub 5 from the previous value.");
				newValue = atoi(line) - 5;
				printf("\nWritting new value: %d\n", newValue);
				sprintf(line2, "%d\n", newValue);
				fputs("-5\n", file);
	    			fputs(line2, file);
				break;
			case 2:
				printf("\nClient will multiply by 3 to the previous value.");
				newValue = atoi(line) * 3;
				printf("\nWritting new value: %d\n", newValue);
				sprintf(line2, "%d\n", newValue);
				fputs("x3\n", file);
	    			fputs(line2, file);
				break;
			case 3:
				printf("\nClient will add 2 to the previous value.");
				newValue = atoi(line) + 2;
				printf("\nWritting new value: %d\n", newValue);
				sprintf(line2, "%d\n", newValue);
				fputs("+2\n", file);
	    			fputs(line2, file);
				break;
			case 4:
				printf("\nClient will sub 4 to the previous value.");
				newValue = atoi(line) - 4;
				printf("\nWritting new value: %d\n", newValue);
				sprintf(line2, "%d\n", newValue);
				fputs("-4\n", file);
	    			fputs(line2, file);
				break;
		}
	    	fclose(file);                                           //fecha arquivo
	    	printf("-Client Finished Writting.\n");
            ans = writerelease_1(pvoid,client);                     //chama função do servidor informando que não é mais necessário acessar a SC
		if(*ans != 0)                                               // Se o servidor retornar 0, operaçao falhou
		{
			printf("\nERROR: Server Failed to liberate client. Aborting program!\n");
			exit(1);
		}
	    	printf("-Client Finished Access Successfully!\n");
	}


	printf("\n\n \033[1m Client Shutting Down!\033[30m \n");
	return 0;
}

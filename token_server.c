/*  File: token_server.c         */
/*  Author: Marcelo Melo Linck   */
/*  Version: 1.5                 */
/*  Date: 09/18/2013             */
/*  marcelo.linck@acad.pucrs.br  */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <semaphore.h>
#include <string.h>
#include "token.h"


//#define STARTTOKEN 1                //1 para o servidor que inicia com o token - 0 para os demais
#define N_MACHINES 3             	//Número de servidores no sistema

int localID;
int nextID;
char * network[N_MACHINES];
int hasToken;
int wantToken;
int print;
int alreadyHave;                    //solução encontrada quando a conexão demora, e o toke é passado,
                                    //como na função start, o token é zerado, essa variável impede de o
                                    //servidor perder o token, ou seja, zerar antes de passar.
CLIENT * client[N_MACHINES];
void *pvoid;

int *doyouhavetoken_1_svc(void *pvoid, struct svc_req *rqstp)		//Função do Servidor - Caso o Servidor possua o token, retorna 0, caso não, retorna 1
{
	static int ans;
	if(hasToken == 1)
		ans = 0;
	else ans = 1;
	
	return &ans;
}

void WhereIsToken()				//Onde está o token?
{
	int lowestID;
	int found;
	int i;
	int *ans;
	lowestID=5;
	found = 0;
	for(i=N_MACHINES-1;i>=0;i--)				//Note que o for esta indo inversamente, pois o ultimo servidor a responder 
	{											//irá sempre ser o com menor ID		
		ans = doyouhavetoken_1(pvoid, client[i]);		//Pergunta para cada servidor se ele possui o token
		if(ans == NULL) 							//Se ele for achado, processo termina e o servidor espera 
												//novamente 30 segundos para fazer a pergunta novamente
			printf("\nServer %s is out of reach!\n", network[i]);
		else if(*ans == 0)						//Caso o servidor com qual ele se comunicou, responda não
		{
			found = 1;	
			break;
		}
		else 	
		{										//Ele seta aquele servidor como menor ID
			lowestID = i;				//Caso o servidor tenha caído, mensagem é impressa
		}
	}
	if(!found)					//Caso ele não tenha achado o token na rede ele for a menor ID, ele cria o token
	{
		if(lowestID == localID) 
		{
			hasToken = 1;
			printf("***Server created a Token!\n");
		}	
		else					//Caso ele não seja a menor ID, ele não faz nada
		{
			printf("This Server is not the lowest ID Online.\n");
		}
	}
	else
	{
		printf("\nServer %s has the token.\n", network[i]);	//Se achado, imprime quem possui o token
	}
	return;	
}

void *VerifyToken()     //Thread que roda de 1 em 1 segundo, verificando se o servidor possui o token
{
	int tokenCounter=0;								//Contador que auxilia na verificação da existência do token na rede, contador conta em segundos
	int *ans;	
	do{
		if(tokenCounter<30)								//Contagem até 30 segundos
		{
			if(!hasToken) tokenCounter++; 
				else tokenCounter=0;
		}
		else										//Caso o token não seja achado, sinal em broadcast é lançado na rede atrás dele
		{
			printf("Searching for Token...\n");
			tokenCounter = 0;
			WhereIsToken();							//Onde está o token?
		}
		if (!wantToken && hasToken)                             //se o cliente não solicitou acesso e este servidor possui o token, ele irá passá-lo
		{
			printf("\nConnecting to Server %s\n", network[nextID]);
			
			hasToken = 0;                                       // Servidor não possui mais o token
			
			printf("###Passing Token…\n");
			
			do{
				ans = passtoken_1(pvoid, client[nextID]);                   //função do próximo servidor, o qual receberá o token
				if (ans == NULL)		// Caso a função anterior retorne NULO, o servidor após seu próximo será o próximo, ou seja,
				{
					if(nextID==N_MACHINES-1) nextID=0; else nextID=nextID+1; // Seu atual próximo não responde, e deve ser substituido
					printf("Server IP: %s, is now linked to Server: %s\n", network[localID], network[nextID]);	
				}															
			}while(ans==NULL);
		}
		sleep(1);
	}while(1);
}

void ConnectThemAll()					//Connecta todos os servidores entre si para realizar o Broadcast
{
	int i;
	for(i=0;i<N_MACHINES;i++)
	{
		if (!(client[i] = clnt_create(network[i], SCPROG, SCVERS, "tcp")))   
		{
			clnt_pcreateerror(network[i]);
			exit(1);
		}
	}
}

void *start_1_svc(int *id, struct svc_req *rqstp)           //função chamada pelo sexto cliente (starter.c), em que inicializa o servidor
{
	localID=*id;
	printf("%d\n", localID);
	if(localID==N_MACHINES-1) nextID=0; else nextID=localID+1;		   //Definição da ID do servidor local e do seu próximo, as quais são passadas por parâmentro
	network[0] = "192.168.1.135";                                   			//Definição dos IPs é feita antes da compilação
	network[1] = "192.168.1.136";                                   
    network[2] = "192.168.1.137";
	//network[3] = "";                                    
	//network[4] = ""; 
	
	printf("\t-Starting Server-\n");
	ConnectThemAll();
	printf("Server IP: %s, Linked to Server: %s\n", network[localID], network[nextID]);

	if (localID==0 || alreadyHave==1)                        //explicado na declaração da variável
	{
		hasToken = 1;
		alreadyHave = 0;
	}
	else
		hasToken = 0;

	wantToken = 0;
	print = 1;
	
	pthread_t th;                                           // Cria a thread VerifyToken definida acima
	
	if(pthread_create(&th, NULL, VerifyToken, NULL)!=0)
	{
		perror("ERROR: Error creating Thread.\n");
		exit(1);
	}

	return;
}

int *passtoken_1_svc(void *pvoid, struct svc_req *rqstp)		//Funcao do Servidor - Passa o Token para o servidor seguinte
{                                                               // Ou recebe o token.
	static int ans = 0;
	alreadyHave = 1;
	hasToken = 1;                                               //Recebe token
	printf("\n***Server got the Token!\n");
	return &ans;
}

int *writerequest_1_svc(void *pvoid, struct svc_req *rqstp)		//Funcao do Cliente - Solicita acesso a sessao critica para escrita
{
	static int ans = 1;
   	wantToken = 1;                                              //Define a flag wantToken, impedindo a thread de passar o token.
	
	if(print) {printf("The local Client requested Access to the Critic Session.\n"); print = 0;}
                                                                //Solução encontrada para imprimir a mensagem assima somente uma vez para
                                                                //cada vez que o cliente queira acessar a SC
	if (hasToken && wantToken)
	{
		print = 1;
		printf("Server has the Token. Allowing Client to Write.\n");
		ans = 0;                                                // Responde 0 se tudo der certo
		return &ans;
	}
	else
	{
		ans = 1;                                                // 1 se tiver algum problema
		return &ans;
	}
}

int *writerelease_1_svc(void *pvoid, struct svc_req *rqstp)		//Funcao do Cliente - Finaliza o acesso na sessao critica
{
	printf("\nClient completed access to file.\n");
	static int ans = 1;
	wantToken = 0;                                              //Define que o cliente não quer mais o token, liberando servidor para passá-lo
	ans = 0;                                                   
	sleep(6);                                                   //Espera por 6 segundos por segurança para não haver conflito.
	return &ans;
}




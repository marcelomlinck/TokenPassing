/*  File: token_server.c         */
/*  Author: Marcelo Melo Linck   */
/*  Version: 1.5                 */
/*  Date: 09/18/2013             */
/*  marcelo.linck@acad.pucrs.br  */

#include <stdio.h>
#include <stdlib.h>
#include <rpc/rpc.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>

#include "token.h"


//Este programa inicializa todos os servidores


main(int argc, char *argv[])  // Passagem dos IPs é feita por parâmetro, a ordem que forem passados será a ID de cada um
{

	void *pvoid;
	CLIENT * client;
    int i;
	int id;
    
	int N_MACHINES; 

	N_MACHINES = atoi(argv[1]);
	
	//Inicia o cliente e tenta conexões
    
    printf("\nStarting Servers...\n");
	
	for(i=2;i<N_MACHINES+2;i++)
    {
        if (!(client = clnt_create(argv[i], SCPROG, SCVERS, "tcp")))   //cria o link
        {
            clnt_pcreateerror(argv[i]);
            exit(1);
        }
		id = (i-2);
		printf("Starting server %s with ID %d \n", argv[i], id);
        start_1(&id, client);  // Chama a função start, função que incializa o servidor
                                // E cria a conecção entre os servidores
    }


	printf("\n\n \033[1m Client Shutting Down!\033[30m \n");
	return 0;
}

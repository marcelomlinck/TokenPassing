/*  File: token_server.c         */
/*  Author: Marcelo Melo Linck   */
/*  Version: 1.3                 */
/*  Date: 09/10/2013             */
/*  marcelo.linck@acad.pucrs.br  */


program SCPROG {                /* value to register the program */
    version SCVERS {            /* version must be assigned a value */
        int WRITEREQUEST(void) = 1;     /*Funcao do Cliente - Solicita acesso a sessao critica para escrita*/
        int PASSTOKEN(void) = 2;  	    /*Funcao do Servidor - Passa o Token para o servidor seguinte*/
		int WRITERELEASE(void) = 3;	    /*Funcao do Cliente - Finaliza o acesso na sessao critica*/
		int DOYOUHAVETOKEN(void) = 4;	    /*Função do Servidor - Caso o Servidor possua o token, retorna 0, caso não, retorna 1*/
		void START(int) = 5;		    /*Funcao do Starter - Inicia Servidores*/
	} = 1;                          /* version value */
} = 0x20000003;                    /* program value */

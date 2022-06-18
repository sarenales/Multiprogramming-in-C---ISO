#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#define error(a){perror(a); exit(1);};

// a 10 dat ls ps sleep20 who

int main(int argc, char *argv[]) { 

	int clockid, timer, i, child, childp, id, cont;
	time_t t0, t1,t2;

	// Empieza a contar el timer.
	t0 = time(NULL);
	time(&t0);
	
	
	
	
	// Control 1: Numero de parametros dentro de lo establecido.
	if (argc < 3 || argc > 5)
		error("Nimero de parametros incorrectos. ");
	
	
	// Control 2: El segundo parametro debe de ser un numero.
	timer = atoi(argv[1]);
	if(timer <= 0 )
		error("No se ha introducido ningun numero en el segundo parametro.");	
	
	
	
	switch(clockid=fork()){
		case -1: 
			error("fork child clock");
			break;
			
			
		case 0: /*hijo*/
			execlp("./clock", "./clock", timer, NULL);
			break;
			
			
		case 1: /* padre*/
			// Encargado de crear los procesos concurrentes.
			for (i = 2; i < argc ; i++)
			{		
				// Se crea otro proceso hijo para luego poder escribir, tras haber terminado.
				if((childp = fork()) == 0){
					printf("Programa numero %d (%s).---------------------\n ", i-1,argv[i]);
					execvp(argv[i], &(argv[i]));
					error("Error execvp. ");
				}
				else if (childp == -1 ) error ("Fork childp");
				
				printf("Proceso subhijo : %d \n", childp);
				
				while(wait(NULL) != childp);
				
				printf("Programa numero %d (%s)terminado.---------------------\n ", i-1,argv[i]);
				
				cont ++; 
				
				kill(childp, SIGKILL);
			}
			while(wait(NULL) != child);
			
			printf("%d comando terminado \n", child);
			printf(" \n");
			printf(" \n");

			
			break;
		
		
		
	}	
	
	
	// while(wait(NULL) != child);	
	
	id = wait(NULL);
	
	
    if(id == clockid)
    { 
		printf("id: %d \n", id);
        printf("El reloj termina antes que los procesos.\n");	
		printf("%d procesos realizados de %d.\n", cont, argc-2);
		time(&t2);
		t2 = time(NULL);
		time(&t2);
		printf("------------------------------------ \n");
		printf("------------------------------------ \n");
		printf("El tiempo total es de %d segundos. \n", t2 - t0); 
		exit(0);
    }else
    { 	
		printf("id: %d \n", id);
		printf("Los hijos terminan antes del tiempo establecido. \n");
		time(&t2);
		t2 = time(NULL);
		time(&t2);
		printf("------------------------------------ \n");
		printf("------------------------------------ \n");
		printf("El tiempo total es de %d segundos. \n", t2 - t0);
		kill(clockid, SIGKILL);
		exit(0);
    }	
	
	return (0);
	
}

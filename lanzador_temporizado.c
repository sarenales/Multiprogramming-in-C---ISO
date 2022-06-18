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

	int clockid, timer, i, childid, childp, id, cont, status;
	time_t t1,t2;

	cont = 0;
	
	// Control 1: Numero de parametros dentro de lo establecido.
	if (argc < 3 || argc > 5)
		error("Nimero de parametros incorrectos. ");
	
	
	// Control 2: El segundo parametro debe de ser un numero.
	timer = atoi(argv[1]);
	if(timer <= 0 )
		error("No se ha introducido ningun numero en el segundo parametro.");	
	
	
	if((clockid = fork())==0){
		execlp("./clock", "./clock", argv[1], NULL);
		perror("execlp clock.");
	}else if ((clockid = fork())==-1) 
		perror("Error fork clockid.");
	
	printf("--clock-process: %d\n ", clockid);
	
	if((childid = fork()) == 0 ){
		for (i = 2; i < argc ; i++){	
		
			if((childp = fork()) == 0){
				printf("Programa numero %d (%s).---------------------\n ", i-1,argv[i]);
				execvp(argv[i], &(argv[i]));
				error("Error execvp. ");
			}
			else if (childp == -1 )
				error ("Fork childp");
				
			while(wait(NULL) != childp);
				
			printf("Programa numero %d (%s)terminado.---------------------\n ", i-1,argv[i]);
			printf("\n");
			
			cont++;
			
			kill(childp, SIGKILL);
		}
			
		// while(wait(NULL) != child);
			
		printf("Commands finished first, stop clock. \n");
	}else if((childid = fork()) == -1 )
		perror("Error fork childid.");
	
	
	t1 = time(NULL);
	
	id = wait(NULL);	
	
    if(id == childid) 	/* child finishes first; stop clock */
    { 
		kill(clockid, SIGKILL);
		wait(NULL);
		printf("child finishes first; stop clock . \n");
		status = 0;
    }else				/* clock finishes first; force child to finish */
    { 	
		kill(childid, SIGKILL);
		wait(NULL);
		printf("%d procesos realizados de %d.\n", cont, argc-2);
		printf("--overtime. Clock finishes first. \n");
		status = 1;
    }	
	
	t2 = time(NULL);
	
	printf("--child time: %d\n", t2-t1);
	
	exit(status);
	
}

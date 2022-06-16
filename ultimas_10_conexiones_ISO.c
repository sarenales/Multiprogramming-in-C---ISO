#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define error(a){perror(a); exit(1);};


main() { 
    int pfd[2], pid;
 
    // se crea la tuberia (PIPE)
    if (pipe(pfd) == -1) error("pipe");

//	printf("parent: pipe created, channels: READ=%d andWRITE=%d\n", pfd[0], pfd[1]);

    // creacion del primer hijo (last)
    switch (pid = fork()) {
 	    case -1: error("fork");
  	    case 0:     /* 1st child: who */
// 		    printf("1st child process created\n");
 		    if (close(1) == -1) error("close");
 		    if (dup(pfd[1]) != 1) error("dup");
 		    close(pfd[0]);
		    close(pfd[1]);
 		    execlp("last","last", "-10", NULL);
		    error("execlp");
    } 
    
//  printf("parent: who(%d) process launched\n", pid);

    // creacion del segundo hijo (grep)
    switch (pid = fork()) {
  	    case -1: error("fork");
   	    case 0: /* 2nd child process: wc -l */
// 	        printf("2nd child process created\n");
 	        if (close(0) == -1) error("close");
	        if (dup(pfd[0]) != 0) error("dup");
 	        close(pfd[0]);
	        close(pfd[1]);
            execlp("grep", "grep", "acaf00", NULL);
 	        error("execlp");
    }
    
//   printf("parent: wc(%d) process launched\n", pid);

    close(pfd[0]);
    close(pfd[1]);
    
    while((pid=wait(NULL))!=-1)
//      printf("parent: %d process finished\n", pid);

    exit(0);
}

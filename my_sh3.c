/* my_sh3.c */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define error(a) {perror(a); exit(1);};
#define BUFSIZE 512
#define MAXARG 10

void get_parameters(char *buf, int n, char *argk[], int m);
void espera(int pid, char arg[]);

main(int argc, char *argv[])
{
   int n, pid, esperapid;
   char buf[BUFSIZE];
   char *arg[MAXARG];

   for (n = 0; n < BUFSIZE; n++) buf[n] = '\0'; // reset buf
   
   write(1, "my_sh3> ", strlen("my_sh3> ")); // write prompt

   while ((n = read(0, buf, BUFSIZE)) > 0){ // read command
      buf[n] = '\n';
      n++;
      get_parameters(buf, n, arg, MAXARG); // parse command
      
      switch (pid = fork()) {
         case -1: error("fork");
            break;
         case 0: /* child */
		    if((esperapid = fork())==0){
                  execvp(arg[1], &(arg[1]));
                  error("exec");
		    }
            else if(esperapid==-1)error("fork");

            printf("%d (%s ...) process created\n", esperapid, arg[1]);

            while(wait(NULL) != esperapid);

            printf("%d (%s ...) process finished\n", esperapid, arg[1]);

            exit(0);
            break;

         default: /* parent */

            if(strcmp(arg[0],"R")==0){
               while(wait(NULL) != pid); // wait for child
            }
            else if(strcmp(arg[0],"S")!=0)error("Bad syntax");

            for (n = 0; n < BUFSIZE; n++) buf[n] = '\0'; // reset buf

            write(1, "my_sh3> ", strlen("my_sh3> ")); // write prompt

            break;
      }
   }
   printf("\n");
}

void get_parameters(char *buf, int n, char *argk[], int m)
{
   int i, j;
   for (i = 0, j = 0; (i < n) && (j < m); j++){
      /* advance blanks */
      while (((buf[i] == ' ') || (buf[i] == '\n')) && (i < n)) i++;

      if (i == n) break;

      argk[j] = &buf[i];

      /* find blank */
      while ((buf[i] != ' ') && (buf[i] != '\n')) i++;

      buf[i++] = '\0';
   }
   argk[j] = NULL;
}



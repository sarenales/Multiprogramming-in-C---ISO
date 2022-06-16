#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int clockid, childid, id, status, i, contMIN, contMED, contMAX;
    int nb_tests, min_time, max_time;
    time_t t0, t1, t2;
    contMIN = 0;
    contMED = 0;
    contMAX = 0;
    t0 = time(NULL);
    time(&t0);

    if (argc < 5)
    {
        printf("Numero de parámetros incorrectos.\n");
        exit(1);
    }

    nb_tests = atoi(argv[1]);
    if (nb_tests <= 0)
    {
        printf("No se ha introducido un número en el segundo parámetro o es cero.\n");
        exit(1);
    }

    min_time = atoi(argv[2]);
    if (min_time <= 0)
    {
        printf("No se ha introducido un número en el tercer parámetro o es cero.\n");
        exit(1);
    }

    max_time = atoi(argv[3]);
    if (max_time <= 0 | max_time <= min_time)
    {
        printf("No se ha introducido un número en el cuarto parámetro o es cero.\n");
        exit(1);
    }

    for (i = 0; i < nb_tests; i++)
    {
        printf("Prueba %d\n", i);
        
        printf("1. Parent-process: %d\n", getpid());

        if ((clockid = fork()) == 0)
            execlp("./clock", "./clock", argv[3], NULL);

        printf("2. Clock-process: %d\n", clockid);

        if ((childid = fork()) == 0)
            execvp(argv[4], &(argv[4]));

        printf("3. Child-process: %d\n", childid);

        t1 = time(NULL);
        time(&t1);
        // printf("Current time of t1: %s", ctime(&t1));
        
        printf("Current time spent %d\n",t1 - t0);

        id = wait(NULL);

        if (id == childid)
        { /* child finishes first; stop clock */
            printf("if, child finishes first; stop clock\n");
            status = 0;
            if (t1 < min_time){
                contMIN++;
				printf("contMIN %d\n", contMIN);	
			}			
            else if (min_time < t1 < max_time){
                contMED++;
				printf("contMED %d\n", contMED);	
			}	
            kill(clockid, SIGKILL);
            wait(NULL);
        }
        else
        { /* clock finishes first; force child to finish */
            printf("else, clock finishes first, force child to finish\n");
            printf("4. Overtime\n");
            status = 1;
            contMAX++;
			printf("contMax %d\n", contMAX);	
            kill(childid, SIGKILL);
            wait(NULL);
        }
        
        // printf("Current time of t2: %s", ctime(&t2));
        time(&t2);
        printf("Current time spent %d\n", t2 - t1);
        printf(" \n");

        // kill(clockid, SIGKILL);
        // wait(NULL);
        // kill(childid, SIGKILL);
        // wait(NULL);
    }
    t2 = time(NULL);
    time(&t2);

    printf("Tiempo total del experimento: %d segundos\n", t2 - t0);
    printf("El programa ha requerido menos del tiempo minimo (%d) %d veces\n", min_time, contMIN);
    printf("El programa ha requerido un intervalo de tiempo entre el tiempo minimo y maximo %d veces\n", contMED);
    printf("El programa ha requerido mas del tiempo maximo (%d) %d veces\n", max_time, contMAX);

    return (0);
}

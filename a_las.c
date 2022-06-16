/***
	PROGRAMA: a_las.c
	DESCRIPCION: Recibe ordenes para ejecutarlas en segundo plano a una hora determinada
	FORMA DE USO: a_las hh:mm.ss linea_de_ordenes
	EJEMPLO: $ a_las 13:55:00 banner HORA DE COMER
***/

#include <stdio.h>
#include <time.h>
#include <signal.h>

#define SEGUNDOS_POR_DIA 86400

/***
	MACRO: hhmmss_a_ss
	DEFINICION: Transforma horas, minutos y segundos a segundos.
***/

#define hhmmss_a_ss(_hh, _mm, _ss)\
								((long)(_hh)*3600+(_mm)*60+(_ss))

/**
	MACRO: ss_s_hhmmss
	DESCRIPCION: TRansforma segundos a horas, minutos y segundos.
**/

#define ss_a_hhmmss(_seg,_hh,_mm,_ss)\
{\
	_hh = (int) ((_seg)/3600);\
	_mm = (int) (((_seg) - (long)_hh*3600) /60);\
	_ss = (int) ((_seg) - (long)_hh*3600 - _mm*60);\
}

/***
	FUNCION: sigalrm_handler
	DESCRIPCION: Rutina de tratamiento de la señar SIGLARM. Su cuerpo es nulo y no hace nada.
***/

void sigalrm_handler(){}

/***
	FUNCION PRINCIPAL.
		Se encarga de analizar los parametros de la linea de ordenes. Si son correctos, crea un proceso hijo y muere. 
		El proceso hijo calcula el valor, en segundos, al que debe inicializar un temporizador
		en tiempo real, lo activa y se pone a esperar. Cuando recibe la señar SIGALRM, la rutina de 
		tratamiento no hace nada, y al volver el proceso a su contexto hace una llmada a "execvp" 
		para ejecutar la linea de ordenes.
***/

main(int argc, char *argv[])
{
	int hh, mm, ss, pid;
	
	// Analisis de los argumentos de la linea de ordenes.
	if(argc < 3){
		fprintf(stderr, "Forma de uso: %s hhmmss linea_de_ordenes\n", argv[0]);
		exit(-1);
	}
	
	if(sscanf(argv[1], "%d:%d:%d", &hh, &mm, &ss) !=3){
		fprintf(stderr, "Formato erroneo: %s\n", argv[1]);
		exit(-1);
	}
	else if(hh < 0 || hh>23 || mm <0 || mm > 59 || ss < 0 || ss >59 ){
		fprintf(stderr, "Hora fuera de rango: %s\n", argv[1]);
		exit(-1);
	}
	
	// Creacion del proceso hijo que se va a ejecutar en segundo plano.
	if((pid = fork() ) == -1){
		perror(argv[0]);
		exit(-1);
	}else if(pid==0){
		//HIJO
		
		long total_segundos;
		time_t t;
		struct tm *tm;
		struct itimerval, temporizador, temporizador_ant;
		
		temporizador.it_value.tv_usec = 0;
		temporizador.it_interval.tv_sec = 0;
		temporizador.it_interval.tv_usec = 0;
		signal(SIGALRM, sigalrm_handler );
		
		// Calculo del valor del temporizador
		total_segundos = hhmmss_a_ss(hh, mm, ss) - hhmmss_a_ss (tm ->tm_hour, tm->tm_min, tm->tm_sec);
		
		if(total_segundos < 0) total_segundos += SEGUNDOS_POR_DIA;
		
		temporizador.it_value.tv_sec = total_segundos;
		
		setitimer (TIMER_REAL, &temporizador, &temporizador_ant);
		
		pause();
		
		// Cuando expira el temporizador, ejecutamos la linea de ordenes.
		execvp(argv[2], &argv[2]);
		
	}else{
		// PADRE
		exit(0);
		
	}

}

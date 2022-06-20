#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define BUFSIZE 60
#define OFFSET 30

void CalcularExtremos(int pos, int fs, int *inicio, int *fin);
long TamanioArchivoRegular(char *NombreFich);
bool obtainfilex(char *nomFich, char *fext);

// void procesamosArchivo(char *archivo);

/* ./utilidad2 DIR1 DIR2 ext Pos */

int main(int argc, char *argv[]){

	DIR *dname1, *dname2; 			// Con un puntero a DIR abriremos el directorio
	struct dirent *ent;				// En *ent habra info sobre el archivo 
	struct stat sbuf, StVar;
	int status, arch, fich, inicio, fin, tamano,i, n ;
	char NombreDir[256];
	char NombreFich[256];
	char cwd[256];
	char buf[61];


	// Control de errores 1: Numero de parametros.
	
	if(argc != 5){
		fprintf(stderr,"Numero de parametros incorrectos!!\n");
		printf("Tiene que ser la siguiente manera");
		printf("./utilidad2 DIR1 DIR2 ext Pos  \n");
		_exit(-1);
	}
	
	dname1 = opendir(argv[1]);
	
	// Control de errores 2: Abrir directorio1.
	if(dname1 == NULL) {
		fprintf(stderr, "Error al abrir el directorio 1 (%s). \n", argv[1]);
		_exit(-1);
	}	
	
	
	dname2 = opendir(argv[2]);
	
	// Control de errores 3: Abrir directorio2.
	if(dname2 == NULL) {
		fprintf(stderr, "Error al abrir el directorio 2 (%s). \n", argv[2]);
		_exit(-1);
	}	
	
	// Control de errores 4: Cuarto argumento tiene que ser un numero.
	n = atoi(argv[4]);
	if(n <= 0 ){
		fprintf(stderr,"No se ha introducido ningun numero en el cuarto parametro. \n");	
		_exit(-1);
	}
	
	getcwd(cwd, sizeof(cwd)-1);
		
	// Buscamos ext en el directorio
	while((ent = readdir(dname1)) != NULL){
		
		// Comprobamos que son FICHEROS 
		if(ent->d_type != DT_DIR) { 
			
			// Mismas EXTENSIONES 
			if(obtainfilex(ent->d_name, argv[3])){		
				
				sprintf(NombreDir,"%s/%s", argv[1], ent->d_name);
				status = stat(NombreDir,&sbuf);
				if(status == -1){
					fprintf(stderr,"Error stat\n");
					_exit(-1);
				}	
				
				// PERMISOS rw en propietario.
				if((sbuf.st_mode & 0600)== 0600){
					printf("name   : %s \n", ent->d_name);
					
					
					sprintf(NombreFich,"./%s/%s", argv[1], ent->d_name);
					
					
					if(TamanioArchivoRegular(NombreFich) < atoi(argv[4])){
						// CASO 1: tamaño fich < Pos bytes , creamos fichero VACIO en dir2
						
						printf("Se ha creado un fichero VACIO en el directorio (%s). \n",argv[2]);
						
						if(chdir(argv[2])==-1){
							fprintf(stderr,"Error posicionarse %s .\n", argv[2]);
							_exit(-1);
						}	
						
						arch = open (ent->d_name ,O_CREAT,0600); 	// Quitamos permisos de escritura y ejecucion para grupo y resto.
						if(arch==-1){
							fprintf(stderr,"Error al abrir el fichero (%s).\n",dname2);
							_exit(-1);
						}
						
						close(arch);
					}else{
						
						//CASO 2: Caso general. 
						
						CalcularExtremos(atoi(argv[4]), TamanioArchivoRegular(NombreFich),  &inicio,  &fin);
						
						if((fich=open(NombreFich, O_RDONLY)) == -1){
								fprintf(stderr,"Error abrir fichero original. \n");
								perror("Error abrir fichero. ");
								_exit(-1);
						}	
						
						
						//Nos posicionamos al principio del fichero del que queremos copiar.
						if(lseek(fich,inicio, SEEK_SET) == -1){
							fprintf(stderr,"Error lseek!!\n");
							_exit(-1);
						}		
					
						// Limpiamos contenido buf
						memset(buf,0x00,sizeof(buf-1));
						
						tamano = read(fich,buf,(fin-inicio+1));
						
						// Creamos canal fichero en dir2
						if(chdir(argv[2])==-1){
							fprintf(stderr,"Error posicionarse en el directorio %s .\n", argv[2]);
							_exit(-1);
						}	
						
						
						arch = open (ent->d_name ,O_CREAT|O_TRUNC|O_WRONLY,0600); 	// Quitamos permisos de escritura y ejecucion para grupo y resto.
						if(arch==-1){
							fprintf(stderr,"Error al abrir el fichero (%s).\n",dname2);
							_exit(-1);
						}

						printf("Se ha creado un fichero en el directorio (%s), con el siguiente contenido. \n",argv[2]);
						for( i=0; i<fin-inicio+1; i++){
							printf("%c",buf[i]);	
						}
						printf("\n");
						
						if(write(arch,buf, fin-inicio+1) == -1){
							fprintf(stderr,"Error al escribir en el archivo (%s).\n",ent->d_name);
							perror("error write.");
							_exit(-1);
						}

						close(arch);	
						
					}	
				}	
			}
		}
		
		// Volvemos al directorio de trabajo.
		if(chdir(cwd)==-1){
			fprintf(stderr,"Error posicionarse al directorio de trabajo %s .\n", cwd);
			_exit(-1);
		}
		
	
	}
	
	
	closedir(dname1);
	closedir(dname2);

	
	return 0;
}

// ----------------------------------------------------------------------------------


bool obtainfilex(char *nomFich, char *fext) 
{ 
    char* ext;
	
    ext = strchr(nomFich,'.');
	
	if(ext != NULL){
		if(strcmp(fext, ext+1) == 0) 
			return true;
	}
	
	return false;
	
	
}


// ---------------------------------------------------------------------------------


long TamanioArchivoRegular(char *NombreFich)
{
    struct stat StVar;
    int Ret;
    off_t FileSize;

    Ret = stat(NombreFich, &StVar);
    if (Ret == -1)
    {
        fprintf(stderr, "No se puede obtener la informacion struct_stat del fichero %s .\n", NombreFich);
        _exit(-1);
    }
	
    FileSize = StVar.st_size;

    return FileSize;
}


// ------------------------------------------------------------------------------------

void CalcularExtremos(int pos, int fs, int *inicio, int *fin)
{
	
	if(pos-OFFSET > 0)
		*inicio = pos-OFFSET;
	else
		*inicio = 0;
	
	if(pos+OFFSET < fs)
		*fin = pos+OFFSET;
	else
		*fin =fs;
}





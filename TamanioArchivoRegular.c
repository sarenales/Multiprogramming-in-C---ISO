#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

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

    printf("File size:  %3ld bytes \n", FileSize);

    return FileSize;
}
// ----------------------------------------------------------------------------------------------------------


main(int argc, char *argv[])
{
    if(argc != 2){
        fprintf(stderr, "Numero de argumentos incorrecto.\n");
        _exit(-1);
    }
    
    TamanioArchivoRegular(argv[1]);
}

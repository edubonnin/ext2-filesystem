// RUBÉN BALLESTEROS JIMÉNEZ, EDUARDO BONNÍN NARVÁEZ, VICENÇ SERVERA FERRER

#include "directorios.h"

#define TAMFILA 100
#define TAMBUFFER (TAMFILA * 1000)

int main(int argc, char const *argv[])
{
    char buffer[TAMBUFFER];
    // memset buffer???
    int total;

    // COMPROBACIÓN SINTAXIS
    if (argc != 3)
    {
        fprintf(stderr, ROJO "Sintaxis: ./mi_ls <disco> </ruta_directorio>" RESET);
        return FALLO;
    }

    // COMPROBACIÓN RUTA CORRECTA
    if (argv[3][0] != '/')
    {
        return FALLO;
    }

    // distinguir directorio/fichero

    if (bmount(argv[1]) == FALLO)
    {
        return FALLO;
    }

    if ((total = mi_dir(argv[3], buffer)) == FALLO)
    {
        return FALLO;
    }

    // IMPRESIÓN RESULTADO DEL COMANDO SI EL TOTAL DE ENTRADAS ES >0
    if (total > 0)
    {
        printf("Total: %d\n", total);
        printf("Tipo    Permisos    mTime           Tamaño  Nombre\n-----------------------"
               "---------------------------------------------------------------------\n%s\n",
               buffer);
    }

    if (bumount(argv[1]) == FALLO)
    {
        return FALLO;
    }

    return EXITO;
}
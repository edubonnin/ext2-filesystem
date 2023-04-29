// RUBÉN BALLESTEROS JIMÉNEZ, EDUARDO BONNÍN NARVÁEZ, VICENÇ SERVERA FERRER

#include "directorios.h"

int main(int argc, char const *argv[])
{
    char buffer[TAMBUFFER], *camino, tipo;
    // memset buffer???
    int total;

    strcpy(camino, argv[3]);

    // COMPROBACIÓN SINTAXIS
    if (argc != 3)
    {
        fprintf(stderr, ROJO "Sintaxis: ./mi_ls <disco> </ruta_directorio>\n" RESET);
        return FALLO;
    }

    // COMPROBACIÓN RUTA CORRECTA
    if (argv[3][0] != '/')
    {
        return FALLO;
    }

    if (camino[strlen(camino) - 1] == '/') // ES UN DIRECTORIO
    {
        tipo = 'd';
    }
    else // ES UN FICHERO
    {
        tipo = 'f';
    }

    if (bmount(argv[1]) == FALLO)
    {
        return FALLO;
    }

    if ((total = mi_dir(argv[3], buffer, tipo)) == FALLO)
    {
        return FALLO;
    }

    // IMPRESIÓN RESULTADO DEL COMANDO SI EL TOTAL DE ENTRADAS ES >0
    if (total > 0)
    {
        if (tipo == 'd')
        {
            printf("Total: %d\n", total);
        }
        printf("Tipo\tPermisos\tmTime\t\t\tTamaño\tNombre\n-----------------------"
               "---------------------------------------------------------------------\n%s\n",
               buffer);
    }

    if (bumount(argv[1]) == FALLO)
    {
        return FALLO;
    }

    return EXITO;
}
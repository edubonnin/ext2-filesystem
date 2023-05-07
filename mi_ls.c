// RUBÉN BALLESTEROS JIMÉNEZ, EDUARDO BONNÍN NARVÁEZ, VICENÇ SERVERA FERRER

#include "directorios.h"

int main(int argc, char const *argv[])
{
    char buffer[TAMBUFFER], tipo;
    // memset buffer???
    int total;

    // COMPROBACIÓN SINTAXIS
    if (argc != 3)
    {
        fprintf(stderr, ROJO "Sintaxis: ./mi_ls <disco> </ruta_directorio>\n" RESET);
        return FALLO;
    }

    // COMPROBACIÓN RUTA CORRECTA
    if (argv[2][0] != '/')
    {
        return FALLO;
    }

    if (argv[2][strlen(argv[3]) - 1] == '/') // ES UN DIRECTORIO
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

    if ((total = mi_dir(argv[2], buffer, tipo)) == FALLO)
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
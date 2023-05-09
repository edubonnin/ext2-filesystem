// // RUBÉN BALLESTEROS JIMÉNEZ, EDUARDO BONNÍN NARVÁEZ, VICENÇ SERVERA FERRER

// #include "directorios.h"

// int main(int argc, char const *argv[])
// {
//     char buffer[TAMBUFFER], tipo;
//     memset(buffer, 0, sizeof(TAMBUFFER));
//     int total;

//     // COMPROBACIÓN SINTAXIS
//     if (argc != 3)
//     {
//         fprintf(stderr, ROJO "Sintaxis: ./mi_ls <disco> </ruta_directorio>\n" RESET);
//         return FALLO;
//     }

//     // COMPROBACIÓN RUTA CORRECTA
//     if (argv[2][0] != '/')
//     {
//         return FALLO;
//     }

//     if (argv[2][strlen(argv[3]) - 1] == '/') // ES UN DIRECTORIO
//     {
//         tipo = 'd';
//     }
//     else // ES UN FICHERO
//     {
//         tipo = 'f';
//     }

//     if (bmount(argv[1]) == FALLO)
//     {
//         return FALLO;
//     }

//     if ((total = mi_dir(argv[2], buffer, tipo)) == FALLO)
//     {
//         return FALLO;
//     }

//     // IMPRESIÓN RESULTADO DEL COMANDO SI EL TOTAL DE ENTRADAS ES >0
//     if (total > 0)
//     {
//         if (tipo == 'd')
//         {
//             printf("Total: %d\n", total);
//         }
//         printf("Tipo\tPermisos\tmTime\t\t\tTamaño\tNombre\n-----------------------"
//                "---------------------------------------------------------------------\n%s\n",
//                buffer);
//     }

//     if (bumount() == FALLO)
//     {
//         return FALLO;
//     }

//     return EXITO;
// }

// Autores: Jorge González Pascual, Luis Clar Fiol
#include "directorios.h"

/**
 * ---------------------------------------------------------------------
 *                          mi_ls.c:
 * ---------------------------------------------------------------------
 *
 * Programa (comando) que lista el contenido de un directorio (nombres
 * de las entradas), llamando a la función mi_dir() de la capa de
 * directorios, que es quien construye el buffer que mostrará mi_ls.c.
 * Indicaremos el total de entradas.
 *
 */

#define DEBUGGER 1

int main(int argc, char const *argv[])
{
    // Comprueba que la sintaxis sea correcta.
    if (argc != 3)
    {
        fprintf(stderr,
                "Error de sintaxis: ./mi_ls <disco></ruta_directorio>\n");
        return FALLO;
    }

    // Monta el disco en el sistema.
    if (bmount(argv[1]) == FALLO)
    {
        fprintf(stderr, "Error de montaje de disco.\n");
        return FALLO;
    }
    char tipo = 'd';
    char buffer[TAMBUFFER];
    memset(buffer, 0, TAMBUFFER);
    int total;
    if ((argv[2][strlen(argv[2]) - 1] != '/')) // si no es un fichero
    {
        tipo = 'f';
    }
    if ((total = mi_dir(argv[2], buffer, tipo)) < 0)
    {
        return FALLO;
    }

#if DEBUGGER
    printf("Total: %d\n", total);
#endif
    if (total > 0)
    {
        printf("Tipo\tModo\tmTime\t\t\tTamaño\tNombre\n");
        printf("----------------------------------------------------------"
               "----------------------\n");
        printf("%s\n", buffer);
    }
    bumount();
    return EXIT_SUCCESS;
}
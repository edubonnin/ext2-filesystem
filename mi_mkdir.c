// RUBÉN BALLESTEROS JIMÉNEZ, EDUARDO BONNÍN NARVÁEZ, VICENÇ SERVERA FERRER

#include "directorios.h"

int main(int argc, char const *argv[])
{
    int permisos = atoi(argv[2]), error;

    // COMPROBACIÓN DE SINTAXIS
    if (argc != 4)
    {
        fprintf(stderr, ROJO "Sintaxis: ./mi_mkdir <nombre_dispositivo> <permisos> </ruta_directorio/>\n" RESET);
        return FALLO;
    }

    if (argv[3][strlen(argv[3]) - 1] != '/') // SI LA RUTA ES UN FICHERO, DEVOLVEMOS ERROR
    {
        fprintf(stderr, ROJO "Error: No es un directorio.\n" RESET);
        return FALLO;
    }

    // COMPROBAMOS QUE PERMISOS SEA UN NÚMERO VÁLIDO
    if (permisos < 0 || permisos > 7)
    {
        fprintf(stderr, ROJO "Error: modo inválido: <<%d>>\n" RESET, permisos);
        return FALLO;
    }

    // MONTAJE DISPOSITIVO VIRTUAL
    if (bmount(argv[1]) == FALLO)
    {
        return FALLO;
    }

    // LLAMADA A mi_creat()
    if ((error = mi_creat(argv[3], permisos)) != EXITO)
    {
        mostrar_error_buscar_entrada(error);
        return FALLO;
    }

    if (bumount() == FALLO)
    {
        return FALLO;
    }

    return EXITO;
}
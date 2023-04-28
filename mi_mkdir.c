// RUBÉN BALLESTEROS JIMÉNEZ, EDUARDO BONNÍN NARVÁEZ, VICENÇ SERVERA FERRER

#include "directorios.h"

int main(int argc, char const *argv[])
{
    int permisos = atoi(argv[2]), error;
    char *ruta;
    strcpy(ruta, argv[3]);

    if (argc != 4)
    {
        fprintf(stderr, ROJO "Sintaxis: ./mi_mkdir <nombre_dispositivo> <permisos> </ruta_directorio/>" RESET);
        return FALLO;
    }

    if (permisos < 0 || permisos > 7)
    {
        fprintf(stderr, ROJO "Error: modo inválido: <<%d>>" RESET, permisos);
        return FALLO;
    }

    if (ruta[strlen(ruta) - 1] != '/')
    {
        fprintf(stderr, ROJO "Error: No es un directorio." RESET, permisos);
        return FALLO;
    }

    if (bmount(argv[1]) == FALLO)
    {
        return FALLO;
    }

    if ((error = mi_creat(ruta, permisos)) != EXITO)
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
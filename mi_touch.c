// RUBÉN BALLESTEROS JIMÉNEZ, EDUARDO BONNÍN NARVÁEZ, VICENÇ SERVERA FERRER

#include "directorios.h"

int main(int argc, char const *argv[])
{
    int permisos = atoi(argv[2]), error;
    char *ruta;
    strcpy(ruta, argv[3]);

    // COMPROBACIÓN DE SINTAXIS
    if (argc != 4)
    {
        fprintf(stderr, ROJO "Sintaxis: ./mi_mkdir <nombre_dispositivo> <permisos> </ruta_directorio/>\n" RESET);
        return FALLO;
    }

    if (ruta[strlen(ruta) - 1] == '/') // SI LA RUTA ES UN DIRECTORIO, DEVOLVEMOS ERROR
    {
        fprintf(stderr, ROJO "Error: No es un fichero.\n" RESET, permisos);
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
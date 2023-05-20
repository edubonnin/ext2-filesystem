// RUBÉN BALLESTEROS JIMÉNEZ, EDUARDO BONNÍN NARVÁEZ, VICENÇ SERVERA FERRER

#include "directorios.h"

int main(int argc, char const *argv[])
{
    int error;

    if (argc != 4)
    {
        fprintf(stderr, ROJO "Sintaxis: ./mi_chmod <nombre_dispositivo> <permisos> </ruta>\n" RESET);
        return FALLO;
    }

    if (atoi(argv[2]) < 0 || atoi(argv[2]) > 7)
    {
        return FALLO;
    }

    if (bmount(argv[1]) == FALLO)
    {
        return FALLO;
    }

    if ((error = mi_chmod(argv[3], atoi(argv[2]))) < 0)
    {
        mostrar_error_buscar_entrada(error);
        return FALLO;
    }

    // SE DESMONTA EL DISPOSITIVO
    if (bumount() == FALLO)
    {
        return FALLO;
    }
}
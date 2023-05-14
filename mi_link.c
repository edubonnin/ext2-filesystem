// RUBÉN BALLESTEROS JIMÉNEZ, EDUARDO BONNÍN NARVÁEZ, VICENÇ SERVERA FERRER

#include "directorios.h"

int main(int argc, char const *argv[])
{
    // COMPROBACIÓN SINTAXIS
    if (argc != 4)
    {
        fprintf(stderr, ROJO "Sintaxis: ./mi_link disco /ruta_fichero_original /ruta_enlace\n" RESET);
        return FALLO;
    }

    // COMPROBAR QUE LAS RUTAS NO SE TRATEN DE DIRECTORIOS
    if (argv[2][strlen(argv[2]) - 1] == '/' || argv[3][strlen(argv[3]) - 1] == '/')
    {
        fprintf(stderr, ROJO "La sintaxis de las rutas no corresponden a un fichero\n" RESET);
        return FALLO;
    }

    // MONTAJE DISPOSITIVO VIRTUAL
    if (bmount(argv[1]) == FALLO)
    {
        return FALLO;
    }

    // LLAMADA A mi_link()
    if (mi_link(argv[2], argv[3]) == FALLO)
    {
        return FALLO;
    }

    if (bumount() == FALLO)
    {
        return FALLO;
    }

    return EXITO;
}
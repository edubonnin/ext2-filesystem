// RUBÉN BALLESTEROS JIMÉNEZ, EDUARDO BONNÍN NARVÁEZ, VICENÇ SERVERA FERRER

#include "directorios.h"

int main(int argc, char const *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, ROJO "Sintaxis: ./mi_chmod <disco> <permisos> </ruta>\n" RESET);
        return FALLO;
    }

    if (argv[2] < 0 || argv[2] > 7)
    {
        return FALLO;
    }

    mi_chmod(argv[3], atoi(argv[2]));
}

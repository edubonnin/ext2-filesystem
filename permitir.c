// RUBÉN BALLESTEROS JIMÉNEZ, EDUARDO BONNÍN NARVÁEZ, VICENÇ SERVERA FERRER

#include "ficheros.h"

int main(int argc, char *argv[])
{
    int ninodo = atoi(argv[2]);
    int permisos = atoi(argv[3]);

    // VALIDACION
    if (argc != 4)
    {
        fprintf(stderr, "Sintaxis: permitir <nombre_dispositivo> <ninodo> <permisos>\n");
        return FALLO;
    }

    // MONTAR DISPOSITIVO VIRTUAL
    if (bmount(argv[1]) == FALLO)
    {
        fprintf(stderr, "permitir.c: Error montando el dispositivo en el sistema.\n");
        return FALLO;
    }

    if (mi_chmod_f(ninodo, permisos))
    {
        fprintf(stderr, "permitir.c: Error con la modificacion de los permisos.\n");
        return FALLO;
    }

    // DESMONTAMOS DISPOSITIVO VIRTUAL
    if (bumount() == FALLO)
    {
        fprintf(stderr, "permitir.c: Error al desmonta el dispositivo virtual.\n");
        return FALLO;
    }

    return EXITO;
}
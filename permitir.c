#include "ficheros.h"

/**
 * ---------------------------------------------------------------------
 *                          permitir.c:
 * ---------------------------------------------------------------------
 *
 * Programa externo ficticio sólo para probar, temporalmente,
 * la funcionalidad de cambio de permisos.
 *
 * Llamada a mi_chmod_f() con los argumentos recibidos, convertidos a entero
 *
 */

int main(int argc, char *argv[])
{
    int ninodo = atoi(argv[2]);
    int permisos = atoi(argv[3]);

    // Validación
    if (argc != 4)
    {
        fprintf(stderr, "Sintaxis: permitir <nombre_dispositivo> <ninodo> <permisos>\n");
        return FALLO;
    }

    // Monta el dispositivo virtual
    if (bmount(argv[1]) == FALLO)
    {
        fprintf(stderr, "permitir.c: Error montando el dispositivo en el sistema.\n");
        return FALLO;
    }

    // Llamada a mi_chmod_f() con los argumentos recibidos, convertidos a entero
    if (mi_chmod_f(ninodo, permisos))
    {
        fprintf(stderr, "permitir.c: Error con la modificacion de los permisos.\n");
        return FALLO;
    }

    // Desmontar el dispositivo virtual
    if (bumount() == FALLO)
    {
        fprintf(stderr, "permitir.c: Error al desmonta el dispositivo virtual.\n");
        return FALLO;
    }

    return EXITO;
}
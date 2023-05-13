#include "directorios.h"

int main(int argc, char const *argv[])
{
    // COMPROBACIÓN SINTAXIS
    if (argc != 3)
    {
        fprintf(stderr, ROJO "Sintaxis: ./mi_rm disco /ruta\n" RESET);
        return FALLO;
    }

    // MONTAJE DISPOSITIVO VIRTUAL
    if (bmount(argv[1]) == FALLO)
    {
        return FALLO;
    }

    // LLAMADA A mi_unlink()
    if (mi_unlink(argv[2]) == FALLO)
    {
        return FALLO;
    }

    if (bumount() == FALLO)
    {
        return FALLO;
    }

    return EXITO;
}
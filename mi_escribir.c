#include "directorios.h"

int main(int argc, char const *argv[])
{
    if (argc != 5)
    {
        fprintf(stderr, ROJO "Sintaxis: ./mi_escribir <disco> </ruta_fichero> <texto> <offset>\n" RESET);
        return FALLO;
    }

    // LLAMA bmount PARA MONTAR EL DISPOSITIVO VIRTUAL
    if (bmount(argv[1]) < 0)
    {
        fprintf(stderr, "mi_escribir.c: Error al montar el dispositivo virtual.\n");
        return FALLO;
    }

    printf("longitud texto: %ld\n", strlen(argv[3]));

    int bytesEscritos = mi_write(argv[2], argv[3], atoi(argv[4]), strlen(argv[3]));
    if (bytesEscritos < 0)
    {
        bytesEscritos = 0;
    }

    printf("Bytes escritos: %d\n", bytesEscritos);

    // DESMONTA EL DISPOSITIVO VIRTUAL
    if (bumount() < 0)
    {
        fprintf(stderr, "mi_escribir.c: Error al desmonta el dispositivo virtual.\n");
        return FALLO;
    }
    return EXITO;
}
// RUBÉN BALLESTEROS JIMÉNEZ, EDUARDO BONNÍN NARVÁEZ, VICENÇ SERVERA FERRER

#include "directorios.h"

int main(int argc, char const *argv[])
{
    struct STAT stat;
    int ninodo;
    if (argc != 3)
    {
        fprintf(stderr, ROJO "Error de sintaxis\n" RESET);
        return FALLO;
    }
    if (bmount(argv[1]) == FALLO)
    {
        fprintf(stderr, ROJO "Error en el montaje del disco\n" RESET);
        return FALLO;
    }
    if ((ninodo = mi_stat(argv[2], &stat)) < 0)
    {
        mostrar_error_buscar_entrada(ninodo);
        return FALLO;
    }

    struct tm *ts;
    char atime[80];
    char mtime[80];
    char ctime[80];

    ts = localtime(&stat.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&stat.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&stat.ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);

    // Motramos valores
    printf("Nº de inodo: %d\n", ninodo);
    printf("tipo: %c\n", stat.tipo);
    printf("permisos: %d\n", stat.permisos);
    printf("atime: %s\n", atime);
    printf("ctime: %s\n", ctime);
    printf("mtime: %s\n", mtime);
    printf("nlinks: %d\n", stat.nlinks);
    printf("tamEnBytesLog: %d\n", stat.tamEnBytesLog);
    printf("numBloquesOcupados: %d\n\n", stat.numBloquesOcupados);

    if (bumount() == FALLO)
    {
        return FALLO;
    }
}
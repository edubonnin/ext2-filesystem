#include "directorios.h"
int main(int argc, char const *argv[])
{
    struct STAT stat;
    int ninodo;
    if (argc != 3)
    {
        fprintf(stderr, ROJO "Error de sintaxis" RESET);
        return FALLO;
    }
    if (bmount(argv[1]) == FALLO)
    {
        fprintf(stderr, ROJO "Error en el montaje del disco" RESET);
        return FALLO;
    }
    if (ninodo = mi_stat(argv[2], &stat)<0)
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
    printf("Nº de inodo: %d\n", ninodo);
    printf("tipo: %c\n", stat.tipo);
    printf("permisos: %d\n", stat.permisos);
    printf("atime: %s\n", stat.atime);
    printf("ctime: %s\n", stat.ctime);
    printf("mtime: %s\n", stat.mtime);
    printf("nlinks: %d\n", stat.nlinks);
    printf("tamEnBytesLog: %d\n", stat.tamEnBytesLog);
    printf("numBloquesOcupados: %d\n", stat.numBloquesOcupados);

    bumount();
}
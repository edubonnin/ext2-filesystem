#include "ficheros.h"

/**
 * ---------------------------------------------------------------------
 *                          truncar.c:
 * ---------------------------------------------------------------------
 *
 * Programa externo ficticio sólo para probar, temporalmente,
 * la funcionalidad d borrado parcial o total del contenido de un archivo
 * del dispositivo virtual.
 *
 */

int main(int argc, char *argv[])
{

    // VALIDACIÓN DE SINTAXIS
    if (argc != 4)
    {
        fprintf(stderr, ROJO "Sintaxis errónea: ./truncar <nombre_dispositivo> <ninodo> <nbytes>\n" RESET);
        return FALLO;
    }

    // OBTENCIÓN PARÁMETROS
    int ninodo = atoi(argv[2]);
    int nbytes = atoi(argv[3]);

    // MONTAMOS DISPOSITIVO VIRTUAL
    if (bmount(argv[1]) == FALLO)
    {
        fprintf(stderr, "truncar.c: Error al montar el dispositivo virtual.\n");
        return FALLO;
    }

    // LLAMADA DE mi_truncar_f() DESDE liberar_inodo() --> SE LIBERA EL INODO
    if (nbytes == 0)
    {
        if (liberar_inodo(ninodo) == -1)
        {
            fprintf(stderr, "truncar.c: Error al librar el inodo %i.\n", ninodo);
            return FALLO;
        }
    }
    else
    {
        mi_truncar_f(ninodo, nbytes);
    }

    // INODO LIBERADO
    struct STAT p_stat;
    if (mi_stat_f(ninodo, &p_stat))
    {
        fprintf(stderr, "truncar.c: Error mi_stat_f()\n");
        return FALLO;
    }

    // CAMBIO FORMATO FECHA
    struct tm *ts;
    char atime[80];
    char mtime[80];
    char ctime[80];
    ts = localtime(&p_stat.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&p_stat.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&p_stat.ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);

    // DATOS INODO ESCRITO
    printf("\nDATOS INODO %d:\n", ninodo);
    printf("tipo=%c\n", p_stat.tipo);
    printf("permisos=%d\n", p_stat.permisos);
    printf("atime: %s\n", atime);
    printf("ctime: %s\n", ctime);
    printf("mtime: %s\n", mtime);
    printf("nLinks= %d\n", p_stat.nlinks);
    printf("tamEnBytesLog= %d\n", p_stat.tamEnBytesLog);
    printf("numBloquesOcupados= %d\n", p_stat.numBloquesOcupados);

    // DESMONTAMOS EL DISPOSITIVO VITRUAL
    if (bumount() == FALLO)
    {
        fprintf(stderr, "truncar.c: Error al desmontar el dispositivo virtual.\n");
        return FALLO;
    }
    return EXITO;
}
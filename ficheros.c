#include "ficheros.h"

// después del mi_write_f() borrar el buffer

/**
 * mi_write_f()
 * DESCRIPCION:
 * INPUTS:
 * OUTPUTS:
 */
int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes)
{
}

/**
 * mi_read_f()
 * DESCRIPCION:
 * INPUTS:
 * OUTPUTS:
 */
int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes)
{
}

/**
 * mi_stat_f()
 * DESCRIPCION:
 * INPUTS: número de inodo y struct STAT pasado por referencia
 * OUTPUTS:
 */
int mi_stat_f(unsigned int ninodo, struct STAT *p_stat)
{
    struct inodo *inodo;
    leer_inodo(ninodo, inodo);

    //LENAMOS P_STAT CON LOS VALORES CORRESPONDIENTES
    p_stat->tipo = inodo->tipo;
    p_stat->permisos = inodo->permisos;
    p_stat->atime = inodo->atime;
    p_stat->mtime = inodo->mtime;
    p_stat->ctime = inodo->ctime;
    p_stat->nlinks = inodo->nlinks;
    p_stat->tamEnBytesLog = inodo->tamEnBytesLog;
    p_stat->numBloquesOcupados = inodo->numBloquesOcupados;
}

/**
 * mi_chmod_f()
 * DESCRIPCION:
 * INPUTS: número de inodo y permisos
 * OUTPUTS:
 */
int mi_chmod_f(unsigned int ninodo, unsigned char permisos)
{
    //LEEMOS EL INODO CORRESPONDIENTE
    struct inodo *inodo;
    leer_inodo(ninodo, inodo);

    //CAMBIAMOS LOS PERMISOS Y ACTUALIZAMOS EL CTIME
    inodo->permisos = permisos;
    inodo->ctime = time(NULL);

    //ESCRIBIMOS EL INODO
    escribir_inodo(ninodo, inodo);
}
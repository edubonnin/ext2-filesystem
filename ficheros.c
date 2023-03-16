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
}

/**
 * mi_chmod_f()
 * DESCRIPCION:
 * INPUTS: número de inodo y permisos
 * OUTPUTS:
 */
int mi_chmod_f(unsigned int ninodo, unsigned char permisos)
{
}
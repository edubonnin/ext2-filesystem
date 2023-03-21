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
    struct inodo inodo;
    leer_inodo(ninodo, &inodo);

    int primerBL;
    int ultimoBL;
    int desp1;
    int desp2;

    if ((inodo.permisos & 2) != 2)
    {
        return FALLO;
    }

    primerBL = offset / BLOCKSIZE;
    ultimoBL = (offset + nbytes - 1) / BLOCKSIZE;
    desp1 = offset % BLOCKSIZE;
    desp2 = (offset + nbytes - 1) % BLOCKSIZE;

    unsigned char buf_bloque[BLOCKSIZE]; // buffer para leer el bloque fisico
    int nbfisico = traducir_bloque_inodo(&inodo, primerBL, '1');

    if (primerBL == ultimoBL) // CASO EN EL QUE TODOS LOS BYTES QUEPAN EN UN BLOQUE
    {
        bread(nbfisico, buf_bloque);                      // leemos el bloque físico
        memcpy(buf_bloque + desp1, buf_original, nbytes); // copiamos el buffer original en buf_bloque
        bwrite(nbfisico, buf_bloque);                     // escribimos el bloque físico
    }
    else // SE TIENE QUE ESCRIBIR MÁS DE UN BLOQUE
    {
        // 1. PRIMER BLOQUE LÓGICO
        bread(nbfisico, buf_bloque);
        memcpy(buf_bloque + desp1, buf_original, BLOCKSIZE - desp1);
        bwrite(nbfisico, buf_bloque);

        // 2. BLOQUES LÓGICOS INTERMEDIOS
        for (int i = primerBL + 1; i < ultimoBL; i++)
        {
            nbfisico = traducir_bloque_inodo(&inodo, i, '1');
            bwrite(nbfisico, buf_original + (BLOCKSIZE - desp1) + (i - primerBL - 1) * BLOCKSIZE);
        }

        // 3. ÚLTIMO BLOQUE
        nbfisico = traducir_bloque_inodo(&inodo, ultimoBL, '1');
        bread(nbfisico, buf_bloque);
        memcpy(buf_bloque, buf_original + (nbytes - (desp2 + 1)), desp2 + 1);
        bwrite(nbfisico, buf_bloque);
    }

    // leer_inodo(ninodo,&inodo); ??????!!!!!!
    if ((offset + nbytes) > inodo.tamEnBytesLog)
    {
        inodo.tamEnBytesLog = offset + nbytes;
        inodo.ctime = time(NULL);
    }
    inodo.mtime = time(NULL);
    escribir_inodo(ninodo, &inodo);
}

/**
 * mi_read_f()
 * DESCRIPCION:
 * INPUTS:
 * OUTPUTS:
 */
int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes)
{
    int leidos;

    struct inodo inodo;
    leer_inodo(ninodo, &inodo);

    if ((inodo.permisos & 4) != 4)
    {
        return FALLO;
    }

    if (offset >= inodo.tamEnBytesLog)
    {
        leidos = 0; // no podemos leer nada
        return leidos;
    }

    if ((offset + nbytes) >= inodo.tamEnBytesLog) // pretende leer más alla de EOF
    {
        nbytes = inodo.tamEnBytesLog - offset; // leemos sólo los bytes que podemos desde el offset hasta EOF
    }
}

/**
 * mi_stat_f()
 * DESCRIPCION:
 * INPUTS: número de inodo y struct STAT pasado por referencia
 * OUTPUTS:
 */
int mi_stat_f(unsigned int ninodo, struct STAT *p_stat)
{
    struct inodo inodo;
    leer_inodo(ninodo, &inodo);

    // LLENAMOS P_STAT CON LOS VALORES CORRESPONDIENTES
    p_stat->tipo = inodo.tipo;
    p_stat->permisos = inodo.permisos;
    p_stat->atime = inodo.atime;
    p_stat->mtime = inodo.mtime;
    p_stat->ctime = inodo.ctime;
    p_stat->nlinks = inodo.nlinks;
    p_stat->tamEnBytesLog = inodo.tamEnBytesLog;
    p_stat->numBloquesOcupados = inodo.numBloquesOcupados;
}

/**
 * mi_chmod_f()
 * DESCRIPCION:
 * INPUTS: número de inodo y permisos
 * OUTPUTS:
 */
int mi_chmod_f(unsigned int ninodo, unsigned char permisos)
{
    // LEEMOS EL INODO CORRESPONDIENTE
    struct inodo inodo;
    leer_inodo(ninodo, &inodo);

    // CAMBIAMOS LOS PERMISOS Y ACTUALIZAMOS EL CTIME
    inodo.permisos = permisos;
    inodo.ctime = time(NULL);

    // ESCRIBIMOS EL INODO
    escribir_inodo(ninodo, &inodo);
}
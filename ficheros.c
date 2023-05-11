// RUBÉN BALLESTEROS JIMÉNEZ, EDUARDO BONNÍN NARVÁEZ, VICENÇ SERVERA FERRER

#include "ficheros.h"

int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes)
{
    struct inodo inodo;
    leer_inodo(ninodo, &inodo);

    int primerBL;
    int ultimoBL;
    int desp1;
    int desp2;
    int bytesEscritos = 0;
    // comprobación permsos de escritura
    if ((inodo.permisos & 2) != 2)
    {
        fprintf(stderr, ROJO "No hay permisos de escritura\n" RESET);
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
        if (bread(nbfisico, buf_bloque) == FALLO)
        {
            return FALLO;
        }                                                 // leemos el bloque físico
        memcpy(buf_bloque + desp1, buf_original, nbytes); // copiamos el buffer original en buf_bloque
        if (bwrite(nbfisico, buf_bloque) == FALLO)
        {
            return FALLO;
        } // escribimos el bloque físico
        bytesEscritos += nbytes;
    }
    else // SE TIENE QUE ESCRIBIR MÁS DE UN BLOQUE
    {
        // 1. PRIMER BLOQUE LÓGICO
        if (bread(nbfisico, buf_bloque) == FALLO)
        {
            return FALLO;
        }
        memcpy(buf_bloque + desp1, buf_original, BLOCKSIZE - desp1);
        if (bwrite(nbfisico, buf_bloque) == FALLO)
        {
            return FALLO;
        }
        bytesEscritos += (BLOCKSIZE - desp1);

        // 2. BLOQUES LÓGICOS INTERMEDIOS
        for (int i = primerBL + 1; i < ultimoBL; i++)
        {
            nbfisico = traducir_bloque_inodo(&inodo, i, '1');
            if (bwrite(nbfisico, buf_original + (BLOCKSIZE - desp1) + (i - primerBL - 1) * BLOCKSIZE) == FALLO)
            {
                return FALLO;
            }
            bytesEscritos += BLOCKSIZE;
        }

        // 3. ÚLTIMO BLOQUE
        nbfisico = traducir_bloque_inodo(&inodo, ultimoBL, '1');
        if (bread(nbfisico, buf_bloque) == FALLO)
        {
            return FALLO;
        }
        memcpy(buf_bloque, buf_original + (nbytes - (desp2 + 1)), desp2 + 1);
        if (bwrite(nbfisico, buf_bloque) == FALLO)
        {
            return FALLO;
        }
        bytesEscritos += (desp2 + 1);
    }

    if ((offset + nbytes) > inodo.tamEnBytesLog)
    {
        inodo.tamEnBytesLog = offset + nbytes;
        inodo.ctime = time(NULL);
    }
    inodo.mtime = time(NULL);
    escribir_inodo(ninodo, &inodo);

    return bytesEscritos;
}

int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes)
{
    // Declaraciones
    int primerBL;
    int ultimoBL;
    int desp1;
    int desp2;
    int nbfisico;
    int bytesleidos = 0;

    char unsigned buf_bloque[BLOCKSIZE];
    struct inodo inodo;

    if (leer_inodo(ninodo, &inodo) == FALLO)
    {
        return bytesleidos;
    }

    // COMPROBAMOS QUE TENEMOS PERMISOS PARA LEER
    if ((inodo.permisos & 4) != 4)
    {
        fprintf(stderr, ROJO "No hay permisos de lectura\n" RESET);
        return bytesleidos;
    }

    // COMPROBAMOS QUE NO QUEREMOS LEER MÁS ALLA DEL eof
    if (offset >= inodo.tamEnBytesLog)
    {
        return bytesleidos;
    }

    // COMPROBAMOS QUE NO QUEREMOS LEER MÁS ALLA DEL eof AÑADIENDO LOS BYTES
    if ((offset + nbytes) >= inodo.tamEnBytesLog)
    {
        // SE TENDRÁN QUE LEER LOS BYTES QUE SEA POSSIBLE LEER
        nbytes = inodo.tamEnBytesLog - offset;
    }

    primerBL = offset / BLOCKSIZE;
    ultimoBL = (offset + nbytes - 1) / BLOCKSIZE;

    desp1 = offset % BLOCKSIZE;
    desp2 = (offset + nbytes - 1) % BLOCKSIZE;

    // OBTENCIÓN DEL NUMERO DE BLOQUE FÍSICO
    nbfisico = traducir_bloque_inodo(&inodo, primerBL, '0');
    // PRIMER BLOQUE FISICO ES IGUAL AL ULTIMO
    if (primerBL == ultimoBL)
    {
        if (nbfisico != -1)
        {
            // LECTURA BLOQUE FISICO
            if (bread(nbfisico, buf_bloque) == FALLO)
            {
                return FALLO;
            }
            memcpy(buf_original, buf_bloque + desp1, nbytes);
        }
        bytesleidos = nbytes;
    }
    // Caso en el que la lectura ocupa mas de un bloque fisico
    else if (primerBL < ultimoBL)
    {

        if (nbfisico != -1)
        {
            // LECTURA BLOQUE FISICO
            if (bread(nbfisico, buf_bloque) == FALLO)
            {
                return FALLO;
            }
            memcpy(buf_original, buf_bloque + desp1, BLOCKSIZE - desp1);
        }

        bytesleidos = BLOCKSIZE - desp1;

        for (int i = primerBL + 1; i < ultimoBL; i++)
        {
            // BLOQUES INTERMEDIOS
            nbfisico = traducir_bloque_inodo(&inodo, i, '0');
            if (nbfisico != -1)
            {
                // LECTURA BLOQUE FISICO
                if (bread(nbfisico, buf_bloque) == FALLO)
                {
                    return FALLO;
                }
                memcpy(buf_original + (BLOCKSIZE - desp1) + (i - primerBL - 1) * BLOCKSIZE, buf_bloque, BLOCKSIZE);
            }
            bytesleidos += BLOCKSIZE;
        }

        // OBTENCIÓN DEL NUMERO DE BLOQUE FÍSICO
        nbfisico = traducir_bloque_inodo(&inodo, ultimoBL, '0');
        if (nbfisico != -1)
        {
            // LECTURA DEL BLOQUE FISICO
            if (bread(nbfisico, buf_bloque) == FALLO)
            {
                return FALLO;
            }
            // CALCULO BYTES LOGICOS
            memcpy(buf_original + (nbytes - desp2 - 1), buf_bloque, desp2 + 1);
        }
        bytesleidos += desp2 + 1;
    }

    // LECTURA DEL INODO
    if (leer_inodo(ninodo, &inodo) == FALLO)
    {
        return FALLO;
    }

    // ACTUALIZACIÓN DEL TIME
    inodo.atime = time(NULL);

    // ESCRITURA DEL INODO
    if (escribir_inodo(ninodo, &inodo) == FALLO)
    {
        return FALLO;
    }

    // Comprobar que no haya errores de escritura
    if (nbytes == bytesleidos)
    {
        return bytesleidos;
    }
    else
    {
        return FALLO;
    }
}

int mi_stat_f(unsigned int ninodo, struct STAT *p_stat)
{
    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo) == FALLO)
    {
        return FALLO;
    }

    // LLENAMOS P_STAT CON LOS VALORES CORRESPONDIENTES
    p_stat->tipo = inodo.tipo;
    p_stat->permisos = inodo.permisos;
    p_stat->atime = inodo.atime;
    p_stat->mtime = inodo.mtime;
    p_stat->ctime = inodo.ctime;
    p_stat->nlinks = inodo.nlinks;
    p_stat->tamEnBytesLog = inodo.tamEnBytesLog;
    p_stat->numBloquesOcupados = inodo.numBloquesOcupados;
    return EXITO;
}

int mi_chmod_f(unsigned int ninodo, unsigned char permisos)
{
    // LEEMOS EL INODO CORRESPONDIENTE
    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo) == FALLO)
    {
        return FALLO;
    }

    // CAMBIAMOS LOS PERMISOS Y ACTUALIZAMOS EL CTIME
    inodo.permisos = permisos;
    inodo.ctime = time(NULL);

    // ESCRIBIMOS EL INODO
    if (escribir_inodo(ninodo, &inodo) == FALLO)
    {
        return FALLO;
    }
    return EXITO;
}

int mi_truncar_f(unsigned int ninodo, unsigned int nbytes)
{

    int primerBL;
    int liberados; // NÚMERO DE BLOQUES LIBERADOS

    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo) == FALLO)
    {
        return FALLO;
    }

    // Comprobar que el inodo tenga permisos de escritura
    if ((inodo.permisos & 2) != 2)
    {
        return FALLO;
    }

    // COMPROBAMOS QUE NO QUEREMOS LEER MÁS ALLA DEL EOF
    if (nbytes >= inodo.tamEnBytesLog)
    {
        return FALLO;
    }

    // SABER PRIMER BLOQUE LÓGICO A LIBERAR
    if (nbytes % BLOCKSIZE == 0)
    {
        primerBL = nbytes / BLOCKSIZE;
    }
    else
    {
        primerBL = nbytes / BLOCKSIZE + 1;
    }

    liberados = liberar_bloques_inodo(primerBL, &inodo);

    inodo.mtime = time(NULL);
    inodo.ctime = time(NULL);
    inodo.tamEnBytesLog = nbytes;
    inodo.numBloquesOcupados -= liberados;

    escribir_inodo(ninodo, &inodo);

    return liberados;
}
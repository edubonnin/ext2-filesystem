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
    int bytesEscritos = 0;

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
        bytesEscritos += nbytes;
    }
    else // SE TIENE QUE ESCRIBIR MÁS DE UN BLOQUE
    {
        // 1. PRIMER BLOQUE LÓGICO
        bread(nbfisico, buf_bloque);
        memcpy(buf_bloque + desp1, buf_original, BLOCKSIZE - desp1);
        bwrite(nbfisico, buf_bloque);
        bytesEscritos += (BLOCKSIZE - desp1);

        // 2. BLOQUES LÓGICOS INTERMEDIOS
        for (int i = primerBL + 1; i < ultimoBL; i++)
        {
            nbfisico = traducir_bloque_inodo(&inodo, i, '1');
            bwrite(nbfisico, buf_original + (BLOCKSIZE - desp1) + (i - primerBL - 1) * BLOCKSIZE);
            bytesEscritos += BLOCKSIZE;
        }

        // 3. ÚLTIMO BLOQUE
        nbfisico = traducir_bloque_inodo(&inodo, ultimoBL, '1');
        bread(nbfisico, buf_bloque);
        memcpy(buf_bloque, buf_original + (nbytes - (desp2 + 1)), desp2 + 1);
        bwrite(nbfisico, buf_bloque);
        bytesEscritos += (desp2 + 1);
    }

    // leer_inodo(ninodo,&inodo); ??????!!!!!!
    if ((offset + nbytes) > inodo.tamEnBytesLog)
    {
        inodo.tamEnBytesLog = offset + nbytes;
        inodo.ctime = time(NULL);
    }
    inodo.mtime = time(NULL);
    escribir_inodo(ninodo, &inodo);

    return bytesEscritos;
}

// /**
//  * mi_read_f()
//  * DESCRIPCION:
//  * INPUTS:
//  * OUTPUTS:
//  */
// int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes)
// {
//     struct inodo inodo;
//     leer_inodo(ninodo, &inodo);

//     int leidos;
//     unsigned int primerBL, ultimoBL;
//     int desp1, desp2, nbfisico;
//     unsigned char buf[BLOCKSIZE];
//     int bytesLeidos = 0;

//     if ((inodo.permisos & 4) != 4)
//     {
//         return FALLO;
//     }

//     // La función no puede leer más allá del tamaño en bytes lógicos del inodo
//     if (offset >= inodo.tamEnBytesLog)
//     {
//         leidos = 0; // no podemos leer nada
//         return leidos;
//     }
//     if ((offset + nbytes) >= inodo.tamEnBytesLog) // pretende leer más alla de EOF
//     {
//         nbytes = inodo.tamEnBytesLog - offset; // leemos sólo los bytes que podemos desde el offset hasta EOF
//     }

//     primerBL = offset / BLOCKSIZE;
//     ultimoBL = (offset + nbytes) - 1 / BLOCKSIZE;
//     desp1 = offset % BLOCKSIZE;
//     desp2 = (offset + nbytes - 1) % BLOCKSIZE;
//     nbfisico = traducir_bloque_inodo(&inodo, primerBL, '0');

//     if (primerBL == ultimoBL)
//     {
//         if (nbfisico != -1)
//         {
//             // Leemos el bloque fisico del disco
//             if (bread(nbfisico, buf) == FALLO)
//             {
//                 fprintf(stderr, "Error en mi_read_f()\n");
//                 return FALLO;
//             }
//             memcpy(buf_original, buf + desp1, nbytes);
//         }
//         bytesLeidos += nbytes;
//     }
//     else
//     {
//         // Parte 1
//         if (nbfisico != -1)
//         {
//             // Leemos el bloque fisico del disco
//             if (bread(nbfisico, buf) == FALLO)
//             {
//                 fprintf(stderr, "Error en mi_read_f()\n");
//                 return FALLO;
//             }
//             memcpy(buf_original, buf + desp1, BLOCKSIZE - desp1);
//         }

//         bytesLeidos = BLOCKSIZE - desp1;

//         // Parte 2
//         for (int i = primerBL + 1; i < ultimoBL; i++)
//         {
//             // bloques intermedios
//             nbfisico = traducir_bloque_inodo(&inodo, i, '0');
//             if (nbfisico != -1)
//             {
//                 // Leemos el bloque fisico del disco
//                 if (bread(nbfisico, buf) == FALLO)
//                 {
//                     fprintf(stderr, "Error en mi_read_f()\n");
//                     return FALLO;
//                 }
//                 memcpy(buf_original + (BLOCKSIZE - desp1) + (i - primerBL - 1) * BLOCKSIZE, buf, BLOCKSIZE);
//             }

//             bytesLeidos += BLOCKSIZE;
//         }

//         // Parte 3
//         nbfisico = traducir_bloque_inodo(&inodo, ultimoBL, '0');
//         if (nbfisico != -1)
//         {
//             // Leemos el bloque fisico del disco
//             if (bread(nbfisico, buf) == FALLO)
//             {
//                 fprintf(stderr, "Error en mi_read_f()\n");
//                 return FALLO;
//             }
//             // Calculamos el byte lógico del último bloque hasta donde hemos de leer
//             memcpy(buf_original + (nbytes - desp2 - 1), buf, desp2 + 1);
//         }

//         bytesLeidos += desp2 + 1;
//     }

//     // Leer el inodo actualizado.
//     if (leer_inodo(ninodo, &inodo) == FALLO)
//     {
//         fprintf(stderr, "Error en leer_inodo())\n");
//         return FALLO;
//     }

//     // Actualizar la metainformación
//     inodo.atime = time(NULL);

//     // Escribimos inodo
//     if (escribir_inodo(ninodo, &inodo) == FALLO)
//     {
//         fprintf(stderr, "Error en escribir_inodo()\n");
//         return FALLO;
//     }
//     // eroores de escritura?
//     if (nbytes == bytesLeidos)
//     {
//         return bytesLeidos;
//     }
//     else
//     {
//         return FALLO;
//     }
// }

int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes)
{
    //Declaraciones
    unsigned int primerBL, ultimoBL;
    int desp1, desp2, nbfisico;
    int bytesleidos = 0;
    int auxByteLeidos = 0;
    char unsigned buf_bloque[BLOCKSIZE];
    struct inodo inodo;

    //Leer el inodo.
    if (leer_inodo(ninodo, &inodo) == FALLO)
    {
        fprintf(stderr, "Error in mi_read_f(): leer_inodo()\n");
        return bytesleidos;
    }

    //Comprobamos que el inodo tenga los permisos para leer
    if ((inodo.permisos & 4) != 4)
    {
        fprintf(stderr, "No hay permisos de lectura!\n\n");
        return bytesleidos;
    }

    if (offset >= inodo.tamEnBytesLog)
    {
        // No podemos leer nada
        return bytesleidos;
    }

    if ((offset + nbytes) >= inodo.tamEnBytesLog)
    { // pretende leer más allá de EOF
        nbytes = inodo.tamEnBytesLog - offset;
        // leemos sólo los bytes que podemos desde el offset hasta EOF
    }

    //Asignaciones de las variables.
    primerBL = offset / BLOCKSIZE;
    ultimoBL = (offset + nbytes - 1) / BLOCKSIZE;

    desp1 = offset % BLOCKSIZE;
    desp2 = (offset + nbytes - 1) % BLOCKSIZE;

    //Obtencion del numero de bloque fisico
    nbfisico = traducir_bloque_inodo(&inodo, primerBL, '0');
    //Caso el cual lo que queremos leer cabe en un bloque fisico
    if (primerBL == ultimoBL)
    {
        if (nbfisico != -1)
        {
            //Leemos el bloque fisico del disco
            auxByteLeidos = bread(nbfisico, buf_bloque);
            if (auxByteLeidos == FALLO)
            {
                fprintf(stderr, "Error mi_read_f(): bread()\n");
                return FALLO;
            }
            memcpy(buf_original, buf_bloque + desp1, nbytes);
        }
        bytesleidos = nbytes;
    }
    //Caso en el que la lectura ocupa mas de un bloque fisico
    else if (primerBL < ultimoBL)
    {
        //Parte 1: Primero bloque leido parcialmente
        if (nbfisico != -1)
        {
            //Leemos el bloque fisico del disco
            auxByteLeidos = bread(nbfisico, buf_bloque);
            if (auxByteLeidos == FALLO)
            {
                fprintf(stderr, "Error mi_read_f(): bread()\n");
                return FALLO;
            }
            memcpy(buf_original, buf_bloque + desp1, BLOCKSIZE - desp1);
        }

        bytesleidos = BLOCKSIZE - desp1;

        //Parte 2: Bloques intermedios
        for (int i = primerBL + 1; i < ultimoBL; i++)
        {
            //Obtenemos los bloques intermedios
            nbfisico = traducir_bloque_inodo(&inodo, i, '0');
            if (nbfisico != -1)
            {
                //Leemos el bloque fisico del disco
                auxByteLeidos = bread(nbfisico, buf_bloque);
                if (auxByteLeidos == FALLO)
                {
                    fprintf(stderr, "Error mi_read_f(): bread()\n");
                    return FALLO;
                }
                memcpy(buf_original + (BLOCKSIZE - desp1) + (i - primerBL - 1) * BLOCKSIZE, buf_bloque, BLOCKSIZE);
            }
            bytesleidos += BLOCKSIZE;
        }

        //Parte 3: Último bloque leido parcialmente
        //Obtenemos el bloque fisico
        nbfisico = traducir_bloque_inodo(&inodo, ultimoBL, '0');
        //Parte 1: Primero bloque leido parcialmente
        if (nbfisico != -1)
        {
            //Leemos el bloque fisico del disco
            auxByteLeidos = bread(nbfisico, buf_bloque);
            if (auxByteLeidos == FALLO)
            {
                fprintf(stderr, "Error mi_read_f(): bread()\n");
                return FALLO;
            }
            //Calculamos el byte lógico del último bloque hasta donde hemos de leer
            memcpy(buf_original + (nbytes - desp2 - 1), buf_bloque, desp2 + 1);
        }
        bytesleidos += desp2 + 1;
    }

    //Leer el inodo actualizado.
    if (leer_inodo(ninodo, &inodo) == FALLO)
    {
        fprintf(stderr, "Error leer_inodo(): mi_read_f()\n");
        return FALLO;
    }

    //Actualizar la metainformación
    inodo.atime = time(NULL);

    //Escribimos inodo
    if (escribir_inodo(ninodo, &inodo) == FALLO)
    {
        fprintf(stderr, "Error escribir_inodo(): mi_read_f()\n");
        return FALLO;
    }

    //Comprobar que no haya errores de escritura y que se haya escrito todo bien.
    if (nbytes == bytesleidos)
    {
#if DEBUGGER
        printf("\tmi_read_f: BIEN\n");
#endif
        return bytesleidos;
    }
    else
    {
#if DEBUGGER
        printf("mi_read_f(): MAL\n\tnbytes:%i\n\tbytesleidos:%i\n", nbytes, bytesleidos);
#endif
        return FALLO;
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
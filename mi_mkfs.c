#include "ficheros_basico.h"

int main(int argc, char **argv)
{

    unsigned int nBloques = atoi(argv[2]);
    unsigned int nInodos = nBloques / 4;

    if (argc != 3)
    {
        fprintf(stderr, "ERROR: Numero de parametros incorrecto. (Correcto: ./mi_mkfs <nombre_dispositivo> <nbloques>)");
    }
    else
    {
        // MONTAMOS EL FICHERO
        bmount(argv[1]);
        // PREPARAMOS UN BLOQUE EL CUAL ESCRIBIREMOS EN EL FICHERO HASTA LLENARLO
        unsigned char buf[BLOCKSIZE];
        memset(buf, '0', BLOCKSIZE);
        // ESCRIBIMOS EL BLOQUE HASTA LLENARLO
        for (int i = 0; i < atoi(argv[2]); i++)
        {
            bwrite(i, buf);
        }
        initSB(nBloques, nInodos);
        initMB();
        initAI();
        // DESMONTAMOS EL FICHERO
        bumount();
    }
}
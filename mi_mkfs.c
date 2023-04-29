// RUBÉN BALLESTEROS JIMÉNEZ, EDUARDO BONNÍN NARVÁEZ, VICENÇ SERVERA FERRER

#include "directorios.h"

int main(int argc, char **argv)
{

    int nbloques = atoi(argv[2]);
    int ninodos = nbloques / 4;

    if (argc != 3)
    {
        fprintf(stderr, ROJO"ERROR: Numero de parametros incorrecto. (Correcto: ./mi_mkfs <nombre_dispositivo> <nbloques>)\n"RESET);
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
        initSB(nbloques, ninodos);
        initMB();
        initAI();

        reservar_inodo('d', 7);

        // DESMONTAMOS EL FICHERO
        bumount();
    }
}
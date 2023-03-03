#include "ficheros_basico.h"
#include <math.h>

double pow(double x, double y);


// CALCULA EL TAMAÑO EN BLOQUES DEL MAPA DE BITS
int tamMB(unsigned int nbloques)
{
    int tamano;
    int mod = (nbloques / 8) % BLOCKSIZE;
    // EN CASO DE QUE EL MODULO SEA 0 EL TAMAÑO EN BLOQUES SERÁ DIRECTAMENTE EL DE LA DIVISIÓN
    if (mod == 0)
    {
        tamano = (nbloques / 8) / BLOCKSIZE;
        // EN CASO CONTRARIO LO SUMAMOS 1 BLOQUE
    }
    else
    {
        tamano = ((nbloques / 8) / BLOCKSIZE) + 1;
    }
    return tamano;
}

// CALCULA EL TAMAÑO EN BLOQEUS DE EL ARRAY DE INODOS
int tamAI(unsigned int ninodos)
{
    int tamano;
    int mod = (ninodos * INODOSIZE) % BLOCKSIZE;
    // EN CASO DE QUE EL MODULO SEA 0 EL TAMAÑO EN BLOQUES SERÁ DIRECTAMENTE EL DE LA DIVISIÓN
    if (mod == 0)
    {
        tamano = (ninodos * INODOSIZE) / BLOCKSIZE;
        // EN CASO CONTRARIO LO SUMAMOS 1 BLOQUE
    }
    else
    {
        tamano = ((ninodos * INODOSIZE) / BLOCKSIZE) + 1;
    }
    return tamano;
}

// INICIALIZA LOS DATOS DEL SUPERBLOQUE
int initSB(unsigned int nbloques, unsigned int ninodos)
{
    // INSTANCIAMOS E INICIALIZAMOS EL SUPERBLOQUE
    struct superbloque SB;
    SB.posPrimerBloqueMB = posSB + tamSB;
    SB.posUltimoBloqueMB = SB.posPrimerBloqueMB + tamMB(nbloques) - 1;
    SB.posPrimerBloqueAI = SB.posUltimoBloqueMB + 1;
    SB.posUltimoBloqueAI = SB.posPrimerBloqueAI + tamAI(ninodos) - 1;
    SB.posPrimerBloqueDatos = SB.posUltimoBloqueAI + 1;
    SB.posUltimoBloqueDatos = nbloques - 1;
    SB.posInodoRaiz = 0;
    SB.posPrimerInodoLibre = 0;
    SB.cantBloquesLibres = nbloques;
    SB.cantInodosLibres = ninodos;
    SB.totBloques = nbloques;
    SB.totInodos = ninodos;

    // ESCRIBMOS EL SUPERBLOQUE EN MEMORIA
    if (bwrite(posSB, &SB) == FALLO)
    {
        return FALLO;
    }
    return EXITO;
}

int initMB()
{
    struct superbloque SB;
    bread(posSB, &SB);
    // NUMERO DE BITS DEL MAPA DE BITS
    int nbits = tamSB + tamMB(SB.totBloques) + tamAI(SB.totInodos);
    // NUMERO DE BYTES DEL MAPA DE BITS
    int bytesMB = (nbits / 8) % BLOCKSIZE;
    // NUMERO DE BYTES SOBRANTES
    int sobrante = nbits % 8;
    // NUMERO DE BLOQUES QUE OBUPA EL MB
    int nbloquesMB = (nbits / 8) / BLOCKSIZE;

    // BUFFER DE TAMAÑO DE 1 BLOQUE
    unsigned char bufferMB[BLOCKSIZE];
    memset(bufferMB, '1', BLOCKSIZE);

    // EN EL CASO DE QUE EL NUMERO DE BLOQUES QUE OCUPA EL MB LOS ESCRIBIRÁ
    for (int i = SB.posPrimerBloqueMB; i < nbloquesMB + SB.posPrimerBloqueMB; i++)
    {
        bwrite(i, bufferMB);
    }

    // RELLENA EL BUFFER CON LOS 1s DESEADOS
    for (int i = 0; i < bytesMB; i++)
    {
        bufferMB[i] = 255;
    }

    // OBTIENE EL NUMERO A ESCRIBIR QUE REPRESENTA EL SOBRANTE
<<<<<<< HEAD
    int decimal_sobrante = 0;
    int pot = 7;
    for (int i = 0; i < sobrante; i++)
    {
        decimal_sobrante += potencia(2, pot);
        pot--;
=======
    double decimal_sobrante = 0.0;
    double potencia = 7.0;
    for (int i = 0; i < sobrante; i++)
    {
        decimal_sobrante += pow(2.0, potencia);
        potencia--;
>>>>>>> 56d4206 (aa)
    }
    bufferMB[bytesMB + 1] = decimal_sobrante;

    // ESCRIBE EL RESTO DE BYTES A 0
    for (int i = bytesMB + 2; i < BLOCKSIZE; i++)
    {
        bufferMB[i] = 0;
    }

    // ESCRIBE LOS BYTES QUE NO OCUPAN UN BLOQUE PER SE.
    bwrite(nbloquesMB + SB.posPrimerBloqueMB, bufferMB);
    SB.cantBloquesLibres -= nbloquesMB;
    return EXITO;
}

//FUNCION AUXILIAR QUE HACE LA POTENCIA DE UN NUMERO
int potencia(int a, int b)
{
    int pot = a;
    for (size_t i = 0; i < b; i++)
    {
        pot *= a;
    }
    return pot;
}

int initAI()
{
    unsigned char bufferAI[BLOCKSIZE];
    memset(bufferAI, '0', BLOCKSIZE);

    struct inodo inodos[BLOCKSIZE / INODOSIZE];

    struct superbloque SB;
    bread(posSB, &SB);

    int salir = 0;
    int contInodos = SB.posPrimerInodoLibre + 1;
    for (int i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI && salir == 0; i++)
    {
        bread(i, inodos);
        for (int j = 0; j < BLOCKSIZE / INODOSIZE; j++)
        {
            inodos[j].tipo = 'l'; // libre
            if (contInodos < SB.totInodos)
            {
                inodos[j].punterosDirectos[0] = contInodos;
                contInodos++;
            }
            else
            {
                inodos[j].punterosDirectos[0] = UINT_MAX;
                salir = 1;
            }
        }
        bwrite(i, inodos);
    }
    return EXITO;
}
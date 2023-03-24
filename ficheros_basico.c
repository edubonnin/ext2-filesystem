#include "ficheros_basico.h"

#define DEBUGN3 1
#define DEBUGN4 1

// FUNCION AUXILIAR QUE HACE LA POTENCIA DE UN NUMERO
int potencia(int a, int b)
{
    int pot = a;
    for (size_t i = 0; i < b; i++)
    {
        pot *= a;
    }
    return pot;
}

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
    // Buffer
    unsigned char buffer[BLOCKSIZE];

    // Todas posiciones a 0.
    if (memset(buffer, 0, BLOCKSIZE) == NULL)
    {
        return EXIT_FAILURE;
    }

    // Leemos el superbloque
    struct superbloque SB;
    if (bread(posSB, &SB) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    // Tamaño MB
    int tamMB = SB.posUltimoBloqueMB - SB.posPrimerBloqueMB;
    // Inicializa bloque a bloque el Mapa de bits
    for (int i = SB.posPrimerBloqueMB; i <= tamMB + SB.posPrimerBloqueMB; i++)
    {
        if (bwrite(i, buffer) == EXIT_FAILURE)
        {
            return EXIT_FAILURE;
        }
    }

    // Ponemos a 1 en el MB los bits que corresponden a los bloques que ocupa el
    // superbloque, el propio MB, y el array de inodos.
    for (unsigned int i = posSB; i < SB.posPrimerBloqueDatos; i++)
    {
        // Podriamos reservar todos los bloques de los metadatos:
        reservar_bloque();
    }

    return EXIT_SUCCESS;
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
    bwrite(posSB, &SB);
    return EXITO;
}

// FUNCIÓN QUE  MODIFICA UN BIT EN EL MB
int escribir_bit(unsigned int nbloque, unsigned int bit)
{
    // LEEMOS EL SUPERBLOQUE PARA OBTENER LA INFORMACIÓN NECESARIA
    struct superbloque SB;
    bread(posSB, &SB);
    // POSICION DEL BYTE QUE CONTIENE EL BIT EN EL MAPA DE BITS
    int posbyte = nbloque / 8;
    // POSICION DENTRO DEL BYTE DEL BIT A MODIFICAR
    int posbit = nbloque % 8;
    // NUMERO DEL BLOQUE EN QUE SE ENCUENTRA EL BIT DENTRO DEL MAPA DE BITS
    int nbloqueMB = posbyte / BLOCKSIZE;
    // NUMERO DEL BLOQUE DONDE SE ENCUENTRA EL BIT A MODIFICAR DENTRO DE EL SISTEMA
    int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB;
    // BUFFER PARA LEER EL BLOQUE A MODIFICAR
    unsigned char bufferMB[BLOCKSIZE];
    bread(nbloqueabs, bufferMB);

    // POSICIÓN DEL BYTE A MODIFICAR DENTRO DEL BLOQUE
    posbyte = posbyte % BLOCKSIZE;
    // MASCARA A APLICAR EN EL BYTE A MODIFICAR
    unsigned char mascara = 128;
    mascara >>= posbit;

    if (bit == 1)
    {
        bufferMB[posbyte] |= mascara;
    }
    else
    {
        bufferMB[posbyte] &= ~mascara;
    }
    if (bwrite(nbloqueabs, bufferMB) == FALLO)
    {
        return FALLO;
    }
    return EXITO;
}

// FUNCION QUE LEE EL BIT DE MB
char leer_bit(unsigned int nbloque)
{
    // LECTURA DEL SUPERBLOQUE
    struct superbloque SB;
    bread(posSB, &SB);
    // POSICION DEL BYTE QUE CONTIENE EL BIT EN EL MAPA DE BITS
    int posbyte = nbloque / 8;
    // POSICION DENTRO DEL BYTE DEL BIT A MODIFICAR
    int posbit = nbloque % 8;
    // NUMERO DEL BLOQUE EN QUE SE ENCUENTRA EL BIT DENTRO DEL MAPA DE BITS
    int nbloqueMB = posbyte / BLOCKSIZE;
    // NUMERO DEL BLOQUE DONDE SE ENCUENTRA EL BIT A MODIFICAR DENTRO DE EL SISTEMA
    int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB;
    // BUFFER PARA LEER EL BLOQUE A MODIFICAR
    unsigned char bufferMB[BLOCKSIZE];
    // LECTURA DEL BLOQUE A LEER
    bread(nbloqueabs, bufferMB);
    // POSCION DEL BYTE A LEER
    posbyte = posbyte % BLOCKSIZE;
    unsigned char mascara = 128;  // 10000000
    mascara >>= posbit;           // DESPLAZAMIENTO DE BITS A LA DERECHA
    mascara &= bufferMB[posbyte]; // OPERADOR AND
    mascara >>= (7 - posbit);     // DESPLAZAMINETO DE BITS A LA DERECHA Y GUARDAR EL REASULTADO DEL BIT

#if DEBUGN3
    printf(GRIS "[leer_bit(%i) → posbyte:%i, posbit:%i, nbloqueMB:%i, nbloqueabs:%i)]\n" RESET, nbloque, posbyte, posbit, nbloqueMB, nbloqueabs);
#endif

    // DEVOLUCION DEL BIT LEIDO
    return mascara;
}

int reservar_bloque()
{
    int nbloque;
    int bloquesllenos = 0;
    struct superbloque SB;
    bread(posSB, &SB);

    if (SB.cantBloquesLibres <= 0)
    {
        return FALLO;
    }

    unsigned char bufferMB[BLOCKSIZE];
    unsigned char bufferAux[BLOCKSIZE];

    memset(bufferAux, 255, BLOCKSIZE);

    // NOS SITUAMOS A LA PRIMERA POSICION DEL MB
    int nbloqueabs = SB.posPrimerBloqueMB;
    // LEEMOS EL PRIMER BLOQUE
    bread(nbloqueabs, bufferMB);

    // MIENTRAS LOS BLOQUES DEL MAPA DE BITS NO CONTENGAN BLOQUES LIBRES HAREMOS LAS SIGUIENTES OPERACIONES
    while (memcmp(bufferMB, bufferAux, BLOCKSIZE) == 0)
    {
        // NOS MOVENOS AL SIGUIENTE BLOQUE DEL MB
        nbloqueabs++;
        // AUMENTAMOS EN UNO EL NUMERO DE BLOQUES LEIDOS
        bloquesllenos++;
        // LEEMOS EL SIGUIENTE BLOQUE
        bread(nbloqueabs, bufferMB);
    }

    // POSICION DEL PRIMER BYTE DIFERENTE A 255(1111 1111) DENTRO DEL BLOQUE DEL MB
    int posbyte = 0;

    while (bufferMB[posbyte] == 255)
    {
        posbyte++;
    }

    unsigned char mascara = 128; // 10000000 & 000000
    int posbit = 0;
    while (bufferMB[posbyte] & mascara)
    {                            // operador AND para bits
        bufferMB[posbyte] <<= 1; // desplazamiento de bits a la izquierda
        posbit++;
    }

    nbloque = ((nbloqueabs - SB.posPrimerBloqueMB) * BLOCKSIZE + posbyte) * 8 + posbit;
    escribir_bit(nbloque, 1);
    SB.cantBloquesLibres--;
    memset(bufferAux, 0, BLOCKSIZE); // Volvmemos a inicializar a 0
    bwrite(SB.posPrimerBloqueDatos + nbloque - 1, bufferAux);
    bwrite(posSB, &SB);

    return nbloque; // NUMERO DE BLOQUE RESERVADO ((== PRIMER BLOQUE LIBRE)
}

int liberar_bloque(unsigned int nbloque)
{
    // LEEMOS EL SUPERBLOQUE PARA OBTENER LA INFORMACIÓN DEL SISTEMA
    struct superbloque SB;
    bread(posSB, &SB);
    // ESCRIBIMOS EL BIT CORRESPONDIENTE AL BLOQUE A 0
    escribir_bit(nbloque, 0);
    // AUMENTAMOS LA CANTIDAD DE BLOQUES LIBRES
    SB.cantBloquesLibres++;
    bwrite(posSB, &SB);

    return nbloque;
}

int escribir_inodo(unsigned int ninodo, struct inodo *inodo)
{
    // LEEMOS EL SUPERBLOQUE PARA OBTENER LA INFORMACIÓN DEL SISTEMA
    struct superbloque SB;
    bread(posSB, &SB);
    unsigned int numerobloque = SB.posPrimerBloqueAI + (ninodo / (BLOCKSIZE / INODOSIZE));
    struct inodo inodos[BLOCKSIZE / INODOSIZE];
    bread(numerobloque, inodos);
    inodos[ninodo % (BLOCKSIZE / INODOSIZE)] = *inodo; // posible error
    if (bwrite(numerobloque, inodos) == FALLO)
    {
        return FALLO;
    }
    return EXITO;
}

int leer_inodo(unsigned int ninodo, struct inodo *inodo)
{
    // LEEMOS EL SUPERBLOQUE PARA OBTENER LA INFORMACIÓN DEL SISTEMA
    struct superbloque SB;
    bread(posSB, &SB);

    struct inodo inodos[BLOCKSIZE / INODOSIZE];

    // BLOQUE EN EL QUE SE ENCUENTRA EL INODO EN EL ARRAY DE INODOS
    int nbloque = (ninodo * INODOSIZE) / BLOCKSIZE;
    // BLOQUE ABSOLUTO EN EL QUE SE ENCUENTRA
    int nbloqueabs = SB.posPrimerBloqueAI + nbloque;
    int err = bread(nbloqueabs, &inodos);
    *inodo = inodos[ninodo % (BLOCKSIZE / INODOSIZE)];

    if (err == FALLO)
    {
        return FALLO;
    }
    return EXITO;
}

int reservar_inodo(unsigned char tipo, unsigned char permisos)
{
    struct superbloque SB;
    bread(posSB, &SB);
    if (SB.cantInodosLibres <= 0)
    {
        return FALLO;
    }
    int posInodoReservado = SB.posPrimerInodoLibre;
    SB.posPrimerInodoLibre++;
    struct inodo inodo;
    inodo.tipo = tipo;
    inodo.permisos = permisos;
    inodo.nlinks = 1;
    inodo.tamEnBytesLog = 0;
    inodo.atime = time(NULL);
    inodo.mtime = time(NULL);
    inodo.ctime = time(NULL);
    inodo.numBloquesOcupados = 0;
    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            inodo.punterosIndirectos[j] = 0;
        }
        inodo.punterosDirectos[i] = 0;
    }

    escribir_inodo(posInodoReservado, &inodo);
    SB.cantInodosLibres--;
    bwrite(posSB, &SB);
    return posInodoReservado;
}

int obtener_nRangoBL(struct inodo *inodo, unsigned int nblogico, unsigned int *ptr)
{
    if (nblogico < DIRECTOS) // MIRAMOS SI ESTA SITUADO EN LOS PUNTEROS DIRECTOS
    {
        *ptr = inodo->punterosDirectos[nblogico];
        return 0;
    }
    else if (nblogico < INDIRECTOS0)
    { // MIRAMOS SI ESTA SITUADO EN PUNTEROS INDIRECTOS0
        *ptr = inodo->punterosIndirectos[0];
        return 1;
    }
    else if (nblogico < INDIRECTOS1)
    { // MIRAMOS SI ESTA SITUADO EN PUNTEROS INDIRECTOS1
        *ptr = inodo->punterosIndirectos[1];
        return 2;
    }
    else if (nblogico < INDIRECTOS2)
    { // MIRAMOS SI ESTA SITUADO EN PUNTEROS INDIRECTOS2
        *ptr = inodo->punterosIndirectos[2];
        return 3;
    }
    else
    { // EN CASO DE QUE NO ESTE SITUADO EN NINGUN RANGO ANTERIOR, SE PRODUCE UN ERROR
        *ptr = 0;
        fprintf(stderr, "Bloque lógico fuera de rango");
        return -1;
    }
}

int obtener_indice(unsigned int nblogico, int nivel_punteros)
{
    if (nblogico < DIRECTOS)
    { // COMPROBACIÓN SI SE TRATA DE UN BLOQUE DIRECTO, EN ESE CASO DEVOLVEMOS NBLOGICO
        return nblogico;
    }
    else if (nblogico < INDIRECTOS0)
    { // COMPROBACIÓN SI SE TRATA DE UN BLOQUE INDIRECTO0, EN ESE CASO DEVOLVEMOS NBLOGICO - DIRECTOS
        return (nblogico - DIRECTOS);
    }
    else if (nblogico < INDIRECTOS1)
    { // COMPROBACIÓN SI SE TRATA DE UN BLOQUE INDIRECTO1, EN ESE CASO COMPROBAMOS EL NIVEL EN EL QUE SE ENCUENTRA
        if (nivel_punteros == 2)
        { // Y DEVOLVEMOS EL VALOR CORRESPONDIENTE AL NIVEL
            return (nblogico - INDIRECTOS0) / NPUNTEROS;
        }
        else if (nivel_punteros == 1)
        {
            return (nblogico - INDIRECTOS0) % NPUNTEROS;
        }
    }
    else if (nblogico < INDIRECTOS2)
    { // COMPROBACIÓN SI SE TRATA DE UN BLOQUE INDIRECTO2, EN ESE CASO COMPROBAMOS EL NIVEL EN EL QUE SE ENCUENTRA
        if (nivel_punteros == 3)
        { // Y DEVOLVEMOS EL VALOR CORRESPONDIENTE AL NIVEL
            return (nblogico - INDIRECTOS1) / (NPUNTEROS * NPUNTEROS);
        }
        else if (nivel_punteros == 2)
        {
            return ((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) / NPUNTEROS;
        }
        else if (nivel_punteros == 1)
        {
            return ((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) % NPUNTEROS;
        }
    }
    return FALLO;
}

int traducir_bloque_inodo(struct inodo *inodo, unsigned int nblogico, unsigned char reservar)
{
    // Descomentar para saltos de línea entre los debugs de distintas llamadas a la función
    // #if DEBUGN4
    //     printf("\n");
    // #endif

    unsigned int ptr, ptr_ant;
    int nRangoBL, nivel_punteros, index;
    unsigned int buffer[NPUNTEROS];

    ptr = 0;
    ptr_ant = 0;
    nRangoBL = obtener_nRangoBL(inodo, nblogico, &ptr);

    if (nRangoBL == -1)
    {
        return FALLO;
    }

    nivel_punteros = nRangoBL;
    // EN CASO DE QUE NO SEA UN PUNTERO DIRECTO
    while (nivel_punteros > 0)
    {
        // SI NO TIENE UN BLOQUE APUNTADO
        if (ptr == 0)
        {
            // SI NO TENEMOS QUE RESERVARLO DEVOLVEMOS -1
            if (reservar == '0')
            {
                return FALLO;
                // EN CASO DE QUE SÍ, LO RESERVAMOS
            }
            else
            {
                ptr = reservar_bloque();
                inodo->numBloquesOcupados++;
                inodo->ctime = time(NULL);

                if (nivel_punteros == nRangoBL)
                {
                    inodo->punterosIndirectos[nRangoBL - 1] = ptr;
#if DEBUGN4
                    printf(GRIS "[traducir_bloque_inodo()→ inodo.punterosIndirectos[%i] = %i (reservado BF %i para punteros_nivel%i)]\n" RESET,
                           nRangoBL - 1, ptr, ptr, nivel_punteros);
#endif
                }
                else
                {
                    buffer[index] = ptr;
#if DEBUGN4
                    printf(GRIS "[traducir_bloque_inodo()→ inodo.punteros_nivel%i[%i] = %i (reservado BF %i para punteros_nivel%i)]\n" RESET,
                           nivel_punteros + 1, index, ptr, ptr, nivel_punteros); // nivel_punteros+1 para hacer referencia al nivel de punteros anterior
#endif
                    bwrite(ptr_ant, buffer);
                }
                memset(buffer, 0, BLOCKSIZE);
            }
            // EN CASO DE QUE EXISTA LO LEEMOS
        }
        else
        {
            bread(ptr, buffer);
        }
        // OBTENEMOS EL IDICE DEL SIGUIENTE NIVEL
        index = obtener_indice(nblogico, nivel_punteros);
        ptr_ant = ptr;
        ptr = buffer[index];
        // DECREMENTAMOS EL NIVEL
        nivel_punteros--;
    }

    // CUANDO EL NIVEL DE PUNTEROS ES DIRECTAMENTE LOS DATOS
    // SI NO TENEMOS UN BLOQUE ASIGNADO
    if (ptr == 0)
    {
        // SI NO LO QUEREMOS RESERVAR DEVOLVEMOS -1
        if (reservar == '0')
        {
            return FALLO;
            // SI LO QUEREMOS RESERVAR LO GESTIONAMOS
        }
        else
        {
            ptr = reservar_bloque();
            inodo->numBloquesOcupados++;
            inodo->ctime = time(NULL);
            // EN CASO DE QUE FUESE UN PUNTERO DIRECTO LO ASIGNAMOS A PUNTEROSDIRECTOS
            if (nRangoBL == 0)
            {
                inodo->punterosDirectos[nblogico] = ptr;
#if DEBUGN4
                printf(GRIS "[traducir_bloque_inodo()→ inodo.punterosDirectos[%i] = %i (reservado BF %i para BL %i)]\n" RESET,
                       nblogico, ptr, ptr, nblogico);
#endif
                // EN CASO CONTRARIO ASIGNAMOS EL PUNTERO A UN BLOQUE DE PUNTEROS
            }
            else
            {
                buffer[index] = ptr;
#if DEBUGN4
                printf(GRIS "[traducir_bloque_inodo()→ inodo.punteros_nivel1[%i] = %i (reservado BF %i para BL %i)]\n" RESET,
                       index, ptr, ptr, nblogico);
#endif
                bwrite(ptr_ant, buffer);
            }
        }
    }
    return ptr;
}

int liberar_inodo(unsigned int ninodo)
{

    int liberados;

    struct inodo inodo;
    leer_inodo(ninodo,&inodo);

    struct superbloque SB;
    bread(posSB,&SB);

    liberados=liberar_bloques_inodo(0,&inodo);

    inodo.numBloquesOcupados-=liberados;
    inodo.tipo='l';
    inodo.tamEnBytesLog=0;

    // ACTUALIZACIÓN LISTA ENLAZADA INODOS LIBRES
    inodo.punterosDirectos[0]=SB.posPrimerInodoLibre;
    SB.posPrimerInodoLibre=ninodo;
    SB.cantInodosLibres++;

    bwrite(posSB,&SB);

    inodo.ctime=time(NULL);
    escribir_inodo(ninodo,&inodo);
    return ninodo;
}

int liberar_bloques_inodo(unsigned int primerBL, struct inodo *inodo)
{

    unsigned int nivel_punteros, indice, ptr = 0, nBL, ultimoBL;
    int nRangoBL;
    unsigned int bloques_punteros[3][NPUNTEROS];
    unsigned char bufAux_punteros[BLOCKSIZE];
    int ptr_nivel[3];
    int indices[3];
    int liberados=0;

    if (inodo->tamEnBytesLog == 0)
    {
        return liberados;
    }

    if (inodo->tamEnBytesLog % BLOCKSIZE == 0)
    {
        ultimoBL = (inodo->tamEnBytesLog / BLOCKSIZE) - 1;
    }
    else
    {
        ultimoBL = inodo->tamEnBytesLog / BLOCKSIZE;
    }

    memset(bufAux_punteros, 0, BLOCKSIZE);

    for (nBL = primerBL; nBL == ultimoBL; nBL++)
    {
        nRangoBL = obtener_nRangoBL(inodo, nBL, &ptr);
        if (nRangoBL < 0)
        {
            return FALLO;
        }
        nivel_punteros = nRangoBL;
        while (ptr > 0 && nivel_punteros > 0)
        {
            indice = obtener_indice(nBL, nivel_punteros);
            if (indice == 0 || nBL == primerBL)
            {
                bread(ptr, bloques_punteros[nivel_punteros - 1]);
            }
            ptr_nivel[nivel_punteros - 1] = ptr;
            indices[nivel_punteros - 1] = indice;
            ptr = bloques_punteros[nivel_punteros - 1][indice];
            nivel_punteros--;
        }

        if (ptr > 0)
        {
            liberar_bloque(ptr);
            liberados++;
            if (nRangoBL == 0)
            {
                inodo->punterosDirectos[nBL] = 0;
            }
            else
            {
                nivel_punteros = 1;
                while (nivel_punteros <= nRangoBL)
                {
                    indice = indices[nivel_punteros - 1];
                    bloques_punteros[nivel_punteros - 1][indice] = 0;
                    ptr = ptr_nivel[nivel_punteros - 1];
                    if (memcmp(bloques_punteros[nivel_punteros - 1], bufAux_punteros, BLOCKSIZE) == 0)
                    {
                        liberar_bloque(ptr);
                        liberados++;
                        //mejora 
                        
                        
                        
                        //fin mejora
                        if (nivel_punteros==nRangoBL)
                        {
                            inodo->punterosIndirectos[nRangoBL-1]=0;
                        }
                        nivel_punteros++;
                    }
                    else
                    {
                        bwrite(ptr, bloques_punteros[nivel_punteros - 1]);
                        nivel_punteros = nRangoBL + 1;
                    }
                }
            }
        }
        else
        {
            // mejora saltando los bloques que no sea necesario explorar  al valer 0 un puntero 
        }
    }
    
    return liberados;
}
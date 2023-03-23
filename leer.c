/*
PROGRAMA *** leer.c ***
./leer
*/

#include "ficheros.h"

#define tambuffer 1500
#define DEBUG 1

/**
 * ---------------------------------------------------------------------
 *                          leer.c:
 * ---------------------------------------------------------------------
 *
 * Programa externo ficticio sólo para probar, temporalmente,
 * la funcionalidad de lectura.
 *
 * Le pasaremos por línea de comandos un nº de inodo obtenido con el
 * programa anterior (escribir.c). ninodo, (además del nombre del
 * dispositivo). Su funcionamiento tiene que ser similar al comando
 * cat de Linux, explorando TODO el fichero.
 *
 */

int main(int argc, char const *argv[])
{
    // Variables
    int ninodo;
    struct superbloque SB;
    struct inodo inodo;
    // Para lectura
    int offset = 0;
    int bytesleidos = 0;
    char buffer[tambuffer];

    // Sintaxis correcta
    if (argc != 3)
    {
        fprintf(stderr, "Sintaxis: leer <nombre_dispositivo><numero_inodo>\n");
        return FALLO;
    }

    // Inicializacion del buffer a 0.
    memset(buffer, 0, tambuffer);
    ninodo = atoi(argv[2]);
    // Montar el dispositivo en el sistema.
    if (bmount(argv[1]) == -1)
    {
        fprintf(stderr, "leer.c: Error al montar el dispositivo.\n");
        return FALLO;
    }

    // Leer superbloque
    if (bread(0, &SB) == FALLO)
    {
        fprintf(stderr, "leer.c: Error de lectura del superbloque.\n");
        return FALLO;
    }

    // Lee del fichero hasta llenar el buffer o fin de fichero.
    int auxBytesLeidos = mi_read_f(ninodo, buffer, offset, sizeof(buffer));
    while (auxBytesLeidos > 0)
    {
        bytesleidos += auxBytesLeidos;
        // Escribe el contenido del buffer en el destino indicado.
        write(1, buffer, auxBytesLeidos);

        // Limpiar buffer
        memset(buffer, 0, tambuffer);
        // Actulizar offset
        offset += tambuffer;
        // Leemos otra vez
        auxBytesLeidos = mi_read_f(ninodo, buffer, offset, sizeof(buffer));
    }

    // Leer el inodo del archivo
    if (leer_inodo(ninodo, &inodo))
    {
        fprintf(stderr, "Error con la lectura del inodo.\n");
        return FALLO;
    }

#if DEBUG
    fprintf(stderr, "\ntotal_bytesleidos: %d\ntamEnBytesLog: %d\n", bytesleidos, inodo.tamEnBytesLog);
#endif

    // Desmonta el dispositivo virtual
    if (bumount() == FALLO)
    {
        fprintf(stderr, "leer.c: Error al desmonta el dispositivo virtual.\n");
        return FALLO;
    }
    return EXITO;
}
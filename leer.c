// RUBÉN BALLESTEROS JIMÉNEZ, EDUARDO BONNÍN NARVÁEZ, VICENÇ SERVERA FERRER

#include "ficheros.h"

#define tambuffer 1500
#define DEBUG 1

int main(int argc, char const *argv[])
{
    int ninodo;
    struct superbloque SB;
    struct inodo inodo;
    int offset = 0;
    int bytesleidos = 0;
    char buffer[tambuffer];

    // SINTAXIS CORRECTA
    if (argc != 3)
    {
        fprintf(stderr, "Sintaxis: leer <nombre_dispositivo><numero_inodo>\n");
        return FALLO;
    }

    // INICIALIZACION DEL BUFFER
    memset(buffer, 0, tambuffer);
    ninodo = atoi(argv[2]);
    // MONTAR EL DISPOSITIVO
    if (bmount(argv[1]) == -1)
    {
        fprintf(stderr, "leer.c: Error al montar el dispositivo.\n");
        return FALLO;
    }

    // LECTURA DEL SUPERBLOQUE
    if (bread(0, &SB) == FALLO)
    {
        fprintf(stderr, "leer.c: Error de lectura del superbloque.\n");
        return FALLO;
    }

    // LECTURA DEL FICHERO HASTA LLEGAR AL FINAL
    int auxBytesLeidos = mi_read_f(ninodo, buffer, offset, sizeof(buffer));
    while (auxBytesLeidos > 0)
    {
        bytesleidos += auxBytesLeidos;
        // ESCRITURA DEL BUFFER
        write(1, buffer, auxBytesLeidos);

        // LIMPIEZA DEL BUFFER
        memset(buffer, 0, tambuffer);
        // ACTUALIZACIÓN DEL OFFSET
        offset += tambuffer;
        // LECTURA
        auxBytesLeidos = mi_read_f(ninodo, buffer, offset, sizeof(buffer));
    }

    // LECTURA DEL INODO DEL ARCHIVO
    if (leer_inodo(ninodo, &inodo))
    {
        fprintf(stderr, "Error con la lectura del inodo.\n");
        return FALLO;
    }

#if DEBUG
    fprintf(stderr, "\ntotal_leidos: %d\ntamEnBytesLog: %d\n", bytesleidos, inodo.tamEnBytesLog);
#endif

    // SE DESMONTA EL DISPOSITIVO
    if (bumount() == FALLO)
    {
        fprintf(stderr, "leer.c: Error al desmonta el dispositivo virtual.\n");
        return FALLO;
    }
    return EXITO;
}
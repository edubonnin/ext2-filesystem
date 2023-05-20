#include "directorios.h"

#define tambuffer 1500

int main(int argc, char const *argv[])
{
    char buffer[tambuffer];
    memset(buffer, 0, sizeof(buffer));

    // SINTAXIS CORRECTA
    if (argc != 3)
    {
        fprintf(stderr, ROJO "Sintaxis: ./mi_cat <disco> </ruta_fichero>\n" RESET);
        return FALLO;
    }

    // COMPROBAMOS QUE SE TRATA DE UNA RUTA QUE ES UN FICHERO Y NO UN DIRECTORIO
    if ((argv[2][strlen(argv[2]) - 1]) == '/')
    {
        fprintf(stderr, ROJO "mi_cat.c: La ruta no se trata de un fichero.\n" RESET);
        return FALLO;
    }

    if (bmount(argv[1]) == FALLO)
    {
        return FALLO;
    }

    int offset = 0;
    int totalBytesLeidos = 0;
    int bytesLeidos = mi_read(argv[2], buffer, offset, sizeof(buffer));

    while (bytesLeidos > 0)
    {
        totalBytesLeidos += bytesLeidos;
        write(1, buffer, bytesLeidos); // ESCRIBE EN LA SALIDA ESTÁNDAR (stdout)
        memset(buffer, 0, sizeof(buffer));
        offset += tambuffer;
        bytesLeidos = mi_read(argv[2], buffer, offset, sizeof(buffer));
    }

    if (totalBytesLeidos < 0)
    {
        mostrar_error_buscar_entrada(totalBytesLeidos);
        bytesLeidos = 0;
    }

    // IMPRIMIMOS RESULTADO POR EL stderr PARA EVITAR POSIBLES REDIRECCIONES A FICHEROS
    fprintf(stderr, "\n\nTotal_leidos %d\n", totalBytesLeidos);

    // SE DESMONTA EL DISPOSITIVO
    if (bumount() == FALLO)
    {
        return FALLO;
    }

    return EXITO;
}
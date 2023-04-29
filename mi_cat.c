#include "directorios.h"

#define tambuffer 1500

int main(int argc, char const *argv[])
{
    // SINTAXIS CORRECTA
    if (argc != 3)
    {
        fprintf(stderr, "Sintaxis: ./mi_cat <disco> </ruta_fichero>\n");
        return FALLO;
    }

    char buffer[tambuffer];

    if (bmount(argv[1]) == FALLO)
    {
        fprintf(stderr, "mi_cat.c: Error al montar el dispositivo.\n");
        return FALLO;
    }

    //COMPROBAMOS QUE SE TRATA DE UNA RUTA QUE ES UN FICHERO Y NO UN DIRECTORIO
    if((argv[2] + strlen(argv[2]) - 1) == '/'){
        fprintf(stderr, "mi_cat.c: La ruta no se trata de un fichero.\n");
        return FALLO;
    }


    int offset = 0;
    int totalBytesLeidos = 0;
    int bytesLeidos = mi_read(argv[2], buffer, offset, sizeof(buffer)); //se tiene que leer todo el fichero, lo que no se extraer el tamaño sin usar otras funciones 
    //lo que he pensado es llamar a buscar entrada para obtener el inodo, leer el inodo y pasarle como parametro el tamenbyteslog, 
    //lo que no se si a este nivel se puedehacer eso
    //otra opcion es hacerlo como en el leer.c que hace un bucle con varias llamada hasta el final
    while (bytesLeidos > 0)
    {
        offset += tambuffer;
        totalBytesLeidos += bytesLeidos;
        memset(buffer, 0, tambuffer);
        bytesLeidos = mi_read(argv[2], buffer, offset, sizeof(buffer));
    }

    printf("Bytes leidos: %d\n", totalBytesLeidos);

    // SE DESMONTA EL DISPOSITIVO
    if (bumount() == FALLO)
    {
        fprintf(stderr, "mi_cat.c: Error al desmonta el dispositivo virtual.\n");
        return FALLO;
    }

    return EXITO;

}

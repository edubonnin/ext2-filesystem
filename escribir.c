/*
PROGRAMA *** escribir.c ***
./ escribir disco "$(cat text2.txt)" 0 --> en cada uno de los offsets del mismo inodo
                              1 --> diferentes inodos (reservar inodo cada vez que se quiere cambiar de offset)
*/

/**
 * ---------------------------------------------------------------------
 *                          escribir.c:
 * ---------------------------------------------------------------------
 *
 * Programa externo ficticio sólo para probar, temporalmente,
 * la funcionalidad de escritura.
 *
 * Escribirá texto en uno o varios inodos haciendo uso de
 * reservar_inodo('f', 6) para obtener un nº de inodo, ninodo, que
 * mostraremos por pantalla y además utilizaremos como parámetro
 * para mi_write_f().
 *
 * Para indicar el texto a escribir tenéis varias opciones a escoger:
 *
 * 1- Pasarlo como argumento escribiéndolo en consola y utilizar la función
 * strlen() para calcular su longitud.
 *
 * 2- Pasarlo como argumento haciendo el cat de cualquier fichero, por ejemplo
 * un fichero.c de vuestra práctica de la siguiente manera:
 *      “$(cat dir_practica/fichero.c)”
 *
 * 3- Asignarlo a un buffer desde código de esta manera:
 *      char buffer[tamanyo];
 *      strcpy (buffer, "blablabla...");
 *
 * 4- Pasar como argumento el nombre de un fichero externo que contenga el texto
 */

#include <stdlib.h>
#include "ficheros.h"

#define DEBUG 1     // Debugger a enseñar tamaños logicos & bloques ocupados
#define DEBUGREAD 0 // Debugger que lee al escribir

int main(int argc, char *argv[])
{
    // Consula de sintaxis correcta
    if (argc != 4)
    {
        fprintf(stderr, "Error Sintaxis: escribir <nombre_dispositivo> <\"$(cat fichero)\"> <diferentes_inodos>\n");
        return FALLO;
    }

    printf("longitud texto: %ld\n\n", strlen(argv[2]));
    int offsets[5] = {9000, 209000, 30725000, 409605000, 480000000};

    // Monta el dispositivo virtual en el sistema.
    if (bmount(argv[1]) < 0)
    {
        fprintf(stderr, "escribir.c: Error al montar el dispositivo virtual.\n");
        return FALLO;
    }

    // Reserva un inodo para la escritura y se comprueba.
    int ninodo = reservar_inodo('f', 6);
    if (ninodo == FALLO)
    {
        fprintf(stderr, "escribir.c: Error al reservar el inodo.\n");
        return FALLO;
    }

    // Bucle de escritura en todos los offsets del array.
    for (int i = 0; i < (sizeof(offsets) / sizeof(int)); i++)
    {

        printf("Nº inodo reservado: %d\n", ninodo);
        printf("offset: %d\n", offsets[i]);
        int bytesescritos = mi_write_f(ninodo, argv[2], offsets[i], strlen(argv[2]));
        if (bytesescritos == FALLO)
        {
            fprintf(stderr, "escribir.c: Error mi_write_f().\n");
            return FALLO;
        }
        printf("Bytes escritos: %d\n\n", bytesescritos);

#if DEBUGREAD
        int longitud = strlen(argv[2]);
        char *buffer_texto = malloc(longitud);
        memset(buffer_texto, 0, longitud);
        int leidos = mi_read_f(ninodo, buffer_texto, offsets[i], longitud);
        printf("Bytes leídos: %d\n", leidos);
#endif

        // Obtencion de la información del inodo escrito
        struct STAT p_stat;
        if (mi_stat_f(ninodo, &p_stat))
        {
            fprintf(stderr, "escribir.c: Error mi_stat_f()\n");
            return FALLO;
        }

        printf("stat.tamEnBytesLog = %d\n", p_stat.tamEnBytesLog);
        printf("stat.numBloquesOcupados = %d\n\n", p_stat.numBloquesOcupados);

        // Si el parámetro <diferentes_indodos> es 0, reserva un nuevo inodo.
        if (strcmp(argv[3], "0"))
        {
            ninodo = reservar_inodo('f', 6);
            if (ninodo == FALLO)
            {
                fprintf(stderr, "escribir.c: Error al reservar el inodo.\n");
                return FALLO;
            }
        }
    }

    // Desmonta el dispositivo virtual
    if (bumount() < 0)
    {
        fprintf(stderr, "escribir.c: Error al desmonta el dispositivo virtual.\n");
        return FALLO;
    }
    return EXIT_SUCCESS;
}